/* MIT License
 * Copyright (c) 2016 Jae Choi
 * STM32H7S3L8 Port - 2024
 * …
 */

#include "main.h"
#include <stdio.h>   /* sprintf / snprintf */

/* --------------------------------------------------------------------------
 *  EXTERN / GLOBALS
 * ------------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;          /* HAL SPI handle (SPI1)          */
extern uint32_t BIOEXG_SETTINGS;         /* BİO-EXG çalışma bayrakları     */

uint8_t  spi_tx_buffer[32]   __attribute__((section(".SRAM2")));
uint8_t  spi_rx_buffer[32]   __attribute__((section(".SRAM2")));
uint8_t  ads1299_data_buffer[27] __attribute__((section(".AXI_SRAM"))); /* 3 status + 24 data */

/* --------------------------------------------------------------------------
 *  HELPER – kaba döngü gecikmesi (550 MHz için ~n µs)
 * ------------------------------------------------------------------------*/
static inline void delay_cycles(int cycles)
{
    for (volatile int d = 0; d < cycles; ++d) { __NOP(); }
}

/* --------------------------------------------------------------------------
 *  1)  ADS1299  INITIALISATION
 * ------------------------------------------------------------------------*/
void ads1299_init(void)
{
    UART_Transmit("Starting ADS1299 Power-up sequence...\n");
    ads1299_pwr_up_seq();
    HAL_Delay(1);                        /* 1 ms */
    ads1299_pwr_up_seq();
    UART_Transmit("Power-up sequence completed\n");

    /* ----- Konfigürasyon öncesi dönemi durdur ----- */
    UART_Transmit("Sending SDATAC...\n");
    ads1299_stop_dataread();

    /* ----- WHO-AMI? ----- */
    if (ads1299_read_reg(ID) == 0x3E) {
        UART_Transmit("ADS1299 ID verified – OK\n");
    } else {
        UART_Transmit("ERROR: ADS1299 ID mismatch – HALT\n");
        while (1) {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
            HAL_Delay(500);
        }
    }

    /* ----- EMG konfigürasyonu ----- */
    ads1299_write_reg(CONFIG1, 0x94);   /* 1 kSPS                            */
    ads1299_write_reg(CONFIG3, 0x60 | (1u << 7));     /* REF buf ON, BIAS buf OFF */
    ads1299_write_reg(CONFIG2, 0xC0 | ADS1299_TEST_INT | ADS1299_TESTSIGNAL_PULSE_FAST);

    const uint8_t ch_default =
        ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN;

    for (uint8_t ch = 0; ch < 8; ++ch)
        ads1299_write_reg(CH1SET + ch, ch_default);

    ads1299_write_reg(LOFF,  LOFF_FREQ_FS_4);     /* AC lead-off 62.5 Hz */
    ads1299_write_reg(MISC1, 1u << 5);            /* SRB1 hepsine bağla  */

#ifdef DEBUG_ADS1299
    char dbg[64];
    sprintf(dbg, "CONFIG3 (exp 0xE0) = 0x%02X\n", ads1299_read_reg(CONFIG3));  UART_Transmit(dbg);
    sprintf(dbg, "CONFIG2             = 0x%02X\n", ads1299_read_reg(CONFIG2)); UART_Transmit(dbg);
    sprintf(dbg, "CH8SET              = 0x%02X\n", ads1299_read_reg(CH8SET));  UART_Transmit(dbg);
#endif

    ads1299_configure_dma();

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);   /* START */
    UART_Transmit("ADS1299 init OK – conversion started\n");
}

/* --------------------------------------------------------------------------
 *  2)  LOW-LEVEL COMMANDS
 * ------------------------------------------------------------------------*/
void ads1299_pwr_up_seq(void)
{
    HAL_Delay(40);                                            /* t_pwd */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);     /* RESET low  */
    delay_cycles(1100);                                       /* ≈2 µs       */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);       /* RESET high */
    delay_cycles(5500);                                       /* ≈10 µs      */
}

void ads1299_stop_dataread(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);   /* CS low  */
    SPI_TX(_SDATAC);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);     /* CS high */
    delay_cycles(550);                                      /* 2 tCLK  */
}

void ads1299_read_data(uint32_t *status, int32_t *data)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    SPI_NO_DELAY_TX(_RDATA);

    *status  = (uint32_t)SPI_NO_DELAY_TX(0x00) << 16;
    *status |= (uint32_t)SPI_NO_DELAY_TX(0x00) << 8;
    *status |= (uint32_t)SPI_NO_DELAY_TX(0x00);

    for (uint8_t ch = 0; ch < 8; ++ch) {
        int32_t tmp  = (int32_t)SPI_NO_DELAY_TX(0x00) << 16;
        tmp         |= (int32_t)SPI_NO_DELAY_TX(0x00) << 8;
        tmp         |= (int32_t)SPI_NO_DELAY_TX(0x00);

        if (tmp & (1 << 23)) { tmp |= 0xFF000000; }  /* sign-extend */
        data[ch] = tmp;
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    delay_cycles(550);
}

/* ---- DMA tabanlı ---- */
void ads1299_read_data_dma(uint32_t *status, int32_t *data)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    spi_tx_buffer[0] = _RDATA;
    for (int i = 1; i < 28; ++i) spi_tx_buffer[i] = 0x00;

    HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer, spi_rx_buffer, 28);
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) { }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);

    *status =  (uint32_t)spi_rx_buffer[1] << 16
             | (uint32_t)spi_rx_buffer[2] << 8
             | (uint32_t)spi_rx_buffer[3];

    for (uint8_t ch = 0; ch < 8; ++ch) {
        uint8_t idx = 4 + 3 * ch;
        int32_t tmp = (int32_t)spi_rx_buffer[idx]     << 16
                    | (int32_t)spi_rx_buffer[idx + 1] << 8
                    | (int32_t)spi_rx_buffer[idx + 2];

        if (tmp & (1 << 23)) { tmp |= 0xFF000000; }
        data[ch] = tmp;
    }

    SCB_CleanDCache_by_Addr((void *)spi_rx_buffer, 32);
}

/* ---- Register R/W ---- */
void ads1299_write_reg(uint8_t addr, uint8_t val)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    SPI_TX(_WREG | addr);
    SPI_TX(0x00);                      /* write 1 byte */
    SPI_TX(val);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    delay_cycles(550);
}

uint8_t ads1299_read_reg(uint8_t addr)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    SPI_TX(_RREG | addr);
    SPI_TX(0x00);                      /* read 1 byte  */
    uint8_t resp = SPI_TX(0x00);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    delay_cycles(550);
    return resp;
}

/* ---- SPI helper ---- */
uint8_t SPI_TX(uint8_t data)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx, 1, HAL_MAX_DELAY);
    delay_cycles(1100);               /* ≈2 µs */
    return rx;
}

uint8_t SPI_NO_DELAY_TX(uint8_t data)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

/* --------------------------------------------------------------------------
 *  3)  SUPPORT ROUTINES
 * ------------------------------------------------------------------------*/
void ads1299_configure_dma(void)
{
    __HAL_SPI_ENABLE_IT(&hspi1, SPI_IT_RXP);   /* H7RS’de RXNE → RXP       */
    UART_Transmit("DMA configured for ADS1299\n");
}

void ads1299_start_continuous_mode(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    SPI_TX(_RDATAC);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    UART_Transmit("ADS1299 RDATAC mode ON\n");
}

void ads1299_configure_channels_from_settings(void)
{
    for (uint8_t ch = 0; ch < 8; ++ch) {
        uint8_t cfg = ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL;
        cfg |= (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(ch))
               ? ADS1299_INPUT_PWR_UP : ADS1299_INPUT_PWR_DOWN;
        ads1299_write_reg(CH1SET + ch, cfg);
    }

    char msg[48];
    snprintf(msg, sizeof(msg), "Channel map set: 0x%02X\n",
             (uint8_t)BIOEXG_SETTINGS);
    UART_Transmit(msg);
}

/* --------------------------------------------------------------------------
 *  4)  SELF-TEST
 * ------------------------------------------------------------------------*/
void ads1299_self_test(void)
{
    UART_Transmit("Running ADS1299 self-test…\n");

    const uint8_t chip_id = ads1299_read_reg(ID);
    char msg[64];
    snprintf(msg, sizeof(msg), "ID read: 0x%02X (expect 0x3E)\n", chip_id);
    UART_Transmit(msg);

    ads1299_write_reg(CONFIG1, 0x55);
    const uint8_t cfg1 = ads1299_read_reg(CONFIG1);
    snprintf(msg, sizeof(msg), "CONFIG1 W/R: 0x%02X (expect 0x55)\n", cfg1);
    UART_Transmit(msg);

    /* restore */
    ads1299_write_reg(CONFIG1, 0x94);

    if (chip_id == 0x3E && cfg1 == 0x55) {
        UART_Transmit("Self-test PASSED\n");
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
    } else {
        UART_Transmit("Self-test FAILED\n");
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
    }
}

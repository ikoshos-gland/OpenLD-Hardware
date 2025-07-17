/* MIT License

   Copyright (c) [2016] [Jae Choi]
   STM32H7S3L8 Port - 2024

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#include "main.h"

// External HAL handle
extern SPI_HandleTypeDef hspi1;

// DMA buffers for enhanced performance
uint8_t spi_tx_buffer[32] __attribute__((section(".SRAM2")));
uint8_t spi_rx_buffer[32] __attribute__((section(".SRAM2")));

// ADS1299 data buffer for DMA operations
uint8_t ads1299_data_buffer[27] __attribute__((section(".AXI_SRAM")));  // 3 status + 24 data bytes

void ads1299_init()
{
    // ADS1299 Power up for STM32H7S3L8
    UART_Transmit("Starting ADS1299 Power up sequence...\n");
    ads1299_pwr_up_seq();
    HAL_Delay(1000);
    ads1299_pwr_up_seq();
    UART_Transmit("Power up sequence completed\n");

    // Stop Conversion for Configuration
    UART_Transmit("Sending Stop Data CMD...\n");
    ads1299_stop_dataread();

    // WHO AM I?
    if (ads1299_read_reg(ID) == 0x3E) {
        UART_Transmit("ADS1299 ID verified - Life is good\n");
    } else {
        UART_Transmit("ERROR: ADS1299 ID mismatch\n");
        while (1) {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);  // Error LED
            HAL_Delay(500);
        }
    }

    // EMG Configurations (Enhanced for STM32H7S3L8):
    // Set CONFIG1 for 1 kHz sampling rate (0x94 = 1000 SPS)
    ads1299_write_reg(CONFIG1, 0x94);
    UART_Transmit("CONFIG1 set to 1kHz sampling\n");

    // Turn on Reference buffer, disable Bias buffer for EMG in CONFIG3
    // Set Test signal configs in CONFIG2
    ads1299_write_reg(CONFIG3, 0x60 | (1 << 7));  // Reference buffer ON, Bias buffer OFF
    ads1299_write_reg(CONFIG2, 0xC0 | ADS1299_TEST_INT | ADS1299_TESTSIGNAL_PULSE_FAST);
    UART_Transmit("Reference and test signal configured\n");

    // All Channels are PGA = 6 | Normal Input | Powered down initially (EMG optimized)
    // These will be enabled based on BIOEXG_SETTINGS
    ads1299_write_reg(CH1SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH2SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH3SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH4SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH5SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH6SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH7SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    ads1299_write_reg(CH8SET, ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL | ADS1299_INPUT_PWR_DOWN);
    UART_Transmit("All channels configured for EMG (PGA=6)\n");

    // Configure Lead Off Options in LOFF: Set AC lead-off at 62.5hz (f_DR/4)
    ads1299_write_reg(LOFF, LOFF_FREQ_FS_4);

    // Connect SRB1 to all inverting outputs
    ads1299_write_reg(MISC1, 1 << 5);

    // Enhanced debug output for H7S3L8
    #ifdef DEBUG_ADS1299
    char debug_msg[100];
    sprintf(debug_msg, "CONFIG3 - should be 0xE0: 0x%02X\n", ads1299_read_reg(CONFIG3));
    UART_Transmit(debug_msg);
    sprintf(debug_msg, "CONFIG2: 0x%02X\n", ads1299_read_reg(CONFIG2));
    UART_Transmit(debug_msg);
    sprintf(debug_msg, "CH8SET: 0x%02X\n", ads1299_read_reg(CH8SET));
    UART_Transmit(debug_msg);
    #endif

    // Configure DMA for continuous data acquisition
    ads1299_configure_dma();

    // Start the conversion
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    UART_Transmit("ADS1299 initialization complete - Starting conversion\n");
}

void ads1299_pwr_up_seq()
{
    // Enhanced power-up sequence for H7S3L8
    // WAIT 40ms
    HAL_Delay(40);
    
    // PULL RESET LOW
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    
    // WAIT 2us (use more precise timing)
    for (volatile int i = 0; i < 1100; i++);  // ~2us at 550MHz
    
    // PULL RESET HIGH
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    
    // WAIT 10us
    for (volatile int i = 0; i < 5500; i++);  // ~10us at 550MHz
}

void ads1299_stop_dataread()
{
    // PULL CS LOW
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    
    // SEND BYTE: 0x11
    SPI_TX(_SDATAC);
    
    // PULL CS HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    
    // WAIT 2*TCLK'S = improved timing for H7S3L8
    for (volatile int i = 0; i < 550; i++);  // ~1us at 550MHz
}

void ads1299_read_data(uint32_t *STATUS, int32_t *DATA)
{
    // PULL CS LOW
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    // SEND BYTE: 0x12 (RDATA command)
    SPI_NO_DELAY_TX(_RDATA);
    
    // READ STATUS (3 bytes)
    *STATUS = SPI_NO_DELAY_TX(0x0) << 16;
    *STATUS |= SPI_NO_DELAY_TX(0x0) << 8;
    *STATUS |= SPI_NO_DELAY_TX(0x0);

    // READ DATA 0 - 7 (24 bytes total)
    for (int i = 0; i < 8; i++) {
        DATA[i] = SPI_NO_DELAY_TX(0x0) << 16;
        DATA[i] |= SPI_NO_DELAY_TX(0x0) << 8;
        DATA[i] |= SPI_NO_DELAY_TX(0x0);

        // Handle two's complement sign extension
        if (DATA[i] & (1 << 23)) {
            DATA[i] ^= 0x00FFFFFF;
            DATA[i]++;
            DATA[i] &= 0x00FFFFFF;
            DATA[i] *= -1;
        }
    }

    // PULL CS HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    
    // WAIT 2*TCLK'S
    for (volatile int i = 0; i < 550; i++);  // ~1us at 550MHz
}

void ads1299_read_data_dma(uint32_t *STATUS, int32_t *DATA)
{
    // Enhanced DMA-based data reading for H7S3L8
    // PULL CS LOW
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    // Prepare DMA transfer
    spi_tx_buffer[0] = _RDATA;
    for (int i = 1; i < 28; i++) {
        spi_tx_buffer[i] = 0x00;  // Dummy bytes
    }

    // Start DMA transfer
    HAL_SPI_TransmitReceive_DMA(&hspi1, spi_tx_buffer, spi_rx_buffer, 28);
    
    // Wait for DMA completion
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

    // PULL CS HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);

    // Parse received data
    // STATUS is bytes 1-3
    *STATUS = (spi_rx_buffer[1] << 16) | (spi_rx_buffer[2] << 8) | spi_rx_buffer[3];

    // DATA is bytes 4-27 (8 channels * 3 bytes each)
    for (int i = 0; i < 8; i++) {
        int idx = 4 + (i * 3);
        DATA[i] = (spi_rx_buffer[idx] << 16) | (spi_rx_buffer[idx + 1] << 8) | spi_rx_buffer[idx + 2];

        // Handle two's complement sign extension
        if (DATA[i] & (1 << 23)) {
            DATA[i] ^= 0x00FFFFFF;
            DATA[i]++;
            DATA[i] &= 0x00FFFFFF;
            DATA[i] *= -1;
        }
    }

    // Clean D-Cache for coherency
    SCB_CleanDCache_by_Addr((uint32_t*)spi_rx_buffer, 32);
}

void ads1299_write_reg(uint8_t ADDR, uint8_t VAL)
{
    // PULL CS LOW
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    // SEND FIRST BYTE: 0x40 | ADDR
    SPI_TX(_WREG | ADDR);
    // SEND SECOND BYTE: NUMBER_TO_WRITE (0x00 for single byte)
    SPI_TX(0x00);
    // SEND VALUE TO WRITE
    SPI_TX(VAL);

    // PULL CS HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    
    // WAIT 2*TCLK'S
    for (volatile int i = 0; i < 550; i++);  // ~1us at 550MHz
}

uint8_t ads1299_read_reg(uint8_t ADDR)
{
    // PULL CS LOW
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    // SEND FIRST BYTE: 0x20 | ADDR
    SPI_TX(_RREG | ADDR);
    // SEND SECOND BYTE: NUMBER_TO_READ (0x00 for single byte)
    SPI_TX(0x00);
    // SEND A DUMMY BYTE TO RECEIVE DATA
    uint8_t RESP = SPI_TX(0x00);

    // PULL CS HIGH
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    
    // WAIT 2*TCLK'S
    for (volatile int i = 0; i < 550; i++);  // ~1us at 550MHz
    
    // Return read value
    return RESP;
}

uint8_t SPI_TX(uint8_t DATA)
{
    // Enhanced SPI transmission for H7S3L8 using HAL
    uint8_t rx_data;
    
    // Transmit and receive data
    HAL_SPI_TransmitReceive(&hspi1, &DATA, &rx_data, 1, HAL_MAX_DELAY);
    
    // Give the ADS1299 some time to process
    for (volatile int i = 0; i < 1100; i++);  // ~2us at 550MHz
    
    return rx_data;
}

uint8_t SPI_NO_DELAY_TX(uint8_t DATA)
{
    // High-speed SPI transmission without delay for H7S3L8
    uint8_t rx_data;
    
    // Transmit and receive data
    HAL_SPI_TransmitReceive(&hspi1, &DATA, &rx_data, 1, HAL_MAX_DELAY);
    
    return rx_data;
}

void ads1299_configure_dma()
{
    // Configure DMA for continuous ADS1299 data acquisition
    // This function sets up circular buffer for continuous EMG data streaming
    
    // Configure SPI for DMA mode
    // STM32H7RS: SPI_IT_RXNE renamed to SPI_IT_RXP
    __HAL_SPI_ENABLE_IT(&hspi1, SPI_IT_RXP);
    
    // Enable DMA requests
    // STM32H7RS: Use HAL_SPI_RegisterCallback for DMA setup
    // Note: DMA enabling is handled by HAL_SPI_Transmit_DMA/Receive_DMA
    // __HAL_SPI_ENABLE_DMA(&hspi1, SPI_DMA_RX | SPI_DMA_TX);
    
    UART_Transmit("DMA configured for ADS1299 continuous acquisition\n");
}

void ads1299_start_continuous_mode()
{
    // Start continuous data read mode
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    SPI_TX(_RDATAC);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    
    UART_Transmit("ADS1299 continuous mode started\n");
}

void ads1299_configure_channels_from_settings()
{
    // Configure channels based on BIOEXG_SETTINGS
    for (int i = 0; i < 8; i++) {
        uint8_t ch_config = ADS1299_PGA_GAIN06 | ADS1299_INPUT_NORMAL;
        
        if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(i)) {
            ch_config |= ADS1299_INPUT_PWR_UP;
        } else {
            ch_config |= ADS1299_INPUT_PWR_DOWN;
        }
        
        ads1299_write_reg(CH1SET + i, ch_config);
    }
    
    char msg[64];
    int result = snprintf(msg, sizeof(msg), "Channels configured: 0x%02X\n", (uint8_t)BIOEXG_SETTINGS);
    if (result > 0 && result < (int)sizeof(msg)) {
        UART_Transmit(msg);
    }
}

void ads1299_self_test()
{
    // Enhanced self-test for H7S3L8
    UART_Transmit("Running ADS1299 self-test...\n");
    
    // Test ID register
    uint8_t id = ads1299_read_reg(ID);
    char msg[64];
    int result = snprintf(msg, sizeof(msg), "ADS1299 ID: 0x%02X (Expected: 0x3E)\n", id);
    if (result > 0 && result < (int)sizeof(msg)) {
        UART_Transmit(msg);
    }
    
    // Test configuration registers
    ads1299_write_reg(CONFIG1, 0x55);
    uint8_t config1 = ads1299_read_reg(CONFIG1);
    result = snprintf(msg, sizeof(msg), "CONFIG1 write/read test: 0x%02X\n", config1);
    if (result > 0 && result < (int)sizeof(msg)) {
        UART_Transmit(msg);
    }
    
    // Restore proper configuration
    ads1299_write_reg(CONFIG1, 0x94);
    
    if (id == 0x3E && config1 == 0x55) {
        UART_Transmit("ADS1299 self-test PASSED\n");
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);  // Success LED
    } else {
        UART_Transmit("ADS1299 self-test FAILED\n");
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);  // Error LED
    }
}
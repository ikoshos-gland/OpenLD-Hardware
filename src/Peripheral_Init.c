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

// HAL handles for STM32H7S3L8
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_uart1_rx;
DMA_HandleTypeDef hdma_uart1_tx;

void init_SystemClock(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    // Configure the main internal regulator output voltage (STM32H7RS)
    // Supply configuration update
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    
    // Configure voltage scaling to Scale 1 (1.25V core voltage)
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }
    
    // Wait for voltage scaling to be ready
    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    // Configure LSE Drive Capability
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    // Initialize the RCC Oscillators
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    
    // STM32H7RS PLL1 configuration for 600MHz SYSCLK
    RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL1.PLLM = 5;       // 25MHz/5 = 5MHz (VCO input)
    RCC_OscInitStruct.PLL1.PLLN = 120;     // 5MHz * 120 = 600MHz (VCO output)
    RCC_OscInitStruct.PLL1.PLLP = 1;       // 600MHz/1 = 600MHz (SYSCLK)
    RCC_OscInitStruct.PLL1.PLLQ = 2;       // 600MHz/2 = 300MHz
    RCC_OscInitStruct.PLL1.PLLR = 2;       // 600MHz/2 = 300MHz
    RCC_OscInitStruct.PLL1.PLLFractional = 0;  // No fractional part
    
    // Disable PLL2 and PLL3 (not used)
    RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Note: STM32H7RS doesn't require overdrive mode for 600MHz operation
    // The voltage scaling configuration above is sufficient

    // Initialize the CPU, AHB and APB bus clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;    // 600MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;      // 600MHz/2 = 300MHz
    // STM32H7RS uses simplified APB structure
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;     // 300MHz/2 = 150MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;     // 300MHz/2 = 150MHz
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
        Error_Handler();
    }

    // Configure peripheral clocks
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1 | RCC_PERIPHCLK_USART1;
    PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_PLL1Q;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

void init_MPU(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    // Disable the MPU
    HAL_MPU_Disable();

    // Configure the MPU attributes for AXI SRAM region
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x24000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    // Enable the MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void init_Cache(void)
{
    // Enable I-Cache
    SCB_EnableICache();

    // Enable D-Cache
    SCB_EnableDCache();
}

void init_GPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO Clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // Configure SPI1 pins (PA5-SCK, PA6-MISO, PA7-MOSI)
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure SPI1 CS pin (PB6 - Manual control)
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);  // CS high (inactive)

    // Configure additional control pins
    GPIO_InitStruct.Pin = GPIO_PIN_7;  // Additional control pin
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure UART1 pins (PA9-TX, PA10-RX)
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure control pins on GPIOD
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_14 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

    // Configure button input (PD9) with pull-up
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Configure LED pins (PE7, PE8, PE10)
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void init_DMA(void)
{
    // Enable DMA clocks
    // STM32H7RS uses GPDMA1 and HPDMA1 instead of DMA1/DMA2
    __HAL_RCC_GPDMA1_CLK_ENABLE();
    __HAL_RCC_HPDMA1_CLK_ENABLE();

    // Configure DMA for SPI1 TX
    // STM32H7RS: Use GPDMA1 channel for SPI1 TX
    hdma_spi1_tx.Instance = GPDMA1_Channel0;
    hdma_spi1_tx.Init.Request = GPDMA1_REQUEST_SPI1_TX;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    hdma_spi1_tx.Init.SrcInc = DMA_SINC_INCREMENTED;
    hdma_spi1_tx.Init.DestInc = DMA_DINC_FIXED;
    hdma_spi1_tx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_spi1_tx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_spi1_tx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_spi1_tx.Init.SrcBurstLength = 1;
    hdma_spi1_tx.Init.DestBurstLength = 1;
    hdma_spi1_tx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    hdma_spi1_tx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    // STM32H7RS: FIFO mode not used in GPDMA
    // hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi1_tx);

    // Configure DMA for SPI1 RX
    // STM32H7RS: Use GPDMA1 channel for SPI1 RX
    hdma_spi1_rx.Instance = GPDMA1_Channel1;
    hdma_spi1_rx.Init.Request = GPDMA1_REQUEST_SPI1_RX;
    hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi1_rx.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    hdma_spi1_rx.Init.SrcInc = DMA_SINC_FIXED;
    hdma_spi1_rx.Init.DestInc = DMA_DINC_INCREMENTED;
    hdma_spi1_rx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_spi1_rx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_spi1_rx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_spi1_rx.Init.SrcBurstLength = 1;
    hdma_spi1_rx.Init.DestBurstLength = 1;
    hdma_spi1_rx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    hdma_spi1_rx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_spi1_rx.Init.Mode = DMA_NORMAL;
    // STM32H7RS: FIFO mode not used in GPDMA
    // hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi1_rx);

    // Link DMA handles to SPI handle
    __HAL_LINKDMA(&hspi1, hdmatx, hdma_spi1_tx);
    __HAL_LINKDMA(&hspi1, hdmarx, hdma_spi1_rx);

    // Configure DMA for USART1 TX (STM32H7RS)
    hdma_uart1_tx.Instance = GPDMA1_Channel2;
    hdma_uart1_tx.Init.Request = GPDMA1_REQUEST_USART1_TX;
    hdma_uart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart1_tx.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    hdma_uart1_tx.Init.SrcInc = DMA_SINC_INCREMENTED;
    hdma_uart1_tx.Init.DestInc = DMA_DINC_FIXED;
    hdma_uart1_tx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_uart1_tx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_uart1_tx.Init.Priority = DMA_LOW_PRIORITY_MID_WEIGHT;
    hdma_uart1_tx.Init.SrcBurstLength = 1;
    hdma_uart1_tx.Init.DestBurstLength = 1;
    hdma_uart1_tx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    hdma_uart1_tx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_uart1_tx.Init.Mode = DMA_NORMAL;
    // STM32H7RS: FIFO mode not used in GPDMA
    // hdma_uart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_uart1_tx);

    // Configure DMA for USART1 RX (STM32H7RS)
    hdma_uart1_rx.Instance = GPDMA1_Channel3;
    hdma_uart1_rx.Init.Request = GPDMA1_REQUEST_USART1_RX;
    hdma_uart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart1_rx.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    hdma_uart1_rx.Init.SrcInc = DMA_SINC_FIXED;
    hdma_uart1_rx.Init.DestInc = DMA_DINC_INCREMENTED;
    hdma_uart1_rx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_uart1_rx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_uart1_rx.Init.Priority = DMA_LOW_PRIORITY_MID_WEIGHT;
    hdma_uart1_rx.Init.SrcBurstLength = 1;
    hdma_uart1_rx.Init.DestBurstLength = 1;
    hdma_uart1_rx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    hdma_uart1_rx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    hdma_uart1_rx.Init.Mode = DMA_NORMAL;
    // STM32H7RS: FIFO mode not used in GPDMA
    // hdma_uart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_uart1_rx);

    // Link DMA handles to UART handle
    __HAL_LINKDMA(&huart1, hdmatx, hdma_uart1_tx);
    __HAL_LINKDMA(&huart1, hdmarx, hdma_uart1_rx);

    // Configure DMA interrupts (STM32H7RS GPDMA1)
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);
}

void init_SPI(void)
{
    // Enable SPI1 clock
    __HAL_RCC_SPI1_CLK_ENABLE();

    // Configure SPI1 for ADS1299
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;    // CPOL = 0
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;        // CPHA = 1
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  // 150MHz/8 = 18.75MHz
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    
    HAL_SPI_Init(&hspi1);
}

void init_UART(void)
{
    // Enable UART1 clock
    __HAL_RCC_USART1_CLK_ENABLE();

    // Configure UART1
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 230400;  // Higher baud rate for H7
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    HAL_UART_Init(&huart1);

    // Configure UART interrupts
    HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void init_EXTI(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure EXTI for data ready (PD6)
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Configure EXTI for processing trigger (PE0)
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // Configure EXTI interrupts (STM32H7RS uses individual EXTI interrupts)
    // Note: GPIO_PIN_0 corresponds to EXTI0_IRQn in STM32H7RS
    
    HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);        // High priority for processing
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void check_UART_Status(void)
{
    // Check UART status and handle any errors
    if (huart1.ErrorCode != HAL_UART_ERROR_NONE) {
        // Handle UART errors
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);  // Error LED
        HAL_UART_Init(&huart1);  // Reinitialize UART
    } else {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);  // Clear error LED
    }
}

// DMA Interrupt Handlers
void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

void DMA1_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_uart1_tx);
}

void DMA1_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_uart1_rx);
}

// SPI1 Interrupt Handler
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

// UART1 Interrupt Handler
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

// System initialization function
void SystemInit_H7S3L8(void)
{
    // Configure system clock
    init_SystemClock();
    
    // Configure MPU
    init_MPU();
    
    // Enable caches
    init_Cache();
    
    // Initialize HAL
    HAL_Init();
    
    // Initialize peripherals
    init_GPIO();
    init_DMA();
    init_SPI();
    init_UART();
    init_EXTI();
    
    // System is ready
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);  // Ready LED
}

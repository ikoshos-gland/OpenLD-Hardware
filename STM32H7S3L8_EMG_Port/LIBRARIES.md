# STM32H7S3L8 EMG Port - Library Dependencies

## Overview
This document provides a comprehensive inventory of all libraries, frameworks, and dependencies used in the STM32H7S3L8_EMG_Port project. This EMG (Electromyography) signal acquisition system leverages the STM32H7S3L8's 600MHz Cortex-M7 architecture with advanced signal processing capabilities.

## Table of Contents
- [STM32H7RS HAL Libraries](#stm32h7rs-hal-libraries)
- [CMSIS-DSP Library](#cmsis-dsp-library)
- [CMSIS Core Libraries](#cmsis-core-libraries)
- [Standard C Libraries](#standard-c-libraries)
- [STM32H7RS-Specific Features](#stm32h7rs-specific-features)
- [Application-Specific Libraries](#application-specific-libraries)
- [Memory and Performance Optimizations](#memory-and-performance-optimizations)
- [System Configuration Dependencies](#system-configuration-dependencies)

---

## STM32H7RS HAL Libraries

### Core HAL Headers
```c
#include "stm32h7rsxx_hal.h"          // Main HAL header
#include "stm32h7rsxx_hal_gpio.h"     // GPIO control
#include "stm32h7rsxx_hal_spi.h"      // SPI communication
#include "stm32h7rsxx_hal_uart.h"     // UART communication
#include "stm32h7rsxx_hal_dma.h"      // DMA management
#include "stm32h7rsxx_hal_rcc.h"      // Clock control
#include "stm32h7rsxx_hal_pwr.h"      // Power management
#include "stm32h7rsxx_hal_pwr_ex.h"   // Extended power management
```

### Key HAL Functions Used

#### GPIO Functions
- `HAL_GPIO_Init()` - Initialize GPIO pin configuration
- `HAL_GPIO_WritePin()` - Write to GPIO pin
- `HAL_GPIO_TogglePin()` - Toggle GPIO pin state

#### SPI Functions
- `HAL_SPI_Init()` - Initialize SPI peripheral
- `HAL_SPI_TransmitReceive()` - Synchronous SPI transmit/receive
- `HAL_SPI_TransmitReceive_DMA()` - DMA-based SPI transmit/receive
- `HAL_SPI_GetState()` - Get SPI peripheral state
- `HAL_SPI_IRQHandler()` - SPI interrupt handler
- `HAL_SPI_TxRxCpltCallback()` - SPI transfer complete callback

#### UART Functions
- `HAL_UART_Init()` - Initialize UART peripheral
- `HAL_UART_Transmit()` - Transmit data via UART
- `HAL_UART_Receive()` - Receive data via UART
- `HAL_UART_IRQHandler()` - UART interrupt handler
- `HAL_UART_RxCpltCallback()` - UART receive complete callback

#### DMA Functions
- `HAL_DMA_Init()` - Initialize DMA channel
- `HAL_DMA_IRQHandler()` - DMA interrupt handler

#### System Functions
- `HAL_Init()` - Initialize HAL library
- `HAL_Delay()` - Blocking delay function
- `HAL_GPIO_EXTI_Callback()` - GPIO external interrupt callback

#### Power Management
- `HAL_PWREx_ConfigSupply()` - Configure power supply
- `HAL_PWR_EnableBkUpAccess()` - Enable backup domain access
- `HAL_PWREx_EnableOverDrive()` - Enable overdrive mode

#### Clock Configuration
- `HAL_RCC_OscConfig()` - Configure system oscillators
- `HAL_RCC_ClockConfig()` - Configure system clocks
- `HAL_RCCEx_PeriphCLKConfig()` - Configure peripheral clocks

#### Memory Protection
- `HAL_MPU_Disable()` - Disable Memory Protection Unit
- `HAL_MPU_ConfigRegion()` - Configure MPU region
- `HAL_MPU_Enable()` - Enable Memory Protection Unit

#### Interrupt Management
- `HAL_NVIC_SetPriority()` - Set interrupt priority
- `HAL_NVIC_EnableIRQ()` - Enable interrupt request

### HAL Macros Used

#### Power Management
- `__HAL_PWR_VOLTAGESCALING_CONFIG()` - Configure voltage scaling
- `__HAL_PWR_GET_FLAG()` - Get power flag status

#### Clock Control
- `__HAL_RCC_LSEDRIVE_CONFIG()` - Configure LSE drive capability
- `__HAL_RCC_GPIOA_CLK_ENABLE()` - Enable GPIOA clock
- `__HAL_RCC_GPIOB_CLK_ENABLE()` - Enable GPIOB clock
- `__HAL_RCC_GPIOC_CLK_ENABLE()` - Enable GPIOC clock
- `__HAL_RCC_GPIOD_CLK_ENABLE()` - Enable GPIOD clock
- `__HAL_RCC_GPIOE_CLK_ENABLE()` - Enable GPIOE clock
- `__HAL_RCC_GPDMA1_CLK_ENABLE()` - Enable GPDMA1 clock
- `__HAL_RCC_HPDMA1_CLK_ENABLE()` - Enable HPDMA1 clock
- `__HAL_RCC_SPI1_CLK_ENABLE()` - Enable SPI1 clock
- `__HAL_RCC_USART1_CLK_ENABLE()` - Enable USART1 clock

#### DMA Configuration
- `__HAL_LINKDMA()` - Link DMA handle to peripheral

---

## CMSIS-DSP Library

### Headers
```c
#include "arm_math.h"                 // Main CMSIS-DSP header
#include "arm_const_structs.h"        // Predefined structures
```

### Key CMSIS-DSP Functions Used

#### IIR Filtering
- `arm_biquad_cascade_df2T_init_f32()` - Initialize biquad cascade IIR filter
- `arm_biquad_cascade_df2T_f32()` - Process data through biquad cascade IIR filter

#### Mathematical Functions
- `arm_sqrt_f32()` - Square root calculation
- `arm_rms_f32()` - Root mean square calculation
- `arm_max_f32()` - Maximum value calculation
- `arm_mean_f32()` - Mean value calculation
- `arm_copy_f32()` - Copy array data

#### FFT Functions
- `arm_rfft_fast_init_f32()` - Initialize real FFT
- `arm_rfft_fast_f32()` - Perform real FFT
- `arm_cmplx_mag_f32()` - Calculate complex magnitude

### Data Structures
- `arm_biquad_cascade_df2T_instance_f32` - Biquad cascade filter instance
- `arm_rfft_fast_instance_f32` - Real FFT instance

### Configuration
- **Filter Stages**: 4 HP + 4 BP biquad sections
- **FFT Size**: 256 points
- **Data Type**: 32-bit floating point (`float32_t`)

---

## CMSIS Core Libraries

### Headers
```c
#include "core_cm7.h"                 // Cortex-M7 core definitions
#include "stm32h7rsxx.h"              // STM32H7RS device header
```

### Key Core Functions Used

#### Cache Management
- `SCB_EnableICache()` - Enable instruction cache
- `SCB_EnableDCache()` - Enable data cache
- `SCB_CleanInvalidateDCache_by_Addr()` - Clean and invalidate data cache by address
- `SCB_CleanDCache_by_Addr()` - Clean data cache by address

#### System Control
- `__disable_irq()` - Disable global interrupts
- `__WFI()` - Wait for interrupt
- `__DSB()` - Data synchronization barrier
- `__ISB()` - Instruction synchronization barrier

#### Debug/Trace
- `DWT->CYCCNT` - Data Watchpoint and Trace cycle counter

---

## Standard C Libraries

### Headers
```c
#include <stdint.h>                   // Integer types
#include <stdio.h>                    // Standard I/O
#include <string.h>                   // String functions  
#include <math.h>                     // Mathematical functions
#include <inttypes.h>                 // Integer format macros
```

### Functions Used
- **String Functions**: `strlen()`, `sprintf()`, `snprintf()`
- **Math Functions**: `fabsf()` (absolute value)

---

## STM32H7RS-Specific Features

### Enhanced Architecture Features
- **System Clock**: 600MHz Cortex-M7 operation
- **Memory Architecture**: Distributed SRAM (AXI, SRAM1, SRAM2, SRAM3, SRAM4)
- **Cache System**: 32KB I-Cache + 32KB D-Cache
- **DMA Controllers**: GPDMA1 and HPDMA1
- **Power Management**: Voltage scaling and overdrive mode

### Clock Configuration
```c
// PLL1 Configuration for 600MHz SYSCLK
RCC_OscInitStruct.PLL1.PLLM = 5;       // 25MHz/5 = 5MHz (VCO input)
RCC_OscInitStruct.PLL1.PLLN = 120;     // 5MHz * 120 = 600MHz (VCO output)
RCC_OscInitStruct.PLL1.PLLP = 1;       // 600MHz/1 = 600MHz (SYSCLK)
```

### Bus Clock Frequencies
- **SYSCLK**: 600MHz
- **HCLK**: 300MHz (600MHz/2)
- **APB1**: 150MHz (300MHz/2)
- **APB2**: 150MHz (300MHz/2)
- **SPI1**: 18.75MHz (150MHz/8)

---

## Application-Specific Libraries

### ADS1299 Driver Functions
```c
// Core ADS1299 Functions
void ads1299_init();
void ads1299_pwr_up_seq();
void ads1299_read_data(uint32_t *STATUS, int32_t *DATA);
void ads1299_read_data_dma(uint32_t *STATUS, int32_t *DATA);
void ads1299_stop_dataread();
uint8_t ads1299_read_reg(uint8_t ADDR);
void ads1299_write_reg(uint8_t ADDR, uint8_t VAL);
void ads1299_configure_dma();
void ads1299_start_continuous_mode();
void ads1299_configure_channels_from_settings();
```

### UART Communication Functions
```c
// UART Helper Functions
void UART_Transmit(const char *data);
void CTX(char data);              // Transmit character
char CRX(void);                   // Receive character
void STX(const char *data);       // Transmit string
void SRX(char *data);             // Receive string
void PREG(uint32_t value);        // Print register value in hex
void PDEC(uint32_t value);        // Print decimal value
```

### EMG Processing Functions
```c
// EMG Signal Processing
void emg_process_realtime();
void emg_calculate_features();
void emg_system_init();
```

### System Functions
```c
// System Initialization
void SystemInit_H7S3L8();
void setting_mode();
void check_RN42_RTS();           // Bluetooth RTS check (stub)
```

---

## Memory and Performance Optimizations

### Memory Section Allocation
```c
// Memory section attributes for optimal performance
#define AXI_SRAM_SECTION    __attribute__((section(".AXI_SRAM")))
#define SRAM1_SECTION       __attribute__((section(".SRAM1")))
#define SRAM2_SECTION       __attribute__((section(".SRAM2")))
#define SRAM3_SECTION       __attribute__((section(".SRAM3")))
#define SRAM4_SECTION       __attribute__((section(".SRAM4")))
#define FLASH_SECTION       __attribute__((section(".FLASH")))
```

### Memory Usage Strategy
- **AXI SRAM**: High-speed data buffers (`emg_raw_buffer`, `emg_filtered_buffer`)
- **SRAM1**: Filter states and coefficients
- **SRAM2**: Processed data and system variables
- **FLASH**: Filter coefficients (read-only)

### Cache Control Macros
```c
#define CACHE_CLEAN_INVALIDATE_DCACHE()     SCB_CleanInvalidateDCache()
#define CACHE_CLEAN_DCACHE()                SCB_CleanDCache()
#define CACHE_INVALIDATE_DCACHE()           SCB_InvalidateDCache()
#define CACHE_INVALIDATE_ICACHE()           SCB_InvalidateICache()
```

### Performance Optimizations
- **DMA-based data acquisition** for reduced CPU load
- **Circular buffers** for continuous data processing
- **Cache-optimized memory layout** for maximum throughput
- **Interrupt-driven processing** for real-time performance

---

## System Configuration Dependencies

### Configuration Files
- `stm32h7rsxx_hal_conf.h` - HAL configuration
- `system_stm32h7rsxx.c` - System initialization
- **Linker scripts** for memory section definitions

### Build Dependencies
- **GNU ARM Embedded Toolchain** (`arm-none-eabi-gcc`)
- **STM32CubeH7RS** HAL/LL drivers
- **CMSIS-DSP** library (optimized for Cortex-M7)
- **ST-Link utilities** for programming and debugging

### Hardware Dependencies
- **STM32H7S3L8** microcontroller (600MHz Cortex-M7)
- **ADS1299** 8-channel 24-bit ADC
- **25MHz HSE** crystal oscillator
- **SPI1** interface for ADS1299 communication
- **UART1** interface for data transmission

---

## Performance Specifications

### System Performance
- **CPU Frequency**: 600MHz (3.6x faster than STM32F407VG)
- **Signal Processing**: Real-time 8-channel EMG @ 1kHz sampling
- **Buffer Size**: 800 samples per channel (4x larger than original)
- **Filter Stages**: 4 HP + 4 BP biquad sections
- **FFT Size**: 256 points for spectral analysis

### Memory Utilization
- **Total RAM**: 1MB distributed across multiple SRAM regions
- **Flash**: 2MB for code and coefficients
- **Cache**: 32KB I-Cache + 32KB D-Cache enabled
- **DMA**: 4 channels for SPI and UART operations

---

## Conclusion

The STM32H7S3L8_EMG_Port project demonstrates a sophisticated integration of STM32H7RS HAL drivers, CMSIS-DSP signal processing, and custom EMG processing algorithms. The system is optimized for high-performance real-time signal acquisition and processing, leveraging the advanced features of the STM32H7S3L8's 600MHz Cortex-M7 architecture with distributed memory and cache management.

This library inventory serves as a reference for understanding the project's dependencies and can be used for:
- **System maintenance** and updates
- **Performance optimization** and profiling
- **Porting** to other STM32H7 variants
- **Documentation** and code reviews
- **Dependency management** for build systems

---

*Generated for STM32H7S3L8_EMG_Port - EMG Signal Acquisition System*  
*STM32H7S3L8 @ 600MHz | 8-Channel EMG | Real-time Processing*
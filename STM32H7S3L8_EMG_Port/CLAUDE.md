# STM32H7S3L8 EMG Port - CLAUDE.md

This file provides guidance to Claude Code when working with the STM32H7S3L8 EMG port.

## Build System

This is an STM32H7S3L8 bare-metal embedded system using GNU ARM Embedded Toolchain, ported from the original STM32F407VG EMG system.

### Essential Commands

```bash
# Build release version (optimized)
make release

# Build debug version with symbols
make debug

# Clean build artifacts
make clean

# Flash firmware to MCU
make install

# Build and start debug session (requires st-util)
make debug
```

### Build Requirements

- GNU ARM Embedded Toolchain (`arm-none-eabi-gcc`)
- STM32H7xx HAL Library
- ST-Link utilities for flashing (`st-flash`, `st-util`)

## Architecture Overview

This is the **STM32H7S3L8 port** of the EMG (Electromyography) signal acquisition system. The system captures 8-channel muscle activity data at 1kHz sampling rate using the more powerful STM32H7S3L8 microcontroller.

### Key STM32H7S3L8 Advantages

**Performance Improvements:**
- **ARM Cortex-M7 @ 550MHz** (vs Cortex-M4 @ 168MHz)
- **2MB Flash, 1MB RAM** (vs 1MB Flash, 192KB RAM)
- **Dual-core architecture** (M7 + M4 cores available)
- **Advanced DMA with DMAMUX**
- **Hardware accelerated DSP operations**

**Enhanced Peripherals:**
- **High-speed SPI** with up to 137.5MHz operation
- **Advanced UART/USART** with FIFO and DMA
- **Multi-AHB bus matrix** for reduced contention
- **Cache system** (32KB I-Cache, 32KB D-Cache)

### Core Components (Ported)

**Hardware Layer:**
- `src/cmd_ADS1299.c` - ADS1299 24-bit ADC configuration (enhanced for H7)
- `src/Peripheral_Init.c` - STM32H7S3L8 peripheral initialization
- `src/cmd_UART.c` - UART communication (updated from USART)

**Signal Processing Pipeline:**
- `src/coeffs_IIR.c` - IIR filter coefficients (10-450Hz EMG optimized)
- `src/IRQ.c` - Real-time signal processing with H7 optimizations
- `inc/main.h` - System configuration for H7 architecture

**System Control:**
- `src/Settings_UART.c` - Terminal interface for runtime configuration
- `src/main.c` - System initialization and main loop

### STM32H7S3L8 Specific Features

**Memory Organization:**
- **AXI SRAM** (512KB) - Main data buffers
- **SRAM1** (128KB) - Filter coefficients and states
- **SRAM2** (128KB) - DMA descriptors and system variables
- **SRAM3** (32KB) - Boot and system critical data
- **SRAM4** (64KB) - Available for expansion

**Clock Configuration:**
- **System Clock**: 550MHz (from PLL)
- **AHB Clock**: 275MHz
- **APB1 Clock**: 137.5MHz  
- **APB2 Clock**: 137.5MHz
- **SPI Clock**: Up to 137.5MHz

**DMA Configuration:**
- **DMAMUX** for flexible request routing
- **DMA1/DMA2** with enhanced features
- **BDMA** for low-power peripheral access

### Signal Processing Enhancements

**Performance Improvements:**
- **2.5x faster DSP operations** due to M7 core
- **Cache optimization** for filter coefficients
- **Enhanced ARM CMSIS-DSP** library utilization
- **Parallel processing** capabilities

**Memory Optimizations:**
- **Large signal buffers** (4x larger windows possible)
- **Multiple filter banks** for advanced EMG analysis
- **Circular buffer management** with DMA

### Key Differences from STM32F407VG

**Register Changes:**
- **RCC** structure completely redesigned
- **GPIO** alternate function mapping updated
- **SPI** peripheral enhanced with FIFO
- **UART** peripheral with advanced features

**Interrupt System:**
- **NVIC** priority grouping updated
- **EXTI** line mapping changed
- **DMA** interrupt handling enhanced

**Memory Mapping:**
- **Flash** at 0x08000000 (same)
- **SRAM** distributed across multiple regions
- **Peripheral** addresses updated for H7

## Development Notes

**Optimization Strategies:**
- Use **AXI SRAM** for high-speed data processing
- Leverage **cache system** for frequently accessed data
- Implement **circular buffers** with DMA for continuous acquisition
- Utilize **dual-core** capabilities for parallel processing

**Power Management:**
- **Dynamic voltage scaling** available
- **Multiple sleep modes** for power optimization
- **Peripheral clock gating** for unused blocks

**Debug Features:**
- **ETM trace** capability for advanced debugging
- **Hardware breakpoints** enhanced
- **Real-time expression evaluation**

## Build Configuration

**Compiler Flags:**
- `-mcpu=cortex-m7` (updated from cortex-m4)
- `-mfpu=fpv5-d16` (updated from fpv4-sp-d16)
- `-mfloat-abi=hard`
- `-mthumb`

**Linker Configuration:**
- **Memory regions** updated for H7S3L8
- **Stack size** increased for larger processing
- **Heap size** optimized for dynamic allocation

## Testing and Validation

**Enhanced Test Capabilities:**
- **Higher sampling rates** possible (up to 4kHz)
- **More complex filtering** without performance penalty
- **Real-time spectral analysis** capabilities
- **Advanced EMG feature extraction**

The STM32H7S3L8 port provides significant performance improvements while maintaining full compatibility with the original EMG acquisition functionality.
# STM32H7S3L8 EMG Port - Library Setup Guide

## Required Libraries Setup

This guide explains how to set up the necessary libraries for the STM32H7S3L8 EMG project.

### 1. STM32H7RS HAL Driver

**Repository:** https://github.com/STMicroelectronics/stm32h7rsxx-hal-driver/tree/9e83b95ae0f70faa067eddce2da617d180937f9b

**Setup:**
```bash
cd /path/to/your/project
git clone --recursive https://github.com/STMicroelectronics/stm32h7rsxx-hal-driver.git
cd stm32h7rsxx-hal-driver
git checkout 9e83b95ae0f70faa067eddce2da617d180937f9b
git submodule update --init --recursive
```

**Directory Structure:**
```
stm32h7rsxx-hal-driver/
├── Inc/                    # HAL header files
├── Src/                    # HAL source files
├── README.md
└── Release_Notes.html
```

### 2. STM32CubeH7RS CMSIS

**Repository:** https://github.com/STMicroelectronics/STM32CubeH7RS/tree/main/Drivers/CMSIS

**Setup:**
```bash
cd /path/to/your/project
git clone --recursive https://github.com/STMicroelectronics/STM32CubeH7RS.git
# Note: Full repository is cloned due to submodule dependencies
# You can remove unnecessary directories after setup if disk space is a concern
```

**Directory Structure:**
```
CMSIS/
├── Core/
│   └── Include/            # Core Cortex-M headers
├── Device/
│   └── ST/
│       └── STM32H7RSxx/
│           └── Include/    # Device-specific headers
├── Include/                # General CMSIS headers
├── DSP/                    # CMSIS-DSP (if included)
└── Documentation/
```

### 3. ARM CMSIS-DSP Library

**Repository:** https://github.com/ARM-software/CMSIS-DSP

**Setup:**
```bash
cd /path/to/your/project
git clone --recursive https://github.com/ARM-software/CMSIS-DSP.git
```

**Directory Structure:**
```
CMSIS-DSP/
├── Include/                # Public DSP headers
├── PrivateInclude/         # Internal build headers
├── Source/                 # DSP source code
│   ├── BasicMathFunctions/
│   ├── FastMathFunctions/
│   ├── FilteringFunctions/
│   ├── TransformFunctions/
│   ├── ComplexMathFunctions/
│   ├── StatisticsFunctions/
│   ├── SupportFunctions/
│   └── CommonTables/
└── ComputeLibrary/         # Optional Neon acceleration
```

## Project Directory Layout

After setting up the libraries, your project should look like:

```
STM32H7S3L8_EMG_Port/
├── stm32h7rsxx-hal-driver/     # STM32H7RS HAL
├── STM32CubeH7RS/              # CMSIS package
│   └── Drivers/
│       └── CMSIS/
├── CMSIS-DSP/                  # ARM DSP library
├── src/                        # Your source files
├── inc/                        # Your header files
├── Makefile                    # Updated for new libraries
├── STM32H7S3L8_FLASH.ld       # Linker script
└── CLAUDE.md                   # Project documentation
```

## Build Configuration

The Makefile has been updated to reference the correct libraries:

### Include Paths:
- `stm32h7rsxx-hal-driver/Inc/`
- `STM32CubeH7RS/Drivers/CMSIS/Include/`
- `STM32CubeH7RS/Drivers/CMSIS/Core/Include/`
- `STM32CubeH7RS/Drivers/CMSIS/Device/ST/STM32H7RSxx/Include/`
- `CMSIS-DSP/Include/`
- `CMSIS-DSP/PrivateInclude/`

### Compiler Definitions:
- `-DSTM32H7S3L8xx` - Device identifier
- `-DUSE_HAL_DRIVER` - Enable HAL driver
- `-DUSE_FULL_LL_DRIVER` - Enable Low-Layer drivers
- `-DARM_MATH_CM7` - Enable Cortex-M7 DSP optimizations

### Key HAL Sources Used:
- `stm32h7rsxx_hal.c` - Core HAL
- `stm32h7rsxx_hal_gpio.c` - GPIO control
- `stm32h7rsxx_hal_spi.c` - SPI for ADS1299 communication
- `stm32h7rsxx_hal_uart.c` - UART for data transmission
- `stm32h7rsxx_hal_dma.c` - DMA for high-speed data transfer
- `stm32h7rsxx_hal_rcc.c` - Clock configuration
- `stm32h7rsxx_hal_cortex.c` - Cortex-M7 specific functions

## Build Commands

```bash
# Clean build
make clean

# Debug build with symbols
make debug

# Optimized release build
make release

# Flash to MCU
make install

# Memory usage analysis
make memory_usage
```

## Verification

To verify the libraries are set up correctly:

1. Check that include paths resolve:
   ```bash
   find . -name "stm32h7rsxx_hal.h"
   find . -name "arm_math.h"
   find . -name "core_cm7.h"
   ```

2. Test build:
   ```bash
   make clean && make debug
   ```

3. Check for successful compilation of HAL and DSP sources in the build output.

## Troubleshooting

**Missing headers:** Ensure all three repositories are cloned and the directory structure matches the Makefile paths.

**Compilation errors:** Verify the device definitions match your specific STM32H7S3L8 variant.

**Linker errors:** Check that the linker script (`STM32H7S3L8_FLASH.ld`) is compatible with your device's memory layout.

## License Information

- **STM32H7RS HAL:** BSD-3-Clause (STMicroelectronics)
- **CMSIS:** Apache License 2.0 (ARM)
- **CMSIS-DSP:** Apache License 2.0 (ARM)

Make sure to review the license files in each repository for compliance requirements.
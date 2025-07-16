# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is an STM32F407VG bare-metal embedded system using GNU ARM Embedded Toolchain. The build system is managed through a comprehensive Makefile.

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
- STM32F4xx DSP Standard Peripheral Library (referenced as `../STM32F4xx_DSP_StdPeriph_Lib_V*`)
- ST-Link utilities for flashing (`st-flash`, `st-util`)

## Architecture Overview

This is an **EMG (Electromyography) signal acquisition system** converted from the original EEG design. The system captures 8-channel muscle activity data at 1kHz sampling rate.

### Core Components

**Hardware Layer:**
- `src/cmd_ADS1299.c` - ADS1299 24-bit ADC configuration and communication
- `src/Peripheral_Init.c` - STM32F407VG peripheral initialization (SPI, USART, GPIO, EXTI)
- `src/cmd_USART.s` - Low-level USART communication routines

**Signal Processing Pipeline:**
- `src/coeffs_IIR.c` - IIR filter coefficients (10-450Hz EMG optimized)
- `src/IRQ.c` - Real-time signal processing in interrupt handlers
- `inc/main.h` - System configuration and filter parameters

**System Control:**
- `src/Settings_USART.c` - Terminal interface for runtime configuration
- `src/main.c` - System initialization and main loop

### Signal Processing Flow

1. **ADS1299 Data Acquisition** (1000 SPS, 6x gain)
2. **IIR Filtering** (High-pass: 10Hz, Low-pass: 450Hz)
3. **EMG Processing:**
   - Full-wave rectification
   - RMS calculation (muscle activity level)
   - Envelope detection (100ms window)
4. **Data Transmission** via USART

### Data Protocol

The system outputs multiple data types:
- `D` - Raw 24-bit ADC data
- `R` - RMS values (muscle activity)
- `E` - Envelope values (muscle activation)
- `I` - Impedance measurements
- `B` - Button state

## Filter Design System

The `filter_design/` directory contains Python tooling for generating EMG-optimized IIR filter coefficients.

### Filter Tool Usage

```bash
cd filter_design
pip install -r requirements.txt
python emg_filter_design.py
```

This generates:
- `emg_coeffs.c` - ARM CMSIS-DSP compatible coefficients
- `emg_filter_response.png` - Frequency response plots

### Filter Integration

When modifying filters:
1. Run filter design script
2. Copy coefficients from `emg_coeffs.c` to `src/coeffs_IIR.c`
3. Update `BIQUAD_STAGES_HP` and `BIQUAD_STAGES_BP` in `inc/main.h`
4. Rebuild firmware

## Key System Parameters

**EMG Configuration:**
- Sampling rate: 1000 SPS (configured in `src/cmd_ADS1299.c`)
- PGA gain: 6x (optimized for EMG amplitude range)
- Filter stages: 3 HP + 3 BP biquad sections
- Signal window: 1000 samples (1 second at 1kHz)

**Channel Management:**
- 8 channels configurable via `BIOEXG_SETTINGS` bitfield
- Runtime configuration through USART settings mode
- Individual channel power control and input selection

## EMG-Specific Features

This system was converted from EEG to EMG with these key changes:
- **4x higher sampling rate** (250 SPS → 1000 SPS)
- **Wider frequency range** (60-65Hz → 10-450Hz)
- **Lower gain** (24x → 6x) for EMG amplitude range
- **EMG signal processing** (rectification, RMS, envelope)

### Real-time Processing

The `EXTI0_IRQHandler()` in `src/IRQ.c` performs:
1. Batch processing of 1000-sample windows
2. IIR filtering using ARM CMSIS-DSP functions
3. EMG-specific calculations (RMS, envelope)
4. Data transmission formatting

## Development Notes

**Memory Architecture:**
- Filter coefficients are stored in Flash
- Signal buffers use 2D arrays: `[CHANNELS][WINDOW_SIZE]`
- ARM CMSIS-DSP library provides optimized filtering functions

**Interrupt Priorities:**
- `EXTI9_5_IRQHandler()` - Data acquisition (high priority)
- `EXTI0_IRQHandler()` - Signal processing (medium priority)
- `USART6_IRQHandler()` - Settings mode (low priority)

**Configuration Management:**
- Runtime settings stored in `BIOEXG_SETTINGS` bitfield
- Channel enable/disable: bits 0-7
- Impedance measurement: bits 12-19
- Settings mode accessible via 'S' command over USART

## Testing and Validation

The system includes impedance measurement capabilities for electrode quality assessment and test signal generation for calibration. The `filter_design/` tools include frequency response validation and stability verification for embedded implementation.
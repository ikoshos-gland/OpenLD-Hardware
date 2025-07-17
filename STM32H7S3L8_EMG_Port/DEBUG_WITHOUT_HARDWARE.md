# Debugging STM32H7S3L8 EMG Port Without Hardware

This guide shows how to debug, test, and validate your STM32H7S3L8 EMG project without physical hardware.

## 1. Compilation Testing

### Basic Build Verification
Test that your code compiles correctly:

```bash
# Test compilation without linking (faster)
make CFLAGS="-c -fsyntax-only" debug

# Full compilation to ELF (without flashing)
make debug

# Check binary size and sections
make memory_usage
```

### Compile-Only Target
Add this to your Makefile for faster syntax checking:

```makefile
compile-check:
	@echo "=== Syntax and Compilation Check ==="
	$(CC) $(CFLAGS) -fsyntax-only $(SRCDIR)/*.c
	@echo "✓ All source files compile successfully"
```

## 2. QEMU Emulation

### Install QEMU for ARM
```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-arm

# macOS
brew install qemu

# Arch Linux
sudo pacman -S qemu-system-arm
```

### Run STM32 Emulation
```bash
# Basic STM32F4 emulation (closest to H7 available)
qemu-system-arm -M netduinoplus2 -cpu cortex-m4 \
    -kernel bin/emg_h7s3l8.elf \
    -nographic -serial stdio \
    -monitor telnet:127.0.0.1:1234,server,nowait

# With GDB debugging
qemu-system-arm -M netduinoplus2 -cpu cortex-m4 \
    -kernel bin/emg_h7s3l8.elf \
    -nographic -serial stdio \
    -gdb tcp::3333 -S
```

### QEMU Debugging Session
```bash
# Terminal 1: Start QEMU
qemu-system-arm -M netduinoplus2 -cpu cortex-m4 \
    -kernel bin/emg_h7s3l8.elf -nographic -gdb tcp::3333 -S

# Terminal 2: Connect GDB
arm-none-eabi-gdb bin/emg_h7s3l8.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) break main
(gdb) continue
```

## 3. Static Analysis

### Cppcheck
Install and run static analysis:
```bash
# Install cppcheck
sudo apt-get install cppcheck  # Ubuntu/Debian
brew install cppcheck          # macOS

# Run analysis
cppcheck --enable=all --std=c11 \
    --platform=unix32 \
    --suppress=missingIncludeSystem \
    src/ inc/
```

### Clang Static Analyzer
```bash
# Install clang
sudo apt-get install clang clang-tools

# Run static analysis
scan-build make debug
```

## 4. Unit Testing Framework

### Create Test Environment
```bash
mkdir -p test/unit
cd test/unit
```

### DSP Function Testing
Create a test for your EMG processing functions:

```c
// test/unit/test_emg_processing.c
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "arm_math.h"

// Mock hardware functions
void mock_hardware_init() {
    printf("Mock: Hardware initialized\n");
}

// Test EMG RMS calculation
void test_emg_rms_calculation() {
    float32_t test_signal[100];
    float32_t rms_result;
    
    // Generate test signal (50Hz sine wave)
    for (int i = 0; i < 100; i++) {
        test_signal[i] = sinf(2 * M_PI * 50 * i / 1000.0f);
    }
    
    // Calculate RMS using ARM CMSIS-DSP
    arm_rms_f32(test_signal, 100, &rms_result);
    
    // Expected RMS of sine wave is amplitude/sqrt(2)
    float32_t expected = 1.0f / sqrtf(2.0f);
    float32_t tolerance = 0.01f;
    
    assert(fabsf(rms_result - expected) < tolerance);
    printf("✓ EMG RMS calculation test passed\n");
}

// Test IIR filter initialization
void test_iir_filter_init() {
    arm_biquad_cascade_df2T_instance_f32 filter;
    float32_t state[8];  // 2 * num_stages
    float32_t coeffs[20]; // 5 * num_stages
    
    // Initialize with dummy coefficients
    for (int i = 0; i < 20; i++) {
        coeffs[i] = 0.1f;
    }
    
    arm_biquad_cascade_df2T_init_f32(&filter, 4, coeffs, state);
    
    assert(filter.numStages == 4);
    assert(filter.pCoeffs == coeffs);
    assert(filter.pState == state);
    
    printf("✓ IIR filter initialization test passed\n");
}

int main() {
    printf("=== EMG Processing Unit Tests ===\n");
    
    mock_hardware_init();
    test_emg_rms_calculation();
    test_iir_filter_init();
    
    printf("✓ All tests passed!\n");
    return 0;
}
```

### Host-Based Test Makefile
```makefile
# test/unit/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I../../CMSIS-DSP/Include -I../../inc
LDFLAGS = -lm

# Use host-based CMSIS-DSP or create mocks
SOURCES = test_emg_processing.c
TARGET = test_emg

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all test clean
```

## 5. Hardware Abstraction Layer (HAL) Mocking

### Create Mock HAL
```c
// test/mocks/mock_hal.c
#include <stdio.h>
#include <stdint.h>

// Mock GPIO functions
void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, uint8_t PinState) {
    printf("Mock GPIO: Port=%p, Pin=0x%04X, State=%d\n", GPIOx, GPIO_Pin, PinState);
}

uint8_t HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin) {
    printf("Mock GPIO Read: Port=%p, Pin=0x%04X\n", GPIOx, GPIO_Pin);
    return 0; // Always return low
}

// Mock SPI functions
uint8_t HAL_SPI_Transmit(void* hspi, uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    printf("Mock SPI TX: Size=%d, Data[0]=0x%02X\n", Size, pData[0]);
    return 0; // HAL_OK
}

uint8_t HAL_SPI_Receive(void* hspi, uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    printf("Mock SPI RX: Size=%d\n", Size);
    // Return dummy ADS1299 data
    for (int i = 0; i < Size; i++) {
        pData[i] = 0x55; // Dummy data
    }
    return 0; // HAL_OK
}

// Mock UART functions
uint8_t HAL_UART_Transmit(void* huart, uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    printf("Mock UART TX: ");
    for (int i = 0; i < Size; i++) {
        printf("%c", pData[i]);
    }
    printf("\n");
    return 0; // HAL_OK
}
```

## 6. Debugging Tools Setup

### GDB with Text User Interface
```bash
# Create .gdbinit for better debugging
cat > .gdbinit << 'EOF'
set confirm off
set pagination off
target remote localhost:3333

# Load symbols
file bin/emg_h7s3l8.elf

# Useful breakpoints
break main
break HardFault_Handler
break ads1299_read_data

# Display registers
define show-regs
    info registers
    info float
end

# ARM-specific commands
set architecture arm
set endian little

echo "STM32H7S3L8 GDB setup complete\n"
EOF
```

### Memory Analysis
```bash
# Add memory analysis target to Makefile
memory-analysis: $(BINDIR)/$(BINELF)
	@echo "=== Memory Analysis ==="
	$(SIZE) -A $(BINDIR)/$(BINELF)
	@echo ""
	@echo "=== Section Details ==="
	$(OD) -h $(BINDIR)/$(BINELF) | head -20
	@echo ""
	@echo "=== Symbol Table (Functions) ==="
	arm-none-eabi-nm $(BINDIR)/$(BINELF) | grep -E " T " | head -20
```

## 7. Continuous Integration Testing

### GitHub Actions Workflow
```yaml
# .github/workflows/build-test.yml
name: STM32H7S3L8 Build Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive
        
    - name: Install ARM Toolchain
      run: |
        sudo apt-get update
        sudo apt-get install gcc-arm-none-eabi
        
    - name: Setup Libraries
      run: ./setup_libraries.sh
      
    - name: Verify Paths
      run: ./verify_paths.sh
      
    - name: Build Debug
      run: make debug
      
    - name: Build Release
      run: make release
      
    - name: Memory Usage Analysis
      run: make memory_usage
      
    - name: Static Analysis
      run: |
        sudo apt-get install cppcheck
        cppcheck --enable=all --error-exitcode=1 src/
```

## 8. Simulation and Modeling

### EMG Signal Simulation
```python
# tools/emg_simulator.py
import numpy as np
import matplotlib.pyplot as plt

def generate_emg_signal(duration=1.0, fs=1000, noise_level=0.1):
    """Generate synthetic EMG signal for testing"""
    t = np.linspace(0, duration, int(fs * duration))
    
    # EMG characteristics: 10-450 Hz frequency range
    # Multiple frequency components
    emg = np.zeros_like(t)
    
    # Add multiple frequency components typical of EMG
    frequencies = [50, 120, 200, 350]  # Hz
    amplitudes = [0.8, 0.6, 0.4, 0.2]
    
    for freq, amp in zip(frequencies, amplitudes):
        emg += amp * np.sin(2 * np.pi * freq * t)
    
    # Add noise
    emg += noise_level * np.random.randn(len(t))
    
    # Apply muscle activation envelope (burst pattern)
    envelope = np.where((t % 0.3) < 0.1, 1.0, 0.2)
    emg *= envelope
    
    return t, emg

def save_test_data(filename, signal, fs=1000):
    """Save test data in format compatible with firmware"""
    # Convert to 24-bit integer (ADS1299 format)
    signal_int = (signal * (2**23 - 1)).astype(np.int32)
    
    with open(filename, 'w') as f:
        f.write("// Generated EMG test data\n")
        f.write(f"const int32_t test_emg_data[{len(signal_int)}] = {{\n")
        for i, sample in enumerate(signal_int):
            f.write(f"    {sample}")
            if i < len(signal_int) - 1:
                f.write(",")
            if (i + 1) % 8 == 0:
                f.write("\n")
        f.write("\n};\n")

if __name__ == "__main__":
    t, emg = generate_emg_signal()
    save_test_data("test_emg_data.h", emg)
    
    plt.figure(figsize=(12, 6))
    plt.plot(t[:200], emg[:200])  # First 200ms
    plt.title("Synthetic EMG Signal")
    plt.xlabel("Time (s)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.savefig("emg_test_signal.png")
    print("Test data generated: test_emg_data.h")
```

## 9. Quick Debug Commands

```bash
# Add these to your shell profile for quick access
alias emg-build="make clean && make debug"
alias emg-check="./verify_paths.sh"
alias emg-qemu="qemu-system-arm -M netduinoplus2 -cpu cortex-m4 -kernel bin/emg_h7s3l8.elf -nographic -serial stdio"
alias emg-gdb="arm-none-eabi-gdb bin/emg_h7s3l8.elf"
alias emg-size="make memory_usage"
```

## Summary

You can debug extensively without hardware by:

1. **Compilation testing** - Verify syntax and linking
2. **QEMU emulation** - Run code in virtual environment  
3. **Static analysis** - Find potential bugs
4. **Unit testing** - Test individual functions
5. **Memory analysis** - Check resource usage
6. **Signal simulation** - Test with synthetic EMG data

This approach will help you catch most issues before you get the physical microcontroller!
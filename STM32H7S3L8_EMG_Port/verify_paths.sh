#!/bin/bash

# Path verification script for STM32H7S3L8 EMG project
# This script checks if all the library paths in the Makefile are correct

echo "=== STM32H7S3L8 EMG Port - Path Verification ==="
echo ""

# Extract paths from Makefile
STM_DIR="stm32h7rsxx-hal-driver"
HAL_SRC="$STM_DIR/Src"
HAL_INC="$STM_DIR/Inc"

CMSIS_DIR="STM32CubeH7RS/Drivers/CMSIS"
CMSIS_INC="$CMSIS_DIR/Include"
CMSIS_DEVICE_INC="$CMSIS_DIR/Device/ST/STM32H7RSxx/Include"
CMSIS_CORE_INC="$CMSIS_DIR/Core/Include"

DSP_DIR="CMSIS-DSP/Source"
DSP_INC="CMSIS-DSP/Include"
DSP_PRIV_INC="CMSIS-DSP/PrivateInclude"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to check directory
check_dir() {
    local dir_path=$1
    local description=$2
    
    if [ -d "$dir_path" ]; then
        echo -e "${GREEN}✓${NC} $description: $dir_path"
        return 0
    else
        echo -e "${RED}✗${NC} $description: $dir_path (NOT FOUND)"
        return 1
    fi
}

# Function to check critical files
check_file() {
    local file_path=$1
    local description=$2
    
    if [ -f "$file_path" ]; then
        echo -e "${GREEN}✓${NC} $description: $file_path"
        return 0
    else
        echo -e "${RED}✗${NC} $description: $file_path (NOT FOUND)"
        return 1
    fi
}

echo "Checking STM32H7RS HAL paths..."
echo "────────────────────────────────────"
check_dir "$STM_DIR" "HAL Root Directory"
check_dir "$HAL_SRC" "HAL Source Directory"
check_dir "$HAL_INC" "HAL Include Directory"

# Check key HAL files
check_file "$HAL_INC/stm32h7rsxx_hal.h" "Main HAL Header"
check_file "$HAL_INC/stm32h7rsxx_hal_gpio.h" "GPIO HAL Header"
check_file "$HAL_INC/stm32h7rsxx_hal_spi.h" "SPI HAL Header"
check_file "$HAL_INC/stm32h7rsxx_hal_uart.h" "UART HAL Header"

echo ""
echo "Checking CMSIS paths..."
echo "─────────────────────────"
check_dir "$CMSIS_DIR" "CMSIS Root Directory"
check_dir "$CMSIS_INC" "CMSIS Include Directory"
check_dir "$CMSIS_CORE_INC" "CMSIS Core Include Directory"
check_dir "$CMSIS_DEVICE_INC" "CMSIS Device Include Directory"

# Check key CMSIS files
check_file "$CMSIS_INC/cmsis_compiler.h" "CMSIS Compiler Header"
check_file "$CMSIS_CORE_INC/core_cm7.h" "Cortex-M7 Core Header"
check_file "$CMSIS_DEVICE_INC/stm32h7rsxx.h" "STM32H7RSxx Device Header"

echo ""
echo "Checking CMSIS-DSP paths..."
echo "──────────────────────────"
check_dir "CMSIS-DSP" "CMSIS-DSP Root Directory"
check_dir "$DSP_DIR" "DSP Source Directory"
check_dir "$DSP_INC" "DSP Include Directory"
check_dir "$DSP_PRIV_INC" "DSP Private Include Directory"

# Check key DSP files
check_file "$DSP_INC/arm_math.h" "ARM Math Header"
check_file "$DSP_INC/arm_math_types.h" "ARM Math Types Header"

# Check some key DSP source directories
check_dir "$DSP_DIR/BasicMathFunctions" "Basic Math Functions"
check_dir "$DSP_DIR/FilteringFunctions" "Filtering Functions"
check_dir "$DSP_DIR/TransformFunctions" "Transform Functions"

echo ""
echo "Checking project structure..."
echo "────────────────────────────"
check_dir "src" "Project Source Directory"
check_dir "inc" "Project Include Directory"
check_file "Makefile" "Project Makefile"
check_file "STM32H7S3L8_FLASH.ld" "Linker Script"

echo ""
echo "=== Makefile Path Analysis ==="
echo ""

# Extract and verify paths from Makefile
echo "Paths defined in Makefile:"
echo "  STM_DIR = $STM_DIR"
echo "  HAL_SRC = $HAL_SRC"
echo "  HAL_INC = $HAL_INC"
echo "  CMSIS_DIR = $CMSIS_DIR"
echo "  CMSIS_INC = $CMSIS_INC"
echo "  CMSIS_DEVICE_INC = $CMSIS_DEVICE_INC"
echo "  CMSIS_CORE_INC = $CMSIS_CORE_INC"
echo "  DSP_DIR = $DSP_DIR"
echo "  DSP_INC = $DSP_INC"
echo "  DSP_PRIV_INC = $DSP_PRIV_INC"

echo ""
echo "=== Recommendations ==="
echo ""

# Count missing items
missing_count=0

# Check each path and count missing ones
for path in "$STM_DIR" "$HAL_SRC" "$HAL_INC" "$CMSIS_DIR" "$CMSIS_INC" "$CMSIS_CORE_INC" "$CMSIS_DEVICE_INC" "$DSP_DIR" "$DSP_INC" "$DSP_PRIV_INC"; do
    if [ ! -d "$path" ]; then
        ((missing_count++))
    fi
done

if [ $missing_count -eq 0 ]; then
    echo -e "${GREEN}✓ All paths are correct! You can proceed with building.${NC}"
    echo ""
    echo "Next steps:"
    echo "  make clean && make debug"
else
    echo -e "${RED}✗ $missing_count path(s) missing. Run the setup script first:${NC}"
    echo ""
    echo "  ./setup_libraries.sh"
    echo ""
    echo "Then run this verification script again."
fi

echo ""
echo "=== Path Verification Complete ==="
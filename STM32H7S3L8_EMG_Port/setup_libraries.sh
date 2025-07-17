#!/bin/bash

# STM32H7S3L8 EMG Port - Library Setup Script
# This script downloads and sets up the required libraries

set -e  # Exit on any error

echo "Setting up STM32H7S3L8 EMG Port libraries..."

PROJECT_DIR=$(pwd)

# Function to check if git is available
check_git() {
    if ! command -v git &> /dev/null; then
        echo "Error: git is not installed. Please install git first."
        exit 1
    fi
}

# Function to clone or update repository
clone_or_update() {
    local repo_url=$1
    local target_dir=$2
    local commit_hash=$3
    
    if [ -d "$target_dir" ]; then
        echo "Directory $target_dir already exists. Updating..."
        cd "$target_dir"
        git fetch origin
        git submodule update --init --recursive
        if [ ! -z "$commit_hash" ]; then
            git checkout "$commit_hash"
            git submodule update --init --recursive
        else
            git pull origin main || git pull origin master
            git submodule update --init --recursive
        fi
        cd "$PROJECT_DIR"
    else
        echo "Cloning $repo_url into $target_dir with submodules..."
        git clone --recursive "$repo_url" "$target_dir"
        if [ ! -z "$commit_hash" ]; then
            cd "$target_dir"
            git checkout "$commit_hash"
            git submodule update --init --recursive
            cd "$PROJECT_DIR"
        fi
    fi
}

# Check prerequisites
check_git

echo "Starting library setup in: $PROJECT_DIR"

# 1. STM32H7RS HAL Driver
echo "=== Setting up STM32H7RS HAL Driver ==="
clone_or_update \
    "https://github.com/STMicroelectronics/stm32h7rsxx-hal-driver.git" \
    "stm32h7rsxx-hal-driver" \
    "9e83b95ae0f70faa067eddce2da617d180937f9b"

# 2. STM32CubeH7RS CMSIS
echo "=== Setting up STM32CubeH7RS CMSIS ==="
if [ ! -d "STM32CubeH7RS" ]; then
    echo "Cloning STM32CubeH7RS with submodules..."
    git clone --recursive "https://github.com/STMicroelectronics/STM32CubeH7RS.git"
    echo "Note: Full repository cloned due to submodule requirements. You can remove unnecessary directories after setup if needed."
else
    echo "STM32CubeH7RS already exists. Updating submodules..."
    cd STM32CubeH7RS
    git submodule update --init --recursive
    cd "$PROJECT_DIR"
fi

# 3. ARM CMSIS-DSP
echo "=== Setting up ARM CMSIS-DSP ==="
clone_or_update \
    "https://github.com/ARM-software/CMSIS-DSP.git" \
    "CMSIS-DSP" \
    ""

# Verify setup
echo "=== Verifying library setup ==="

check_file() {
    if [ -f "$1" ]; then
        echo "✓ Found: $1"
    else
        echo "✗ Missing: $1"
        return 1
    fi
}

check_dir() {
    if [ -d "$1" ]; then
        echo "✓ Found directory: $1"
    else
        echo "✗ Missing directory: $1"
        return 1
    fi
}

echo "Checking STM32H7RS HAL..."
check_file "stm32h7rsxx-hal-driver/Inc/stm32h7rsxx_hal.h"
check_file "stm32h7rsxx-hal-driver/Src/stm32h7rsxx_hal.c"

echo "Checking CMSIS..."
check_file "STM32CubeH7RS/Drivers/CMSIS/Include/cmsis_compiler.h"
check_file "STM32CubeH7RS/Drivers/CMSIS/Core/Include/core_cm7.h"
check_dir "STM32CubeH7RS/Drivers/CMSIS/Device/ST/STM32H7RSxx/Include"

echo "Checking CMSIS-DSP..."
check_file "CMSIS-DSP/Include/arm_math.h"
check_dir "CMSIS-DSP/Source"

echo ""
echo "=== Library Setup Complete ==="
echo "Libraries installed in:"
echo "  - stm32h7rsxx-hal-driver/"
echo "  - STM32CubeH7RS/Drivers/CMSIS/"
echo "  - CMSIS-DSP/"
echo ""
echo "You can now build the project with:"
echo "  make clean && make debug"
echo ""
echo "For more information, see LIBRARY_SETUP.md"
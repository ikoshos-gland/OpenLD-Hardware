# STM32H7S3L8 EMG Build Commands Reference

## Overview

This document provides a comprehensive reference for all available make commands in the STM32H7S3L8 EMG signal acquisition system. The build system implements enterprise-grade development practices with strict validation, comprehensive testing, and multiple analysis layers.

## Prerequisites

### Required Tools
- **ARM GCC Toolchain**: `arm-none-eabi-gcc` for embedded builds
- **Clang**: For host-based static analysis and sanitizers
- **ST-Link Tools**: `st-flash` and `st-util` for hardware programming
- **CPPcheck**: For enhanced static analysis
- **clang-tidy**: For comprehensive code analysis
- **Unity Framework**: For unit testing (included in project)

### Optional Tools
- **gcov**: For code coverage analysis
- **bear**: For improved compile_commands.json generation
- **PVS-Studio**: For enterprise static analysis (future enhancement)

## Basic Build Commands

### 🔨 Standard Build Targets

#### `make all` / `make release`
- **Purpose**: Builds optimized firmware for production use
- **Optimization**: `-O3 -flto` (Link Time Optimization)
- **Output**: `bin/emg_h7s3l8.bin` and `bin/emg_h7s3l8.elf`
- **Use Case**: Final production firmware
- **Build Time**: ~30 seconds

#### `make debug`
- **Purpose**: Builds debug version with full symbols
- **Flags**: `-O0 -g3` (no optimization, maximum debug info)
- **Defines**: `USE_FULL_ASSERT` and `DEBUG`
- **Output**: Debug-enabled firmware with assertions
- **Use Case**: Development and debugging
- **Build Time**: ~25 seconds

#### `make clean`
- **Purpose**: Removes all build artifacts
- **Cleans**: `obj/` and `bin/` directories
- **Use Case**: Fresh build environment
- **Time**: Instant

### 🔄 Hardware Interface

#### `make install` / `make flash`
- **Purpose**: Flashes firmware to STM32H7S3L8 MCU
- **Address**: 0x08000000 (Flash memory start)
- **Tool**: st-flash
- **Prerequisites**: ST-Link connected, release build completed
- **Time**: ~10 seconds

#### `make debug_session`
- **Purpose**: Starts interactive GDB debug session
- **Tools**: st-util + arm-none-eabi-gdb
- **Features**: TUI interface, hardware breakpoints
- **Prerequisites**: Debug build, ST-Link connected
- **Usage**: Interactive debugging with hardware

## Strict Build System (Zero Warnings Policy)

### ⚠️ Strict Build Targets

#### `make strict-build`
- **Purpose**: Builds with zero warnings policy enforcement
- **Flags**: `-Wall -Wextra -Werror -pedantic` + 20+ additional warnings
- **Policy**: Any warning = build failure
- **Use Case**: Code quality validation
- **Build Time**: ~35 seconds (+17% vs standard)

#### `make strict-debug`
- **Purpose**: Debug build with strict validation
- **Combines**: Debug flags + strict warnings
- **Output**: Debug firmware with zero warnings
- **Use Case**: Development with quality enforcement

#### `make strict-release`
- **Purpose**: Production build with strict validation
- **Combines**: Release optimization + strict warnings
- **Output**: Optimized firmware with zero warnings
- **Use Case**: Production-ready firmware

### 🔍 Warning Categories (Strict Mode)
- **Core Warnings**: `-Wall -Wextra -Werror -pedantic`
- **Code Quality**: `-Wshadow -Wdouble-promotion -Wformat=2`
- **Security**: `-Werror=format-security -Wcast-align -Wcast-qual`
- **Maintainability**: `-Wwrite-strings -Wswitch-default -Wswitch-enum`
- **Reliability**: `-Wnull-dereference -Wunreachable-code -Wfloat-equal`
- **Best Practices**: `-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations`

## Host-Only Static Analysis

### 🖥️ Host Analysis Targets

#### `make host-check`
- **Purpose**: Clang-based syntax validation for x86_64 host
- **Sanitizers**: AddressSanitizer + UndefinedBehaviorSanitizer
- **Flags**: `-fsanitize=undefined,address -fsyntax-only`
- **Target**: x86_64-pc-linux-gnu
- **Use Case**: Cross-platform compatibility validation
- **Time**: ~15 seconds

#### `make host-sanitize`
- **Purpose**: Comprehensive sanitizer analysis
- **Focus**: Memory safety and undefined behavior detection
- **Output**: Sanitizer reports in `host_analysis/`
- **Use Case**: Runtime error detection without hardware
- **Time**: ~20 seconds

## Advanced Static Analysis

### 🔬 Static Analysis Targets

#### `make static-analysis`
- **Purpose**: Enhanced CPPcheck analysis
- **Flags**: `--enable=all --inconclusive --force --error-exitcode=1`
- **Standards**: C11 compliance checking
- **Output**: `analysis_reports/cppcheck_enhanced.txt`
- **Use Case**: Deep code analysis and bug detection
- **Time**: ~45 seconds

#### `make clang-static-analysis`
- **Purpose**: Clang Static Analyzer with security focus
- **Checkers**: core, unix, security, nullability
- **Output**: Analysis reports per source file
- **Use Case**: Security vulnerability detection
- **Time**: ~30 seconds

#### `make clang-tidy-analysis`
- **Purpose**: Comprehensive clang-tidy validation
- **Checks**: 100+ rules across multiple categories
- **Features**: Generates `compile_commands.json`
- **Categories**: clang-analyzer, misc, cert, readability, performance, bugprone
- **Output**: `analysis_reports/clang-tidy-comprehensive.txt`
- **Time**: ~60 seconds

#### `make advanced-static-analysis`
- **Purpose**: Combines all three static analysis tools
- **Sequence**: CPPcheck → Clang Analyzer → Clang-tidy
- **Output**: Comprehensive analysis suite
- **Use Case**: Complete code quality assessment
- **Time**: ~2 minutes

#### `make comprehensive-static-analysis`
- **Purpose**: Complete static analysis including compliance
- **Includes**: Advanced analysis + MISRA + CERT + Security
- **Output**: All analysis reports
- **Use Case**: Enterprise-grade code validation
- **Time**: ~3 minutes

### 📋 Compliance and Security

#### `make misra-compliance`
- **Purpose**: MISRA C:2012 compliance checking
- **Tool**: CPPcheck with MISRA addon
- **Output**: `misra_report.txt`
- **Use Case**: Automotive/medical device compliance
- **Time**: ~40 seconds

#### `make cert-compliance`
- **Purpose**: CERT C secure coding standards validation
- **Tool**: CPPcheck with CERT addon
- **Output**: `cert_report.txt`
- **Use Case**: Security-critical applications
- **Time**: ~40 seconds

#### `make security-analysis`
- **Purpose**: Security vulnerability scanning
- **Focus**: Buffer overflows, memory leaks, null pointers
- **Output**: `security_report.txt`
- **Use Case**: Security audit and validation
- **Time**: ~35 seconds

## Validation Pipelines

### 🚀 Comprehensive Validation

#### `make validate-all`
- **Purpose**: Complete validation pipeline
- **Sequence**: 
  1. Clean build environment
  2. Syntax and compilation check
  3. Host-only static analysis
  4. Strict build validation
  5. Comprehensive static analysis
- **Use Case**: Full project validation
- **Time**: ~4-5 minutes

#### `make ci-ready`
- **Purpose**: CI/CD-ready validation
- **Sequence**:
  1. Clean build environment
  2. Syntax validation
  3. Host static analysis
  4. Sanitizer validation
  5. Strict build (zero warnings)
  6. Advanced static analysis
- **Use Case**: Continuous integration pipeline
- **Time**: ~3-4 minutes

#### `make production-ready`
- **Purpose**: Production deployment validation
- **Sequence**:
  1. Clean build environment
  2. Strict release build
  3. Comprehensive analysis
  4. Memory usage analysis
- **Use Case**: Pre-deployment validation
- **Time**: ~3-4 minutes

## Testing Framework

### 🧪 Unit Testing

#### `make unit-tests`
- **Purpose**: Runs Unity unit tests
- **Tests**: EMG processing + Communication protocols
- **Framework**: Unity testing framework
- **Output**: Test results and pass/fail status
- **Time**: ~10 seconds

#### `make test-emg`
- **Purpose**: EMG-specific processing tests
- **Focus**: Signal processing, filtering, feature extraction
- **Output**: EMG processing validation results
- **Time**: ~5 seconds

#### `make test-communication`
- **Purpose**: Communication protocol tests
- **Focus**: UART, SPI, data transmission
- **Output**: Communication validation results
- **Time**: ~5 seconds

#### `make test-all`
- **Purpose**: Complete test suite execution
- **Includes**: All unit tests + integration tests
- **Output**: Comprehensive test results
- **Time**: ~15 seconds

### 📊 Test Analysis

#### `make coverage-report`
- **Purpose**: Generates code coverage reports
- **Tool**: gcov with profile-guided coverage
- **Output**: Coverage reports in `coverage_reports/`
- **Metrics**: Line coverage, branch coverage, function coverage
- **Time**: ~30 seconds

#### `make performance-test`
- **Purpose**: Performance benchmarking
- **Focus**: EMG processing performance timing
- **Output**: Performance metrics and benchmarks
- **Time**: ~15 seconds

#### `make comprehensive-test`
- **Purpose**: Complete testing pipeline
- **Includes**: Compilation + Static analysis + Unit tests + Coverage + Performance
- **Output**: All test and analysis reports
- **Time**: ~5-6 minutes

## Analysis and Debugging

### 📈 Memory and Performance

#### `make memory_usage`
- **Purpose**: Shows memory usage analysis
- **Tool**: arm-none-eabi-size
- **Output**: Memory sections (Flash, RAM, Stack)
- **Available Memory**: 2MB Flash, 1MB RAM (distributed)
- **Time**: ~2 seconds

#### `make memory-analysis`
- **Purpose**: Detailed memory analysis
- **Features**: Section breakdown, symbol table, memory map
- **Output**: Detailed memory usage report
- **Use Case**: Memory optimization and debugging
- **Time**: ~5 seconds

#### `make performance`
- **Purpose**: Shows performance metrics
- **Metrics**: CPU @ 550MHz, DSP capabilities, cache info
- **Output**: System performance characteristics
- **Time**: Instant

### 🔧 Utility Commands

#### `make compile-check`
- **Purpose**: Syntax and compilation verification
- **Flags**: `-fsyntax-only`
- **Use Case**: Quick syntax validation without building
- **Time**: ~10 seconds

#### `make no-hardware-test`
- **Purpose**: Testing without hardware
- **Includes**: Compile check + Static analysis + Memory analysis
- **Use Case**: Development without hardware access
- **Time**: ~2 minutes

#### `make help`
- **Purpose**: Shows comprehensive help
- **Output**: All available commands with descriptions
- **Use Case**: Command reference
- **Time**: Instant

## Cleaning Commands

### 🧹 Cleanup Targets

#### `make clean`
- **Purpose**: Basic build artifact cleanup
- **Removes**: `obj/` and `bin/` directories
- **Time**: Instant

#### `make clean-tests`
- **Purpose**: Removes test artifacts
- **Removes**: Test executables, coverage reports, test logs
- **Time**: Instant

#### `make clean-strict`
- **Purpose**: Removes strict build artifacts
- **Removes**: Analysis reports, host analysis, logs
- **Time**: Instant

#### `make clean-all`
- **Purpose**: Complete cleanup
- **Removes**: All build artifacts, test reports, analysis files
- **Time**: ~2 seconds

## Command Reference Table

| Command | Category | Purpose | Time | Dependencies |
|---------|----------|---------|------|-------------|
| `make all` | Build | Production build | ~30s | ARM GCC |
| `make debug` | Build | Debug build | ~25s | ARM GCC |
| `make strict-build` | Quality | Zero warnings build | ~35s | ARM GCC |
| `make host-check` | Analysis | Host static analysis | ~15s | Clang |
| `make static-analysis` | Analysis | CPPcheck analysis | ~45s | CPPcheck |
| `make validate-all` | Pipeline | Complete validation | ~4-5m | All tools |
| `make ci-ready` | Pipeline | CI validation | ~3-4m | All tools |
| `make unit-tests` | Testing | Unit tests | ~10s | Unity |
| `make flash` | Hardware | Flash firmware | ~10s | ST-Link |
| `make clean` | Utility | Clean build | Instant | None |

## Usage Examples

### Development Workflow
```bash
# Start development
make clean
make debug
make debug_session

# Code quality check
make strict-build
make host-check
make static-analysis
```

### CI/CD Pipeline
```bash
# Complete CI validation
make ci-ready

# Or individual steps
make clean
make compile-check
make host-check
make strict-build
make unit-tests
```

### Production Deployment
```bash
# Production validation
make production-ready

# Deploy to hardware
make flash
```

### Quality Assurance
```bash
# Comprehensive validation
make validate-all

# Testing and coverage
make test-all
make coverage-report
```

## Performance Characteristics

### Build Times (on typical development machine)
- **Standard Build**: ~30 seconds
- **Debug Build**: ~25 seconds
- **Strict Build**: ~35 seconds (+17% overhead)
- **Host Check**: ~15 seconds
- **Static Analysis**: ~45 seconds
- **Complete Validation**: ~4-5 minutes

### Memory Usage (STM32H7S3L8)
- **Flash**: 2048KB available
- **AXI SRAM**: 512KB (high-speed data)
- **SRAM1**: 128KB (filter states)
- **SRAM2**: 128KB (processed data)
- **SRAM3**: 32KB (system variables)
- **SRAM4**: 64KB (additional storage)

### CPU Performance
- **Core**: ARM Cortex-M7 @ 550MHz
- **FPU**: Single/Double precision floating point
- **DSP**: Enhanced ARM CMSIS-DSP library
- **Cache**: 32KB I-Cache + 32KB D-Cache
- **Performance**: ~2.5x faster than STM32F407VG

## Troubleshooting

### Common Issues

#### Build Failures
- **Missing Tools**: Install ARM GCC toolchain
- **Permission Issues**: Check ST-Link permissions
- **Memory Issues**: Use `make clean` before rebuild

#### Static Analysis Warnings
- **External Libraries**: Warnings in HAL/CMSIS are suppressed
- **Host Check**: Pointer casting warnings in external libs are suppressed
- **Strict Mode**: All project warnings must be fixed

#### Test Failures
- **Unity Framework**: Ensure Unity is properly included
- **Mock Hardware**: Check mock implementations
- **Coverage**: Ensure gcov is installed for coverage reports

### Performance Optimization
- **Parallel Builds**: Use `make -j4` for faster compilation
- **Incremental Builds**: Only changed files are recompiled
- **LTO**: Link Time Optimization enabled in release builds
- **Cache**: Proper cache management for embedded target

## Future Enhancements

### Planned Features
- **PVS-Studio Integration**: Enterprise static analysis
- **Coverity Support**: Advanced security scanning
- **Automated Performance Profiling**: Runtime analysis
- **Enhanced Test Coverage**: Automated test generation

### Tool Integration
- **clang-format**: Automated code formatting
- **include-what-you-use**: Header optimization
- **bear**: Improved compile database generation
- **Docker**: Containerized build environment

---

*This documentation covers the comprehensive build system for the STM32H7S3L8 EMG signal acquisition system. For updates and additional information, refer to the project's CLAUDE.md file.*
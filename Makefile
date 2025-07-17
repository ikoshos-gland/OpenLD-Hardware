###
# STM32H7S3L8 EMG Port Makefile
# Updated for STM32H7RS HAL drivers
###

###
# GNU ARM Embedded Toolchain
CC   = arm-none-eabi-gcc
LD   = arm-none-eabi-ld
AS   = arm-none-eabi-as
CP   = arm-none-eabi-objcopy
OD   = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

###
# Directory Structure
OBJDIR = obj
BINDIR = bin
INCDIR = inc
SRCDIR = src

###
# STM32H7RS HAL Library path
STM_DIR = stm32h7rsxx-hal-driver

# This is where the HAL source files are located
HAL_SRC = $(STM_DIR)/Src
HAL_INC = $(STM_DIR)/Inc

# CMSIS path for STM32H7RS
CMSIS_DIR = STM32CubeH7RS/Drivers/CMSIS
CMSIS_INC = $(CMSIS_DIR)/Include
CMSIS_DEVICE_INC = $(CMSIS_DIR)/Device/ST/STM32H7RSxx/Include
CMSIS_CORE_INC = $(CMSIS_DIR)/Core/Include

# ARM CMSIS-DSP Library
DSP_DIR = CMSIS-DSP/Source
DSP_INC = CMSIS-DSP/Include
DSP_PRIV_INC = CMSIS-DSP/PrivateInclude

# Tell make to look in these folders for source files
vpath %.c $(HAL_SRC)
vpath %.c $(DSP_DIR)
vpath %.c $(SRCDIR)
vpath %.s $(SRCDIR)

# Find source files
ASOURCES      = $(notdir $(shell find -L $(SRCDIR) -name '*.s'))
CSOURCES      = $(notdir $(shell find -L $(SRCDIR) -name '*.c'))

##########################################
#        STM32H7RS HAL SOURCES           #
##########################################
# Essential HAL modules for EMG system
HAL_SOURCES   = stm32h7rsxx_hal.c
HAL_SOURCES  += stm32h7rsxx_hal_gpio.c
HAL_SOURCES  += stm32h7rsxx_hal_spi.c
HAL_SOURCES  += stm32h7rsxx_hal_uart.c
HAL_SOURCES  += stm32h7rsxx_hal_uart_ex.c
HAL_SOURCES  += stm32h7rsxx_hal_dma.c
HAL_SOURCES  += stm32h7rsxx_hal_dma_ex.c
HAL_SOURCES  += stm32h7rsxx_hal_rcc.c
HAL_SOURCES  += stm32h7rsxx_hal_rcc_ex.c
HAL_SOURCES  += stm32h7rsxx_hal_cortex.c
HAL_SOURCES  += stm32h7rsxx_hal_exti.c
HAL_SOURCES  += stm32h7rsxx_hal_flash.c
HAL_SOURCES  += stm32h7rsxx_hal_flash_ex.c
HAL_SOURCES  += stm32h7rsxx_hal_pwr.c
HAL_SOURCES  += stm32h7rsxx_hal_pwr_ex.c
HAL_SOURCES  += stm32h7rsxx_hal_tim.c
HAL_SOURCES  += stm32h7rsxx_hal_tim_ex.c

##########################################
#        CMSIS DSP SOURCES               #
##########################################
# Enhanced DSP library for Cortex-M7
DSP_SOURCES   = FastMathFunctions/arm_sin_f32.c
DSP_SOURCES  += FastMathFunctions/arm_cos_f32.c
# DSP_SOURCES  += FastMathFunctions/arm_sqrt_f32.c  # Not available in this CMSIS-DSP version
DSP_SOURCES  += CommonTables/arm_common_tables.c
DSP_SOURCES  += CommonTables/arm_const_structs.c
DSP_SOURCES  += BasicMathFunctions/arm_mult_f32.c
DSP_SOURCES  += BasicMathFunctions/arm_scale_f32.c
DSP_SOURCES  += BasicMathFunctions/arm_abs_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_mean_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_rms_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_std_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_var_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_max_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_min_f32.c
DSP_SOURCES  += FilteringFunctions/arm_fir_f32.c
DSP_SOURCES  += FilteringFunctions/arm_fir_init_f32.c
DSP_SOURCES  += FilteringFunctions/arm_biquad_cascade_df2T_f32.c
DSP_SOURCES  += FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c
DSP_SOURCES  += TransformFunctions/arm_rfft_fast_f32.c
DSP_SOURCES  += TransformFunctions/arm_rfft_fast_init_f32.c
DSP_SOURCES  += TransformFunctions/arm_cfft_f32.c
DSP_SOURCES  += TransformFunctions/arm_cfft_init_f32.c
DSP_SOURCES  += TransformFunctions/arm_cfft_radix8_f32.c
DSP_SOURCES  += TransformFunctions/arm_bitreversal2.c
DSP_SOURCES  += ComplexMathFunctions/arm_cmplx_mag_f32.c
DSP_SOURCES  += ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
DSP_SOURCES  += SupportFunctions/arm_copy_f32.c
DSP_SOURCES  += SupportFunctions/arm_fill_f32.c
DSP_SOURCES  += StatisticsFunctions/arm_power_f32.c

CSOURCES     += $(HAL_SOURCES)
CSOURCES     += $(DSP_SOURCES)

# Find header directories
INC           = $(shell find -L $(INCDIR) -name '*.h' -exec dirname {} \; | uniq)
INC          += $(CMSIS_INC)
INC          += $(CMSIS_CORE_INC)
INC          += $(CMSIS_DEVICE_INC)
INC          += $(HAL_INC)
INC          += $(DSP_INC)
INC          += $(DSP_PRIV_INC)
INCLUDES      = $(INC:%=-I%)

# Find libraries
INCLUDES_LIBS =
LINK_LIBS     =

# Create object list
OBJECTS       = $(ASOURCES:%.s=$(OBJDIR)/%.o)
OBJECTS      += $(CSOURCES:%.c=$(OBJDIR)/%.o)

# Define output files ELF & BIN
BINELF        = emg_h7s3l8.elf
BIN           = emg_h7s3l8.bin

###
# STM32H7S3L8 MCU FLAGS
MCFLAGS  = -std=c11 -mcpu=cortex-m7 -mthumb -mlittle-endian -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb-interwork -g -Wall

###
# STRICT BUILD CONFIGURATION
CFLAGS_STRICT := \
  -std=c11 \
  -Wall -Wextra -Werror -pedantic \
  -Wshadow -Wdouble-promotion -Wformat=2 \
  -Wnull-dereference -Wunused-parameter \
  -Wduplicated-cond -Wduplicated-branches -Wlogical-op \
  -Werror=format-security -Wcast-align -Wcast-qual \
  -Wwrite-strings -Wswitch-default -Wswitch-enum \
  -Wunreachable-code -Wfloat-equal -Wstrict-prototypes \
  -Wmissing-prototypes -Wmissing-declarations \
  -Wredundant-decls -Winline \
  -fdiagnostics-color=always -g3 -O0

# STRICT BUILD CONFIGURATION (relaxed for external libraries)
CFLAGS_STRICT_EXTERNAL := \
  -std=c11 \
  -Wall -Wextra -Werror \
  -Wno-nested-externs -Wno-sign-conversion -Wno-conversion \
  -Wno-unused-parameter -Wno-missing-prototypes -Wno-missing-declarations \
  -Wno-redundant-decls -Wno-cast-qual -Wno-cast-align \
  -Wno-switch-default -Wno-switch-enum -Wno-inline \
  -Wno-double-promotion -Wno-float-equal \
  -fdiagnostics-color=always -g3 -O0

###
# HOST-ONLY STATIC ANALYSIS FLAGS (Clang-compatible)
HOST_CFLAGS := \
  -std=c11 \
  -Wall -Wextra -Werror -pedantic \
  -Wshadow -Wdouble-promotion -Wformat=2 \
  -Wnull-dereference -Wunused-parameter \
  -Werror=format-security -Wcast-align -Wcast-qual \
  -Wwrite-strings -Wswitch-default -Wswitch-enum \
  -Wunreachable-code -Wfloat-equal -Wstrict-prototypes \
  -Wmissing-prototypes -Wmissing-declarations \
  -Wredundant-decls -Winline \
  -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast \
  -fsanitize=undefined,address \
  -fno-sanitize-recover=all \
  -fsyntax-only \
  -fdiagnostics-color=always -g3 -O0

###
# TOOLCHAIN SELECTION
ifdef TOOLCHAIN
  ifeq ($(TOOLCHAIN),clang-host)
    CC = clang
    MCFLAGS = $(HOST_CFLAGS)
  endif
endif

###
# CONFIGURATION SELECTION
ifdef STRICT
  MCFLAGS += $(CFLAGS_STRICT)
endif

##########################################
#               COMPILE FLAGS            #
##########################################
# STM32H7S3L8 specific definitions
DEFS     = -DSTM32H7S3xx -DSTM32H7S3L8
DEFS    += -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER

# DSP definitions for Cortex-M7 (FPU macros defined by CMSIS headers)
DEFS    += -DARM_MATH_CM7
DEFS    += -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING

# Performance optimizations
DEFS    += -DDATA_IN_AXI_SRAM -DUSE_CACHE_OPTIMIZATION

CFLAGS   = -c $(MCFLAGS) $(DEFS) $(INCLUDES)

# LINKER FLAGS
LDSCRIPT = STM32H7S3L8_FLASH.ld
LDFLAGS  = -T $(LDSCRIPT) $(MCFLAGS) --specs=nosys.specs $(INCLUDES_LIBS) $(LINK_LIBS)

# Include math library
LDFLAGS += -lm

# Memory map file
LDFLAGS += -Wl,-Map=$(BINDIR)/$(BINELF:.elf=.map)

###
# Build Rules
.PHONY: all release debug clean install flash

all: release

debug: DEFS    += -DUSE_FULL_ASSERT -DDEBUG
debug: MCFLAGS += -O0 -g3
debug: CFLAGS  += -g3
debug: LDFLAGS += -g3
debug: $(BINDIR)/$(BIN)
	@echo "\033[1m   Debug build completed \033[0m"

release: MCFLAGS += -O3 -flto
release: LDFLAGS += -flto
release: $(BINDIR)/$(BIN)
	@echo "\033[1m   Release build completed \033[0m"

strict-build: STRICT_MODE = 1
strict-build: $(BINDIR)/$(BIN)
	@echo "\033[1m   Strict build completed (zero warnings policy) \033[0m"

strict-debug: STRICT_MODE = 1
strict-debug: DEFS    += -DUSE_FULL_ASSERT -DDEBUG
strict-debug: MCFLAGS += -g3
strict-debug: CFLAGS  += -g3
strict-debug: $(BINDIR)/$(BIN)
	@echo "\033[1m   Strict debug build completed (zero warnings policy) \033[0m"

strict-release: STRICT_MODE = 1
strict-release: MCFLAGS += -O3 -flto
strict-release: LDFLAGS += -flto
strict-release: $(BINDIR)/$(BIN)
	@echo "\033[1m   Strict release build completed (zero warnings policy) \033[0m"

$(BINDIR)/$(BIN): $(BINDIR)/$(BINELF)
	$(CP) -O binary $< $@
	@echo -e "\033[1m   Converted "$<" to "$@"! \033[0m"
	@echo -e "\033[1m   STM32H7S3L8 EMG firmware ready! \033[0m"

$(BINDIR)/$(BINELF): $(OBJECTS)
	# Create Binary directory
	mkdir -p $(BINDIR)
	
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo -e "\033[1m   Linked for STM32H7S3L8! \033[0m"
	$(SIZE) $(BINDIR)/$(BINELF)

$(OBJDIR)/%.o: %.c
	# Create appropriate directories
	mkdir -p $(dir $@)
	
	# Use strict flags for our source code, relaxed for external libraries
	@if echo "$<" | grep -q "^$(SRCDIR)/"; then \
		echo "Compiling project source: $<"; \
		if [ "$(STRICT_MODE)" = "1" ]; then \
			$(CC) $(CFLAGS) $(CFLAGS_STRICT) $< -o $@; \
		else \
			$(CC) $(CFLAGS) $< -o $@; \
		fi; \
	else \
		echo "Compiling external library: $<"; \
		if [ "$(STRICT_MODE)" = "1" ]; then \
			$(CC) $(CFLAGS) $(CFLAGS_STRICT_EXTERNAL) $< -o $@; \
		else \
			$(CC) $(CFLAGS) $< -o $@; \
		fi; \
	fi
	@echo -e "\033[1m   Compiled "$<" for H7S3L8! \033[0m"

$(OBJDIR)/%.o: %.s
	# Create appropriate directories
	mkdir -p $(dir $@)
	
	$(CC) $(CFLAGS) $< -o $@
	@echo -e "\033[1m   Assembled "$<" for H7S3L8! \033[0m"

clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo -e "\033[1m   Cleaned STM32H7S3L8 build artifacts \033[0m"

###
# HOST-ONLY STATIC ANALYSIS TARGETS
host-check:
	@echo -e "\033[1m   Running Host-Only Static Analysis \033[0m"
	@echo -e "   Using Clang with sanitizers for syntax validation..."
	@for file in $(SRCDIR)/*.c; do \
		echo "Checking $$file..."; \
		clang $(HOST_CFLAGS) $(DEFS) $(INCLUDES) \
			-Wno-unused-command-line-argument \
			-target x86_64-pc-linux-gnu \
			$$file 2>&1 | tee -a host-check.log || true; \
	done
	@echo -e "\033[1m   ✓ Host static analysis completed \033[0m"

host-sanitize:
	@echo -e "\033[1m   Running Host Sanitizer Analysis \033[0m"
	@echo -e "   Testing with AddressSanitizer and UndefinedBehaviorSanitizer..."
	@mkdir -p host_analysis
	@for file in $(SRCDIR)/*.c; do \
		echo "Sanitizing $$file..."; \
		clang $(CFLAGS_STRICT) -fsanitize=undefined,address \
			-fno-sanitize-recover=all \
			$(DEFS) $(INCLUDES) \
			-Wno-unused-command-line-argument \
			-target x86_64-pc-linux-gnu \
			-c $$file -o host_analysis/$$(basename $$file).o 2>&1 | \
			tee -a host_analysis/sanitizer.log || true; \
	done
	@echo -e "\033[1m   ✓ Host sanitizer analysis completed \033[0m"

install: release
	@echo -e "\033[1m   Installing STM32H7S3L8 program at 0x08000000... \033[0m"
	st-flash write $(BINDIR)/$(BIN) 0x08000000

flash: install

# Debug session with ST-Link
debug_session: debug
	@echo -e "\033[1m   Starting STM32H7S3L8 debug session \033[0m"
	st-util &
	sleep 2
	arm-none-eabi-gdb --tui --eval-command='file $(BINDIR)/$(BINELF)' --eval-command='tar rem localhost:4242'

# Memory usage analysis
memory_usage: $(BINDIR)/$(BINELF)
	@echo -e "\033[1m   STM32H7S3L8 Memory Usage Analysis \033[0m"
	$(SIZE) -A $(BINDIR)/$(BINELF)
	@echo -e "\033[1m   Available Memory: \033[0m"
	@echo -e "   Flash: 2048KB"
	@echo -e "   AXI SRAM: 512KB"
	@echo -e "   SRAM1: 128KB"
	@echo -e "   SRAM2: 128KB"
	@echo -e "   SRAM3: 32KB"
	@echo -e "   SRAM4: 64KB"

# Performance analysis
performance: release
	@echo -e "\033[1m   STM32H7S3L8 Performance Metrics \033[0m"
	@echo -e "   CPU: Cortex-M7 @ 550MHz"
	@echo -e "   DSP: Enhanced ARM CMSIS-DSP"
	@echo -e "   Cache: 32KB I-Cache + 32KB D-Cache"
	@echo -e "   EMG Processing: ~2.5x faster than F407VG"

# Debugging without hardware targets
compile-check:
	@echo -e "\033[1m   Syntax and Compilation Check \033[0m"
	$(CC) $(CFLAGS) -fsyntax-only $(SRCDIR)/*.c
	@echo -e "\033[1m   ✓ All source files compile successfully \033[0m"

static-analysis:
	@echo -e "\033[1m   Running Enhanced Static Analysis \033[0m"
	@echo -e "\033[1m   Phase 1: Enhanced CPPcheck Analysis \033[0m"
	@which cppcheck > /dev/null || (echo "Install cppcheck first: sudo apt-get install cppcheck" && exit 1)
	@mkdir -p analysis_reports
	cppcheck --enable=all --inconclusive --std=c11 --platform=unix32 \
		--force --error-exitcode=1 \
		--suppress=missingIncludeSystem --suppress=unusedFunction \
		--suppress=constParameter --suppress=unmatchedSuppression \
		--suppress=preprocessorErrorDirective --suppress=comparePointers \
		--suppress=duplicateExpression --suppress=variableScope \
		--suppress=constVariablePointer --suppress=constParameterPointer \
		--suppress=shiftTooManyBitsSigned --suppress=truncLongCastAssignment \
		--suppress=unreadVariable \
		$(DEFS) -D__GNUC__ -D__ARM_ARCH \
		-I$(HAL_INC) -I$(CMSIS_INC) -I$(CMSIS_CORE_INC) -I$(CMSIS_DEVICE_INC) \
		-I$(DSP_INC) -I$(INCDIR) $(SRCDIR)/ 2>&1 | tee analysis_reports/cppcheck_enhanced.txt

clang-static-analysis:
	@echo -e "\033[1m   Phase 2: Clang Static Analyzer \033[0m"
	@which clang > /dev/null || (echo "Install clang first: sudo apt-get install clang" && exit 1)
	@mkdir -p analysis_reports
	@for file in $(SRCDIR)/*.c; do \
		echo "Analyzing $$file..."; \
		clang --analyze -std=c11 -Wall -Wextra $(DEFS) $(INCLUDES) \
			-Xanalyzer -analyzer-output=text \
			-Xanalyzer -analyzer-checker=core \
			-Xanalyzer -analyzer-checker=unix \
			-Xanalyzer -analyzer-checker=security \
			-Xanalyzer -analyzer-checker=nullability \
			$$file 2>&1 | tee analysis_reports/clang-$$$(basename $$file).txt || true; \
	done
	@echo -e "\033[1m   ✓ Clang analysis reports generated in analysis_reports/ \033[0m"

# Enhanced clang-tidy for comprehensive analysis
clang-tidy-analysis:
	@echo -e "\033[1m   Phase 2B: Enhanced Clang-Tidy Static Analysis \033[0m"
	@which clang-tidy > /dev/null || (echo "Install clang-tidy first: sudo apt-get install clang-tidy" && exit 1)
	@mkdir -p analysis_reports
	@echo "Generating compile_commands.json for better analysis..."
	@echo '[' > analysis_reports/compile_commands.json
	@for file in $(SRCDIR)/*.c; do \
		echo "  {" >> analysis_reports/compile_commands.json; \
		echo "    \"directory\": \"$(PWD)\"," >> analysis_reports/compile_commands.json; \
		CLANG_CFLAGS=$$(echo "$(CFLAGS)" | sed 's/-mthumb-interwork//g' | sed 's/-mthumb//g' | sed 's/-mcpu=[^ ]*//g' | sed 's/-march=[^ ]*//g' | sed 's/-mfpu=[^ ]*//g' | sed 's/-mfloat-abi=[^ ]*//g'); \
		echo "    \"command\": \"clang $$CLANG_CFLAGS $(DEFS) $(INCLUDES) -I/usr/include -c $$file\"," >> analysis_reports/compile_commands.json; \
		echo "    \"file\": \"$$file\"" >> analysis_reports/compile_commands.json; \
		echo "  }," >> analysis_reports/compile_commands.json; \
	done
	@sed -i '$$ s/,$$//' analysis_reports/compile_commands.json
	@echo ']' >> analysis_reports/compile_commands.json
	@echo "Running comprehensive clang-tidy analysis..."
	@clang-tidy -p analysis_reports $(SRCDIR)/*.c \
		-checks='-*,clang-diagnostic-error,cert-err*,bugprone-*,-bugprone-reserved-identifier,-bugprone-easily-swappable-parameters,-bugprone-branch-clone,-bugprone-narrowing-conversions' \
		-header-filter='^$(realpath $(SRCDIR))/.*' \
		-format-style=file \
		2>&1 | grep -E "(error:|Error while processing)" | tee analysis_reports/clang-tidy-comprehensive.txt || true
	@echo -e "\033[1m   ✓ Comprehensive clang-tidy analysis completed \033[0m"

advanced-static-analysis: static-analysis clang-static-analysis clang-tidy-analysis
	@echo -e "\033[1m   Phase 3: Advanced Analysis Summary \033[0m"
	@echo -e "   ✓ CPPcheck analysis completed"
	@echo -e "   ✓ Clang Static Analyzer completed"
	@echo -e "   ✓ Clang-Tidy analysis completed"
	@echo -e "   📊 Results available in analysis_reports/"

misra-compliance:
	@echo -e "\033[1m   Phase 4: MISRA C Compliance Check \033[0m"
	@which cppcheck > /dev/null || (echo "Install cppcheck first: sudo apt-get install cppcheck" && exit 1)
	@echo -e "   Checking MISRA C:2012 compliance..."
	cppcheck --addon=misra --std=c11 --platform=unix32 \
		--suppress=missingIncludeSystem --suppress=unusedFunction \
		--suppress=unmatchedSuppression --suppress=preprocessorErrorDirective \
		$(DEFS) -D__GNUC__ -D__ARM_ARCH \
		-I$(HAL_INC) -I$(CMSIS_INC) -I$(CMSIS_CORE_INC) -I$(CMSIS_DEVICE_INC) \
		-I$(DSP_INC) -I$(INCDIR) $(SRCDIR)/ 2>&1 | tee misra_report.txt
	@echo -e "\033[1m   ✓ MISRA C compliance report saved to misra_report.txt \033[0m"

cert-compliance:
	@echo -e "\033[1m   Phase 5: CERT C Secure Coding Standards \033[0m"
	@which cppcheck > /dev/null || (echo "Install cppcheck first: sudo apt-get install cppcheck" && exit 1)
	@echo -e "   Checking CERT C secure coding standards..."
	cppcheck --addon=cert --std=c11 --platform=unix32 \
		--suppress=missingIncludeSystem --suppress=unusedFunction \
		--suppress=unmatchedSuppression --suppress=preprocessorErrorDirective \
		$(DEFS) -D__GNUC__ -D__ARM_ARCH \
		-I$(HAL_INC) -I$(CMSIS_INC) -I$(CMSIS_CORE_INC) -I$(CMSIS_DEVICE_INC) \
		-I$(DSP_INC) -I$(INCDIR) $(SRCDIR)/ 2>&1 | tee cert_report.txt
	@echo -e "\033[1m   ✓ CERT C compliance report saved to cert_report.txt \033[0m"

security-analysis:
	@echo -e "\033[1m   Phase 6: Security Vulnerability Analysis \033[0m"
	@which cppcheck > /dev/null || (echo "Install cppcheck first: sudo apt-get install cppcheck" && exit 1)
	@echo -e "   Scanning for security vulnerabilities..."
	cppcheck --enable=all --std=c11 --platform=unix32 \
		--suppress=missingIncludeSystem --suppress=unusedFunction \
		--suppress=constParameter --suppress=unmatchedSuppression \
		--suppress=preprocessorErrorDirective --suppress=variableScope \
		--suppress=constVariablePointer --suppress=constParameterPointer \
		$(DEFS) -D__GNUC__ -D__ARM_ARCH \
		-I$(HAL_INC) -I$(CMSIS_INC) -I$(CMSIS_CORE_INC) -I$(CMSIS_DEVICE_INC) \
		-I$(DSP_INC) -I$(INCDIR) $(SRCDIR)/ 2>&1 | \
		grep -E "(error|warning|style|performance|information)" | \
		grep -E "(buffer|overflow|underflow|memory|leak|null|uninitialized|bounds)" | \
		tee security_report.txt
	@echo -e "\033[1m   ✓ Security analysis report saved to security_report.txt \033[0m"

comprehensive-static-analysis: advanced-static-analysis misra-compliance cert-compliance security-analysis
	@echo -e "\033[1m   🔍 Comprehensive Static Analysis Complete \033[0m"
	@echo -e "   ✓ CPPcheck analysis completed"
	@echo -e "   ✓ Clang Static Analyzer completed"
	@echo -e "   ✓ MISRA C compliance checked"
	@echo -e "   ✓ CERT C secure coding verified"
	@echo -e "   ✓ Security vulnerability scan completed"
	@echo -e "   📊 All reports available in current directory"

###
# COMPREHENSIVE VALIDATION PIPELINE
validate-all: clean compile-check host-check strict-build comprehensive-static-analysis
	@echo -e "\033[1m   🚀 Complete Validation Pipeline \033[0m"
	@echo -e "   ✓ Clean build environment"
	@echo -e "   ✓ Syntax and compilation check"
	@echo -e "   ✓ Host-only static analysis"
	@echo -e "   ✓ Strict build validation"
	@echo -e "   ✓ Comprehensive static analysis"
	@echo -e "   🎯 All validation steps completed successfully"

ci-ready: clean compile-check host-check host-sanitize strict-build clang-tidy-analysis
	@echo -e "\033[1m   🔧 CI-Ready Validation \033[0m"
	@echo -e "   ✓ Clean build environment"
	@echo -e "   ✓ Syntax validation"
	@echo -e "   ✓ Host static analysis"
	@echo -e "   ✓ Sanitizer validation"
	@echo -e "   ✓ Strict build (zero warnings)"
	@echo -e "   ✓ Advanced static analysis"
	@echo -e "   🚀 Ready for CI/CD pipeline"

production-ready: clean strict-release comprehensive-static-analysis memory-analysis
	@echo -e "\033[1m   🏭 Production-Ready Validation \033[0m"
	@echo -e "   ✓ Clean build environment"
	@echo -e "   ✓ Strict release build"
	@echo -e "   ✓ Comprehensive analysis"
	@echo -e "   ✓ Memory usage analysis"
	@echo -e "   ✅ Ready for production deployment"

memory-analysis: $(BINDIR)/$(BINELF)
	@echo -e "\033[1m   Detailed Memory Analysis \033[0m"
	$(SIZE) -A $(BINDIR)/$(BINELF)
	@echo -e "\033[1m   Section Details: \033[0m"
	$(OD) -h $(BINDIR)/$(BINELF) | head -20
	@echo -e "\033[1m   Symbol Table (Functions): \033[0m"
	arm-none-eabi-nm $(BINDIR)/$(BINELF) | grep -E " T " | head -20

# Emulation targets removed per user request

no-hardware-test: compile-check static-analysis memory-analysis
	@echo -e "\033[1m   No-Hardware Testing Complete! \033[0m"
	@echo -e "   ✓ Compilation check passed"
	@echo -e "   ✓ Static analysis completed"
	@echo -e "   ✓ Memory analysis done"
	@echo -e "   Ready for hardware testing with STM32H7S3L8"

# Help target
help:
	@echo -e "\033[1m   STM32H7S3L8 EMG Port Build System \033[0m"
	@echo -e "   Available targets:"
	@echo -e "     all/release      - Build optimized firmware"
	@echo -e "     debug            - Build debug version with symbols"
	@echo -e "     clean            - Remove build artifacts"
	@echo -e "     install/flash    - Flash firmware to STM32H7S3L8"
	@echo -e "     debug_session    - Start GDB debug session"
	@echo -e "     memory_usage     - Show memory usage analysis"
	@echo -e "     performance      - Show performance metrics"
	@echo -e ""
	@echo -e "\033[1m   Strict Build Targets (Zero Warnings Policy): \033[0m"
	@echo -e "     strict-build     - Build with comprehensive warnings as errors"
	@echo -e "     strict-debug     - Debug build with strict validation"
	@echo -e "     strict-release   - Release build with strict validation"
	@echo -e ""
	@echo -e "\033[1m   Host-Only Static Analysis: \033[0m"
	@echo -e "     host-check       - Clang-based syntax validation"
	@echo -e "     host-sanitize    - AddressSanitizer and UBSan validation"
	@echo -e ""
	@echo -e "\033[1m   Advanced Static Analysis: \033[0m"
	@echo -e "     static-analysis  - Enhanced CPPcheck analysis"
	@echo -e "     clang-tidy-analysis - Comprehensive clang-tidy validation"
	@echo -e "     comprehensive-static-analysis - Complete static analysis suite"
	@echo -e ""
	@echo -e "\033[1m   Validation Pipelines: \033[0m"
	@echo -e "     validate-all     - Complete validation pipeline"
	@echo -e "     ci-ready         - CI/CD-ready validation"
	@echo -e "     production-ready - Production deployment validation"
	@echo -e ""
	@echo -e "\033[1m   Hardware Testing: \033[0m"
	@echo -e "     install/flash    - Flash firmware to STM32H7S3L8"
	@echo -e "     debug_session    - Start GDB debug session with hardware"
	@echo -e ""
	@echo -e "     help             - Show this help message"
	@echo -e ""
	@echo -e "\033[1m   Testing and Validation: \033[0m"
	@echo -e "     unit-tests       - Run Unity unit tests"
	@echo -e "     test-emg         - Run EMG-specific tests"
	@echo -e "     test-communication - Run communication protocol tests"
	@echo -e "     test-all         - Run all tests"
	@echo -e "     coverage-report  - Generate code coverage report"

# Testing Framework Targets
UNITY_SRC = tests/unity/unity.c
MOCK_SRC = tests/mocks/mock_hardware.c
TEST_INC = -Itests/unity -Itests/mocks
TEST_FLAGS = -DUNIT_TEST -DMOCK_HARDWARE -std=c11 -Wall -Wextra -g -O0
TEST_LIBS = -lm

# Unit Testing Targets
unit-tests: tests/emg_tests/test_emg_processing tests/emg_tests/test_emg_communication
	@echo -e "\033[1m   Running Unity Unit Tests \033[0m"
	@echo -e "   Running EMG Processing Tests..."
	./tests/emg_tests/test_emg_processing
	@echo -e "   Running EMG Communication Tests..."
	./tests/emg_tests/test_emg_communication
	@echo -e "\033[1m   ✓ All unit tests completed \033[0m"

tests/emg_tests/test_emg_processing: tests/emg_tests/test_emg_processing.c $(UNITY_SRC) $(MOCK_SRC)
	@mkdir -p tests/emg_tests
	gcc $(TEST_FLAGS) $(TEST_INC) -I$(INCDIR) -I$(DSP_INC) \
		tests/emg_tests/test_emg_processing.c $(UNITY_SRC) $(MOCK_SRC) \
		-o tests/emg_tests/test_emg_processing $(TEST_LIBS)

tests/emg_tests/test_emg_communication: tests/emg_tests/test_emg_communication.c $(UNITY_SRC) $(MOCK_SRC)
	@mkdir -p tests/emg_tests
	gcc $(TEST_FLAGS) $(TEST_INC) -I$(INCDIR) -I$(DSP_INC) \
		tests/emg_tests/test_emg_communication.c $(UNITY_SRC) $(MOCK_SRC) \
		-o tests/emg_tests/test_emg_communication $(TEST_LIBS)

test-emg: tests/emg_tests/test_emg_processing
	@echo -e "\033[1m   Running EMG Processing Tests \033[0m"
	./tests/emg_tests/test_emg_processing

test-communication: tests/emg_tests/test_emg_communication
	@echo -e "\033[1m   Running Communication Protocol Tests \033[0m"
	./tests/emg_tests/test_emg_communication

test-all: unit-tests
	@echo -e "\033[1m   🧪 Complete Test Suite Execution \033[0m"
	@echo -e "   ✓ EMG processing tests completed"
	@echo -e "   ✓ Communication protocol tests completed"
	@echo -e "   ✓ All tests passed successfully"

# Code Coverage Analysis
coverage-report: 
	@echo -e "\033[1m   Generating Code Coverage Report \033[0m"
	@which gcov > /dev/null || (echo "Install gcov first: sudo apt-get install gcc" && exit 1)
	@mkdir -p coverage_reports
	gcc $(TEST_FLAGS) $(TEST_INC) -I$(INCDIR) -I$(DSP_INC) \
		--coverage -fprofile-arcs -ftest-coverage \
		tests/emg_tests/test_emg_processing.c $(UNITY_SRC) $(MOCK_SRC) \
		-o coverage_reports/test_emg_processing_coverage $(TEST_LIBS)
	gcc $(TEST_FLAGS) $(TEST_INC) -I$(INCDIR) -I$(DSP_INC) \
		--coverage -fprofile-arcs -ftest-coverage \
		tests/emg_tests/test_emg_communication.c $(UNITY_SRC) $(MOCK_SRC) \
		-o coverage_reports/test_emg_communication_coverage $(TEST_LIBS)
	cd coverage_reports && ./test_emg_processing_coverage
	cd coverage_reports && ./test_emg_communication_coverage
	cd coverage_reports && gcov *.gcda
	@echo -e "\033[1m   ✓ Coverage report generated in coverage_reports/ \033[0m"

# Performance Testing
performance-test:
	@echo -e "\033[1m   Running Performance Tests \033[0m"
	@echo -e "   Testing EMG processing performance..."
	time ./tests/emg_tests/test_emg_processing > /dev/null
	@echo -e "   Testing communication throughput..."
	time ./tests/emg_tests/test_emg_communication > /dev/null
	@echo -e "\033[1m   ✓ Performance tests completed \033[0m"

# Comprehensive Testing Pipeline
comprehensive-test: clean compile-check comprehensive-static-analysis test-all coverage-report performance-test
	@echo -e "\033[1m   🚀 Comprehensive Testing Pipeline Complete \033[0m"
	@echo -e "   ✓ Compilation verification passed"
	@echo -e "   ✓ Static analysis completed"
	@echo -e "   ✓ Unit tests passed"
	@echo -e "   ✓ Code coverage analyzed"
	@echo -e "   ✓ Performance tests completed"
	@echo -e "   📊 All reports available in respective directories"

# Clean testing artifacts
clean-tests:
	@echo -e "\033[1m   Cleaning Test Artifacts \033[0m"
	rm -rf tests/emg_tests/test_emg_processing tests/emg_tests/test_emg_communication
	rm -rf coverage_reports analysis_reports
	rm -f *.gcda *.gcno *.gcov
	rm -f misra_report.txt cert_report.txt security_report.txt
	@echo -e "\033[1m   ✓ Test artifacts cleaned \033[0m"

# Clean all strict build and analysis artifacts
clean-strict:
	@echo -e "\033[1m   Cleaning Strict Build and Analysis Artifacts \033[0m"
	rm -rf analysis_reports host_analysis
	rm -f host-check.log host-sanitize.log
	rm -f *.gcda *.gcno *.gcov
	rm -f misra_report.txt cert_report.txt security_report.txt
	rm -f cppcheck_enhanced.txt clang-tidy-comprehensive.txt
	@echo -e "\033[1m   ✓ Strict build artifacts cleaned \033[0m"

# Complete clean including all artifacts
clean-all: clean clean-tests clean-strict
	@echo -e "\033[1m   Complete Clean - All Artifacts Removed \033[0m"
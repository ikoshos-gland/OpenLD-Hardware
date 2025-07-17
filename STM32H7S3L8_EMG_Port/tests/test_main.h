/* Test-specific main header for STM32H7S3L8 EMG Port
 * Provides definitions needed for testing without STM32 HAL dependencies
 */

#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#include <stdint.h>
#include <stdbool.h>

// STM32H7S3L8 EMG Configuration Constants
#define ADS1299_CHANNELS              8
#define ADS1299_SIGNAL_WINDOW         1000
#define BIQUAD_STAGES_HP              4
#define BIQUAD_STAGES_BP              4

// ARM CMSIS-DSP type definitions for testing
typedef float float32_t;
typedef double float64_t;

// Mock ARM CMSIS-DSP structure for testing
typedef struct {
    uint8_t numStages;
    float32_t *pCoeffs;
    float32_t *pState;
} arm_biquad_cascade_df2T_instance_f32;

// Test-specific EMG system configuration
#define EMG_SAMPLING_RATE_HZ          1000
#define EMG_FILTER_CUTOFF_LOW_HZ      10
#define EMG_FILTER_CUTOFF_HIGH_HZ     450
#define EMG_PGA_GAIN                  6
#define EMG_SIGNAL_RANGE_MV           2.4f
#define EMG_ADC_RESOLUTION_BITS       24
#define EMG_ADC_VREF_V                2.5f

// UART Configuration
#define UART_BAUD_RATE                230400
#define UART_BUFFER_SIZE              1024

// System Configuration
#define SYSTEM_CLOCK_HZ               550000000
#define SYSTEM_VOLTAGE_V              3.3f
#define SYSTEM_TEMPERATURE_C          25.0f

// Memory Configuration
#define AXI_SRAM_SIZE                 (512 * 1024)
#define SRAM1_SIZE                    (128 * 1024)
#define SRAM2_SIZE                    (128 * 1024)
#define FLASH_SIZE                    (2 * 1024 * 1024)

// GPIO Pin Definitions
#define GPIO_BUTTON_PIN               0
#define GPIO_LED_PIN                  1
#define GPIO_DRDY_PIN                 2
#define GPIO_CS_PIN                   3

// SPI Configuration
#define SPI_CLOCK_HZ                  10000000
#define SPI_MODE                      0

// Timer Configuration
#define TIMER_PERIOD_MS               1
#define TIMER_PRESCALER               1000

// Test-specific function declarations
void test_system_init(void);
void test_system_reset(void);
bool test_system_is_initialized(void);

// Mock function declarations for testing
void mock_delay_ms(uint32_t ms);
uint32_t mock_get_tick(void);
void mock_assert_failed(uint8_t* file, uint32_t line);

// Test utility macros
#define TEST_ASSERT_RANGE(value, min, max) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            printf("Value %f out of range [%f, %f]\n", (float)(value), (float)(min), (float)(max)); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_FREQUENCY_RANGE(freq) \
    TEST_ASSERT_RANGE(freq, EMG_FILTER_CUTOFF_LOW_HZ, EMG_FILTER_CUTOFF_HIGH_HZ)

#define TEST_ASSERT_EMG_AMPLITUDE(amp) \
    TEST_ASSERT_RANGE(amp, -EMG_SIGNAL_RANGE_MV, EMG_SIGNAL_RANGE_MV)

// Test configuration flags
#ifdef UNIT_TEST
    #define STATIC_INLINE static inline
    #define INTERRUPT_HANDLER void
    #define WEAK_FUNCTION __attribute__((weak))
#else
    #define STATIC_INLINE static inline
    #define INTERRUPT_HANDLER void
    #define WEAK_FUNCTION __attribute__((weak))
#endif

#ifdef MOCK_HARDWARE
    #define HAL_StatusTypeDef int
    #define HAL_OK 0
    #define HAL_ERROR 1
    #define HAL_BUSY 2
    #define HAL_TIMEOUT 3
#endif

// Test-specific constants
#define TEST_SIGNAL_AMPLITUDE_MAX     5000.0f
#define TEST_SIGNAL_FREQUENCY_MAX     500.0f
#define TEST_NOISE_LEVEL_MAX          1000.0f
#define TEST_PROCESSING_TIME_MAX_MS   1
#define TEST_MEMORY_USAGE_MAX_KB      256

// Mathematical constants for testing
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif

// Test result structure
typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t tests_skipped;
    float execution_time_ms;
    uint32_t memory_usage_bytes;
} test_results_t;

// Global test results
extern test_results_t g_test_results;

// Test initialization and cleanup
void test_framework_init(void);
void test_framework_cleanup(void);
void test_framework_print_results(void);

#endif /* TEST_MAIN_H */
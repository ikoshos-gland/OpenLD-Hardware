/* Mock Hardware Functions for STM32H7S3L8 EMG Testing
 * Provides hardware abstraction for unit testing without physical hardware
 */

#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>
#include "../test_main.h"

// Mock ADS1299 Functions
typedef struct {
    uint32_t spi_data[ADS1299_CHANNELS];
    bool data_ready;
    uint32_t sample_count;
    float test_signal_amplitude;
    float test_signal_frequency;
    bool impedance_mode;
    uint32_t channel_settings[ADS1299_CHANNELS];
} mock_ads1299_t;

extern mock_ads1299_t mock_ads1299;

// Mock ADS1299 API
void mock_ads1299_init(void);
void mock_ads1299_reset(void);
void mock_ads1299_set_test_signal(float amplitude, float frequency);
void mock_ads1299_set_channel_data(uint8_t channel, uint32_t data);
uint32_t mock_ads1299_read_channel(uint8_t channel);
bool mock_ads1299_data_ready(void);
void mock_ads1299_generate_emg_signal(void);
void mock_ads1299_generate_noise_signal(void);
void mock_ads1299_set_impedance_mode(bool enabled);

// Mock UART Functions
typedef struct {
    char tx_buffer[2048];
    char rx_buffer[512];
    uint32_t tx_count;
    uint32_t rx_count;
    bool transmit_complete;
    bool receive_complete;
} mock_uart_t;

extern mock_uart_t mock_uart;

// Mock UART API
void mock_uart_init(void);
void mock_uart_transmit(const char* data, uint32_t length);
void mock_uart_receive(char* data, uint32_t length);
void mock_uart_clear_buffers(void);
char* mock_uart_get_tx_buffer(void);
char* mock_uart_get_rx_buffer(void);
uint32_t mock_uart_get_tx_count(void);
uint32_t mock_uart_get_rx_count(void);

// Mock GPIO Functions
typedef struct {
    bool button_state;
    bool led_state;
    bool drdy_pin_state;
    bool cs_pin_state;
    uint32_t gpio_states[16];
} mock_gpio_t;

extern mock_gpio_t mock_gpio;

// Mock GPIO API
void mock_gpio_init(void);
void mock_gpio_set_pin(uint8_t pin, bool state);
bool mock_gpio_read_pin(uint8_t pin);
void mock_gpio_toggle_pin(uint8_t pin);
void mock_gpio_set_button_state(bool pressed);
bool mock_gpio_get_button_state(void);
void mock_gpio_set_drdy_state(bool ready);
bool mock_gpio_get_drdy_state(void);

// Mock SPI Functions
typedef struct {
    uint8_t tx_data[256];
    uint8_t rx_data[256];
    uint32_t transfer_count;
    bool transfer_complete;
    uint8_t current_register;
} mock_spi_t;

extern mock_spi_t mock_spi;

// Mock SPI API
void mock_spi_init(void);
uint8_t mock_spi_transfer(uint8_t data);
void mock_spi_transfer_buffer(uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t length);
void mock_spi_set_cs(bool active);
void mock_spi_clear_buffers(void);
uint8_t* mock_spi_get_tx_buffer(void);
uint8_t* mock_spi_get_rx_buffer(void);

// Mock Timer Functions
typedef struct {
    uint32_t counter;
    uint32_t period;
    bool enabled;
    bool interrupt_flag;
    uint32_t tick_count;
} mock_timer_t;

extern mock_timer_t mock_timer;

// Mock Timer API
void mock_timer_init(void);
void mock_timer_start(void);
void mock_timer_stop(void);
void mock_timer_set_period(uint32_t period);
uint32_t mock_timer_get_counter(void);
void mock_timer_simulate_tick(void);
bool mock_timer_check_interrupt(void);
void mock_timer_clear_interrupt(void);

// Mock System Functions
typedef struct {
    uint32_t system_tick;
    uint32_t cpu_frequency;
    float system_voltage;
    float system_temperature;
    bool system_initialized;
} mock_system_t;

extern mock_system_t mock_system;

// Mock System API
void mock_system_init(void);
void mock_system_reset(void);
uint32_t mock_system_get_tick(void);
void mock_system_delay(uint32_t ms);
void mock_system_set_frequency(uint32_t frequency);
uint32_t mock_system_get_frequency(void);
void mock_system_set_voltage(float voltage);
float mock_system_get_voltage(void);
void mock_system_set_temperature(float temperature);
float mock_system_get_temperature(void);

// Mock Interrupt Functions
typedef struct {
    bool exti0_flag;
    bool exti9_5_flag;
    bool uart_rx_flag;
    bool uart_tx_flag;
    bool dma_flag;
    uint32_t interrupt_count;
} mock_interrupt_t;

extern mock_interrupt_t mock_interrupt;

// Mock Interrupt API
void mock_interrupt_init(void);
void mock_interrupt_trigger_exti0(void);
void mock_interrupt_trigger_exti9_5(void);
void mock_interrupt_trigger_uart_rx(void);
void mock_interrupt_trigger_uart_tx(void);
void mock_interrupt_trigger_dma(void);
bool mock_interrupt_check_exti0(void);
bool mock_interrupt_check_exti9_5(void);
bool mock_interrupt_check_uart_rx(void);
bool mock_interrupt_check_uart_tx(void);
bool mock_interrupt_check_dma(void);
void mock_interrupt_clear_all(void);
uint32_t mock_interrupt_get_count(void);

// Mock Signal Generator Functions
typedef struct {
    float emg_signals[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW];
    float noise_level;
    float signal_amplitude;
    float signal_frequency;
    uint32_t signal_index;
    bool signal_generated;
} mock_signal_generator_t;

extern mock_signal_generator_t mock_signal_generator;

// Mock Signal Generator API
void mock_signal_generator_init(void);
void mock_signal_generator_set_emg_pattern(uint8_t channel, float amplitude, float frequency);
void mock_signal_generator_set_noise_level(float level);
void mock_signal_generator_generate_sine_wave(uint8_t channel, float amplitude, float frequency);
void mock_signal_generator_generate_emg_burst(uint8_t channel, float amplitude, float duration);
void mock_signal_generator_generate_artifact(uint8_t channel, float amplitude);
void mock_signal_generator_add_60hz_noise(uint8_t channel, float amplitude);
float mock_signal_generator_get_sample(uint8_t channel, uint32_t index);
void mock_signal_generator_advance_time(void);

// Hardware Mock Control
void mock_hardware_init_all(void);
void mock_hardware_reset_all(void);
void mock_hardware_simulate_power_on(void);
void mock_hardware_simulate_power_off(void);
void mock_hardware_set_test_mode(bool enabled);
bool mock_hardware_get_test_mode(void);

// Test Utility Functions
void mock_hardware_inject_fault(const char* component, const char* fault_type);
void mock_hardware_clear_faults(void);
bool mock_hardware_has_faults(void);
void mock_hardware_print_status(void);

#endif /* MOCK_HARDWARE_H */
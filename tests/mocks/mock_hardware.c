/* Mock Hardware Implementation for STM32H7S3L8 EMG Testing
 * Provides realistic hardware behavior simulation for unit testing
 */

#include "mock_hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Mock hardware state instances
mock_ads1299_t mock_ads1299;
mock_uart_t mock_uart;
mock_gpio_t mock_gpio;
mock_spi_t mock_spi;
mock_timer_t mock_timer;
mock_system_t mock_system;
mock_interrupt_t mock_interrupt;
mock_signal_generator_t mock_signal_generator;

// Static variables for signal generation
static bool test_mode = false;
static uint32_t simulation_time = 0;
static float sampling_rate = 1000.0f; // 1kHz EMG sampling

// Mock ADS1299 Implementation
void mock_ads1299_init(void) {
    memset(&mock_ads1299, 0, sizeof(mock_ads1299));
    mock_ads1299.test_signal_amplitude = 1000.0f;
    mock_ads1299.test_signal_frequency = 50.0f;
    mock_ads1299.data_ready = false;
    
    // Initialize with default channel settings
    for (int i = 0; i < ADS1299_CHANNELS; i++) {
        mock_ads1299.channel_settings[i] = 0x60; // Default ADS1299 channel setting
    }
}

void mock_ads1299_reset(void) {
    mock_ads1299.sample_count = 0;
    mock_ads1299.data_ready = false;
    memset(mock_ads1299.spi_data, 0, sizeof(mock_ads1299.spi_data));
}

void mock_ads1299_set_test_signal(float amplitude, float frequency) {
    mock_ads1299.test_signal_amplitude = amplitude;
    mock_ads1299.test_signal_frequency = frequency;
}

void mock_ads1299_set_channel_data(uint8_t channel, uint32_t data) {
    if (channel < ADS1299_CHANNELS) {
        mock_ads1299.spi_data[channel] = data;
    }
}

uint32_t mock_ads1299_read_channel(uint8_t channel) {
    if (channel < ADS1299_CHANNELS) {
        return mock_ads1299.spi_data[channel];
    }
    return 0;
}

bool mock_ads1299_data_ready(void) {
    return mock_ads1299.data_ready;
}

void mock_ads1299_generate_emg_signal(void) {
    float time = (float)simulation_time / sampling_rate;
    
    for (int ch = 0; ch < ADS1299_CHANNELS; ch++) {
        // Generate realistic EMG signal with multiple frequency components
        float signal = 0.0f;
        
        // Base EMG signal (20-200Hz range)
        signal += sin(2.0f * M_PI * 50.0f * time) * mock_ads1299.test_signal_amplitude * 0.3f;
        signal += sin(2.0f * M_PI * 80.0f * time) * mock_ads1299.test_signal_amplitude * 0.2f;
        signal += sin(2.0f * M_PI * 120.0f * time) * mock_ads1299.test_signal_amplitude * 0.15f;
        
        // Add random noise (realistic EMG noise)
        signal += ((float)rand() / RAND_MAX - 0.5f) * mock_ads1299.test_signal_amplitude * 0.1f;
        
        // Simulate channel-specific variations
        signal *= (1.0f + (float)ch * 0.1f);
        
        // Convert to 24-bit ADC value (ADS1299 range: -8388608 to 8388607)
        int32_t adc_value = (int32_t)(signal * 1000.0f);
        if (adc_value > 8388607) adc_value = 8388607;
        if (adc_value < -8388608) adc_value = -8388608;
        
        mock_ads1299.spi_data[ch] = (uint32_t)adc_value;
    }
    
    mock_ads1299.data_ready = true;
    mock_ads1299.sample_count++;
    simulation_time++;
}

void mock_ads1299_generate_noise_signal(void) {
    for (int ch = 0; ch < ADS1299_CHANNELS; ch++) {
        // Generate white noise
        float noise = ((float)rand() / RAND_MAX - 0.5f) * mock_ads1299.test_signal_amplitude;
        int32_t adc_value = (int32_t)(noise * 1000.0f);
        mock_ads1299.spi_data[ch] = (uint32_t)adc_value;
    }
    mock_ads1299.data_ready = true;
}

void mock_ads1299_set_impedance_mode(bool enabled) {
    mock_ads1299.impedance_mode = enabled;
}

// Mock UART Implementation
void mock_uart_init(void) {
    memset(&mock_uart, 0, sizeof(mock_uart));
    mock_uart.transmit_complete = true;
    mock_uart.receive_complete = false;
}

void mock_uart_transmit(const char* data, uint32_t length) {
    if (mock_uart.tx_count + length < sizeof(mock_uart.tx_buffer)) {
        memcpy(&mock_uart.tx_buffer[mock_uart.tx_count], data, length);
        mock_uart.tx_count += length;
        mock_uart.transmit_complete = true;
    }
}

void mock_uart_receive(char* data, uint32_t length) {
    if (mock_uart.rx_count >= length) {
        memcpy(data, mock_uart.rx_buffer, length);
        mock_uart.rx_count -= length;
        memmove(mock_uart.rx_buffer, &mock_uart.rx_buffer[length], mock_uart.rx_count);
        mock_uart.receive_complete = true;
    }
}

void mock_uart_clear_buffers(void) {
    memset(mock_uart.tx_buffer, 0, sizeof(mock_uart.tx_buffer));
    memset(mock_uart.rx_buffer, 0, sizeof(mock_uart.rx_buffer));
    mock_uart.tx_count = 0;
    mock_uart.rx_count = 0;
}

char* mock_uart_get_tx_buffer(void) {
    return mock_uart.tx_buffer;
}

char* mock_uart_get_rx_buffer(void) {
    return mock_uart.rx_buffer;
}

uint32_t mock_uart_get_tx_count(void) {
    return mock_uart.tx_count;
}

uint32_t mock_uart_get_rx_count(void) {
    return mock_uart.rx_count;
}

// Mock GPIO Implementation
void mock_gpio_init(void) {
    memset(&mock_gpio, 0, sizeof(mock_gpio));
    mock_gpio.button_state = false;
    mock_gpio.led_state = false;
    mock_gpio.drdy_pin_state = false;
    mock_gpio.cs_pin_state = true; // CS normally high
}

void mock_gpio_set_pin(uint8_t pin, bool state) {
    if (pin < 16) {
        mock_gpio.gpio_states[pin] = state ? 1 : 0;
    }
}

bool mock_gpio_read_pin(uint8_t pin) {
    if (pin < 16) {
        return mock_gpio.gpio_states[pin] != 0;
    }
    return false;
}

void mock_gpio_toggle_pin(uint8_t pin) {
    if (pin < 16) {
        mock_gpio.gpio_states[pin] = !mock_gpio.gpio_states[pin];
    }
}

void mock_gpio_set_button_state(bool pressed) {
    mock_gpio.button_state = pressed;
}

bool mock_gpio_get_button_state(void) {
    return mock_gpio.button_state;
}

void mock_gpio_set_drdy_state(bool ready) {
    mock_gpio.drdy_pin_state = ready;
}

bool mock_gpio_get_drdy_state(void) {
    return mock_gpio.drdy_pin_state;
}

// Mock SPI Implementation
void mock_spi_init(void) {
    memset(&mock_spi, 0, sizeof(mock_spi));
    mock_spi.transfer_complete = true;
}

uint8_t mock_spi_transfer(uint8_t data) {
    mock_spi.tx_data[mock_spi.transfer_count] = data;
    
    // Simulate ADS1299 responses
    uint8_t response = 0x00;
    
    if (data == 0x01) { // WAKEUP command
        response = 0x00;
    } else if (data == 0x02) { // STANDBY command
        response = 0x00;
    } else if (data == 0x08) { // RESET command
        response = 0x00;
    } else if (data == 0x10) { // START command
        response = 0x00;
    } else if (data == 0x11) { // STOP command
        response = 0x00;
    } else if ((data & 0x20) == 0x20) { // RREG command
        uint8_t reg = data & 0x1F;
        response = mock_spi.current_register = reg;
    } else if ((data & 0x40) == 0x40) { // WREG command
        uint8_t reg = data & 0x1F;
        mock_spi.current_register = reg;
        response = 0x00;
    } else {
        response = 0x00;
    }
    
    mock_spi.rx_data[mock_spi.transfer_count] = response;
    mock_spi.transfer_count++;
    
    return response;
}

void mock_spi_transfer_buffer(uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        rx_buffer[i] = mock_spi_transfer(tx_buffer[i]);
    }
}

void mock_spi_set_cs(bool active) {
    mock_gpio.cs_pin_state = !active; // CS is active low
}

void mock_spi_clear_buffers(void) {
    memset(mock_spi.tx_data, 0, sizeof(mock_spi.tx_data));
    memset(mock_spi.rx_data, 0, sizeof(mock_spi.rx_data));
    mock_spi.transfer_count = 0;
}

uint8_t* mock_spi_get_tx_buffer(void) {
    return mock_spi.tx_data;
}

uint8_t* mock_spi_get_rx_buffer(void) {
    return mock_spi.rx_data;
}

// Mock Timer Implementation
void mock_timer_init(void) {
    memset(&mock_timer, 0, sizeof(mock_timer));
    mock_timer.period = 1000; // 1ms default
    mock_timer.enabled = false;
}

void mock_timer_start(void) {
    mock_timer.enabled = true;
    mock_timer.counter = 0;
}

void mock_timer_stop(void) {
    mock_timer.enabled = false;
}

void mock_timer_set_period(uint32_t period) {
    mock_timer.period = period;
}

uint32_t mock_timer_get_counter(void) {
    return mock_timer.counter;
}

void mock_timer_simulate_tick(void) {
    if (mock_timer.enabled) {
        mock_timer.counter++;
        mock_timer.tick_count++;
        if (mock_timer.counter >= mock_timer.period) {
            mock_timer.counter = 0;
            mock_timer.interrupt_flag = true;
        }
    }
}

bool mock_timer_check_interrupt(void) {
    return mock_timer.interrupt_flag;
}

void mock_timer_clear_interrupt(void) {
    mock_timer.interrupt_flag = false;
}

// Mock System Implementation
void mock_system_init(void) {
    memset(&mock_system, 0, sizeof(mock_system));
    mock_system.cpu_frequency = 550000000; // 550MHz H7S3L8
    mock_system.system_voltage = 3.3f;
    mock_system.system_temperature = 25.0f;
    mock_system.system_initialized = true;
}

void mock_system_reset(void) {
    mock_system.system_tick = 0;
    mock_system.system_initialized = false;
}

uint32_t mock_system_get_tick(void) {
    return mock_system.system_tick;
}

void mock_system_delay(uint32_t ms) {
    mock_system.system_tick += ms;
}

void mock_system_set_frequency(uint32_t frequency) {
    mock_system.cpu_frequency = frequency;
}

uint32_t mock_system_get_frequency(void) {
    return mock_system.cpu_frequency;
}

void mock_system_set_voltage(float voltage) {
    mock_system.system_voltage = voltage;
}

float mock_system_get_voltage(void) {
    return mock_system.system_voltage;
}

void mock_system_set_temperature(float temperature) {
    mock_system.system_temperature = temperature;
}

float mock_system_get_temperature(void) {
    return mock_system.system_temperature;
}

// Mock Interrupt Implementation
void mock_interrupt_init(void) {
    memset(&mock_interrupt, 0, sizeof(mock_interrupt));
}

void mock_interrupt_trigger_exti0(void) {
    mock_interrupt.exti0_flag = true;
    mock_interrupt.interrupt_count++;
}

void mock_interrupt_trigger_exti9_5(void) {
    mock_interrupt.exti9_5_flag = true;
    mock_interrupt.interrupt_count++;
}

void mock_interrupt_trigger_uart_rx(void) {
    mock_interrupt.uart_rx_flag = true;
    mock_interrupt.interrupt_count++;
}

void mock_interrupt_trigger_uart_tx(void) {
    mock_interrupt.uart_tx_flag = true;
    mock_interrupt.interrupt_count++;
}

void mock_interrupt_trigger_dma(void) {
    mock_interrupt.dma_flag = true;
    mock_interrupt.interrupt_count++;
}

bool mock_interrupt_check_exti0(void) {
    return mock_interrupt.exti0_flag;
}

bool mock_interrupt_check_exti9_5(void) {
    return mock_interrupt.exti9_5_flag;
}

bool mock_interrupt_check_uart_rx(void) {
    return mock_interrupt.uart_rx_flag;
}

bool mock_interrupt_check_uart_tx(void) {
    return mock_interrupt.uart_tx_flag;
}

bool mock_interrupt_check_dma(void) {
    return mock_interrupt.dma_flag;
}

void mock_interrupt_clear_all(void) {
    mock_interrupt.exti0_flag = false;
    mock_interrupt.exti9_5_flag = false;
    mock_interrupt.uart_rx_flag = false;
    mock_interrupt.uart_tx_flag = false;
    mock_interrupt.dma_flag = false;
}

uint32_t mock_interrupt_get_count(void) {
    return mock_interrupt.interrupt_count;
}

// Mock Signal Generator Implementation
void mock_signal_generator_init(void) {
    memset(&mock_signal_generator, 0, sizeof(mock_signal_generator));
    mock_signal_generator.noise_level = 0.1f;
    mock_signal_generator.signal_amplitude = 1000.0f;
    mock_signal_generator.signal_frequency = 50.0f;
    srand(time(NULL));
}

void mock_signal_generator_set_emg_pattern(uint8_t channel, float amplitude, float frequency) {
    if (channel < ADS1299_CHANNELS) {
        mock_signal_generator.signal_amplitude = amplitude;
        mock_signal_generator.signal_frequency = frequency;
    }
}

void mock_signal_generator_set_noise_level(float level) {
    mock_signal_generator.noise_level = level;
}

void mock_signal_generator_generate_sine_wave(uint8_t channel, float amplitude, float frequency) {
    if (channel < ADS1299_CHANNELS) {
        for (int i = 0; i < ADS1299_SIGNAL_WINDOW; i++) {
            float time = (float)i / sampling_rate;
            mock_signal_generator.emg_signals[channel][i] = amplitude * sin(2.0f * M_PI * frequency * time);
        }
    }
}

void mock_signal_generator_generate_emg_burst(uint8_t channel, float amplitude, float duration) {
    if (channel < ADS1299_CHANNELS) {
        uint32_t burst_samples = (uint32_t)(duration * sampling_rate);
        for (int i = 0; i < ADS1299_SIGNAL_WINDOW; i++) {
            if (i < burst_samples) {
                float time = (float)i / sampling_rate;
                float envelope = exp(-time * 5.0f); // Exponential decay
                mock_signal_generator.emg_signals[channel][i] = amplitude * envelope * 
                    (sin(2.0f * M_PI * 50.0f * time) + sin(2.0f * M_PI * 80.0f * time) * 0.5f);
            } else {
                mock_signal_generator.emg_signals[channel][i] = 0.0f;
            }
        }
    }
}

void mock_signal_generator_generate_artifact(uint8_t channel, float amplitude) {
    if (channel < ADS1299_CHANNELS) {
        for (int i = 0; i < ADS1299_SIGNAL_WINDOW; i++) {
            mock_signal_generator.emg_signals[channel][i] = amplitude * (((float)rand() / RAND_MAX) - 0.5f);
        }
    }
}

void mock_signal_generator_add_60hz_noise(uint8_t channel, float amplitude) {
    if (channel < ADS1299_CHANNELS) {
        for (int i = 0; i < ADS1299_SIGNAL_WINDOW; i++) {
            float time = (float)i / sampling_rate;
            mock_signal_generator.emg_signals[channel][i] += amplitude * sin(2.0f * M_PI * 60.0f * time);
        }
    }
}

float mock_signal_generator_get_sample(uint8_t channel, uint32_t index) {
    if (channel < ADS1299_CHANNELS && index < ADS1299_SIGNAL_WINDOW) {
        return mock_signal_generator.emg_signals[channel][index];
    }
    return 0.0f;
}

void mock_signal_generator_advance_time(void) {
    mock_signal_generator.signal_index++;
    if (mock_signal_generator.signal_index >= ADS1299_SIGNAL_WINDOW) {
        mock_signal_generator.signal_index = 0;
    }
}

// Hardware Mock Control Functions
void mock_hardware_init_all(void) {
    mock_ads1299_init();
    mock_uart_init();
    mock_gpio_init();
    mock_spi_init();
    mock_timer_init();
    mock_system_init();
    mock_interrupt_init();
    mock_signal_generator_init();
}

void mock_hardware_reset_all(void) {
    mock_ads1299_reset();
    mock_uart_clear_buffers();
    mock_gpio_init();
    mock_spi_clear_buffers();
    mock_timer_init();
    mock_system_reset();
    mock_interrupt_clear_all();
    mock_signal_generator_init();
}

void mock_hardware_simulate_power_on(void) {
    mock_hardware_init_all();
    mock_system.system_initialized = true;
}

void mock_hardware_simulate_power_off(void) {
    mock_hardware_reset_all();
    mock_system.system_initialized = false;
}

void mock_hardware_set_test_mode(bool enabled) {
    test_mode = enabled;
}

bool mock_hardware_get_test_mode(void) {
    return test_mode;
}

// Test Utility Functions
void mock_hardware_inject_fault(const char* component, const char* fault_type) {
    // Implementation for fault injection testing
    printf("MOCK: Injecting fault in %s: %s\n", component, fault_type);
}

void mock_hardware_clear_faults(void) {
    // Implementation for clearing injected faults
    printf("MOCK: All faults cleared\n");
}

bool mock_hardware_has_faults(void) {
    // Implementation for checking if faults are present
    return false;
}

void mock_hardware_print_status(void) {
    printf("=== MOCK HARDWARE STATUS ===\n");
    printf("System initialized: %s\n", mock_system.system_initialized ? "YES" : "NO");
    printf("CPU frequency: %u Hz\n", mock_system.cpu_frequency);
    printf("System voltage: %.2f V\n", mock_system.system_voltage);
    printf("System temperature: %.1f °C\n", mock_system.system_temperature);
    printf("ADS1299 samples: %u\n", mock_ads1299.sample_count);
    printf("UART TX count: %u\n", mock_uart.tx_count);
    printf("UART RX count: %u\n", mock_uart.rx_count);
    printf("Timer ticks: %u\n", mock_timer.tick_count);
    printf("Interrupt count: %u\n", mock_interrupt.interrupt_count);
    printf("============================\n");
}
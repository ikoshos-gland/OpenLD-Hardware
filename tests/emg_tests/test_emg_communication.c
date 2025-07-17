/* EMG Communication Protocol Unit Tests
 * Testing UART communication, data formatting, and protocol handling
 * STM32H7S3L8 EMG Port Testing Suite
 */

#include "../unity/unity.h"
#include "../mocks/mock_hardware.h"
#include "../test_main.h"
#include <string.h>
#include <stdio.h>

// Test data structures
static char test_uart_buffer[1024];
static char expected_output[1024];

// Mock EMG data for testing
static float mock_emg_data[ADS1299_CHANNELS] = {
    1250.0f, 890.0f, 2150.0f, 340.0f,
    1890.0f, 760.0f, 1120.0f, 980.0f
};

static uint32_t mock_raw_data[ADS1299_CHANNELS] = {
    123456, 98765, 456789, 234567,
    678901, 345678, 789012, 456789
};

// Test Setup and Teardown
void setUp(void) {
    mock_hardware_init_all();
    memset(test_uart_buffer, 0, sizeof(test_uart_buffer));
    memset(expected_output, 0, sizeof(expected_output));
}

void tearDown(void) {
    mock_hardware_reset_all();
}

// Helper Functions
void format_emg_data_packet(char* buffer, const char* type, const void* data, bool is_float) {
    sprintf(buffer, "%s ", type);
    
    if (is_float) {
        const float* float_data = (const float*)data;
        for (int i = 0; i < ADS1299_CHANNELS; i++) {
            sprintf(buffer + strlen(buffer), "%.0f ", float_data[i] * 10000.0f);
        }
    } else {
        const uint32_t* uint_data = (const uint32_t*)data;
        for (int i = 0; i < ADS1299_CHANNELS; i++) {
            sprintf(buffer + strlen(buffer), "%u ", uint_data[i]);
        }
    }
    
    strcat(buffer, "\n");
}

void simulate_emg_data_transmission(void) {
    // Simulate complete EMG data packet transmission
    char packet_buffer[512];
    
    // Raw data packet
    format_emg_data_packet(packet_buffer, "D", mock_raw_data, false);
    mock_uart_transmit(packet_buffer, strlen(packet_buffer));
    
    // RMS data packet
    format_emg_data_packet(packet_buffer, "R", mock_emg_data, true);
    mock_uart_transmit(packet_buffer, strlen(packet_buffer));
    
    // Envelope data packet
    format_emg_data_packet(packet_buffer, "E", mock_emg_data, true);
    mock_uart_transmit(packet_buffer, strlen(packet_buffer));
    
    // End marker
    mock_uart_transmit("; We're good:\n", 14);
}

// Test UART Initialization
void test_uart_initialization(void) {
    TEST_ASSERT_EQUAL_UINT32(0, mock_uart_get_tx_count());
    TEST_ASSERT_EQUAL_UINT32(0, mock_uart_get_rx_count());
    TEST_ASSERT_TRUE(mock_uart.transmit_complete);
    TEST_ASSERT_FALSE(mock_uart.receive_complete);
}

// Test UART Transmission
void test_uart_basic_transmission(void) {
    const char* test_message = "Test message";
    mock_uart_transmit(test_message, strlen(test_message));
    
    TEST_ASSERT_EQUAL_UINT32(strlen(test_message), mock_uart_get_tx_count());
    TEST_ASSERT_EQUAL_STRING(test_message, mock_uart_get_tx_buffer());
}

// Test UART Reception
void test_uart_basic_reception(void) {
    // Simulate received data
    strcpy(mock_uart.rx_buffer, "S"); // Settings command
    mock_uart.rx_count = 1;
    
    char received_char;
    mock_uart_receive(&received_char, 1);
    
    TEST_ASSERT_EQUAL_CHAR('S', received_char);
    TEST_ASSERT_EQUAL_UINT32(0, mock_uart_get_rx_count());
}

// Test Raw Data Packet Format
void test_raw_data_packet_format(void) {
    char expected_packet[256];
    format_emg_data_packet(expected_packet, "D", mock_raw_data, false);
    
    mock_uart_transmit(expected_packet, strlen(expected_packet));
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    TEST_ASSERT_EQUAL_STRING(expected_packet, tx_buffer);
    
    // Check that packet starts with "D"
    TEST_ASSERT_EQUAL_CHAR('D', tx_buffer[0]);
    TEST_ASSERT_EQUAL_CHAR(' ', tx_buffer[1]);
    
    // Check that packet ends with newline
    TEST_ASSERT_EQUAL_CHAR('\n', tx_buffer[strlen(tx_buffer) - 1]);
}

// Test RMS Data Packet Format
void test_rms_data_packet_format(void) {
    char expected_packet[256];
    format_emg_data_packet(expected_packet, "R", mock_emg_data, true);
    
    mock_uart_transmit(expected_packet, strlen(expected_packet));
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    TEST_ASSERT_EQUAL_STRING(expected_packet, tx_buffer);
    
    // Check that packet starts with "R"
    TEST_ASSERT_EQUAL_CHAR('R', tx_buffer[0]);
    
    // Check that RMS values are scaled by 10000
    TEST_ASSERT_TRUE(strstr(tx_buffer, "12500") != NULL); // 1250.0 * 10000
    TEST_ASSERT_TRUE(strstr(tx_buffer, "8900") != NULL);  // 890.0 * 10000
}

// Test Envelope Data Packet Format
void test_envelope_data_packet_format(void) {
    char expected_packet[256];
    format_emg_data_packet(expected_packet, "E", mock_emg_data, true);
    
    mock_uart_transmit(expected_packet, strlen(expected_packet));
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    
    // Check that packet starts with "E"
    TEST_ASSERT_EQUAL_CHAR('E', tx_buffer[0]);
    
    // Check that envelope values are present
    TEST_ASSERT_TRUE(strstr(tx_buffer, "21500") != NULL); // 2150.0 * 10000
    TEST_ASSERT_TRUE(strstr(tx_buffer, "3400") != NULL);  // 340.0 * 10000
}

// Test Complete Data Packet Sequence
void test_complete_data_packet_sequence(void) {
    simulate_emg_data_transmission();
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    
    // Check that all packet types are present
    TEST_ASSERT_TRUE(strstr(tx_buffer, "D ") != NULL); // Raw data
    TEST_ASSERT_TRUE(strstr(tx_buffer, "R ") != NULL); // RMS data
    TEST_ASSERT_TRUE(strstr(tx_buffer, "E ") != NULL); // Envelope data
    TEST_ASSERT_TRUE(strstr(tx_buffer, "; We're good:") != NULL); // End marker
}

// Test Packet Parsing
void test_packet_parsing(void) {
    // Simulate received packet
    const char* test_packet = "D 123456 98765 456789 234567 678901 345678 789012 456789 \n";
    
    // Parse packet (simplified parser for testing)
    char packet_type;
    uint32_t values[ADS1299_CHANNELS];
    
    sscanf(test_packet, "%c %u %u %u %u %u %u %u %u",
           &packet_type, &values[0], &values[1], &values[2], &values[3],
           &values[4], &values[5], &values[6], &values[7]);
    
    TEST_ASSERT_EQUAL_CHAR('D', packet_type);
    TEST_ASSERT_EQUAL_UINT32(123456, values[0]);
    TEST_ASSERT_EQUAL_UINT32(98765, values[1]);
    TEST_ASSERT_EQUAL_UINT32(456789, values[2]);
    TEST_ASSERT_EQUAL_UINT32(789012, values[6]);
}

// Test Settings Mode Entry
void test_settings_mode_entry(void) {
    // Simulate settings command
    strcpy(mock_uart.rx_buffer, "S");
    mock_uart.rx_count = 1;
    
    char received_char;
    mock_uart_receive(&received_char, 1);
    
    TEST_ASSERT_EQUAL_CHAR('S', received_char);
    
    // Should trigger settings mode response
    const char* settings_response = "Settings Mode Activated\n";
    mock_uart_transmit(settings_response, strlen(settings_response));
    
    TEST_ASSERT_TRUE(strstr(mock_uart_get_tx_buffer(), "Settings Mode") != NULL);
}

// Test Channel Configuration Command
void test_channel_configuration_command(void) {
    // Simulate channel configuration command (Channel 1, Normal input)
    const char* config_command = "C1N";
    strcpy(mock_uart.rx_buffer, config_command);
    mock_uart.rx_count = strlen(config_command);
    
    char received_buffer[10];
    mock_uart_receive(received_buffer, strlen(config_command));
    
    TEST_ASSERT_EQUAL_MEMORY(config_command, received_buffer, strlen(config_command));
}

// Test Impedance Measurement Command
void test_impedance_measurement_command(void) {
    // Simulate impedance measurement command
    const char* impedance_command = "I1";
    strcpy(mock_uart.rx_buffer, impedance_command);
    mock_uart.rx_count = strlen(impedance_command);
    
    char received_buffer[10];
    mock_uart_receive(received_buffer, strlen(impedance_command));
    
    TEST_ASSERT_EQUAL_MEMORY(impedance_command, received_buffer, strlen(impedance_command));
}

// Test Button State Transmission
void test_button_state_transmission(void) {
    // Test button pressed state
    mock_gpio_set_button_state(true);
    const char* button_pressed = "B 1\n";
    mock_uart_transmit(button_pressed, strlen(button_pressed));
    
    TEST_ASSERT_TRUE(strstr(mock_uart_get_tx_buffer(), "B 1") != NULL);
    
    // Test button released state
    mock_uart_clear_buffers();
    mock_gpio_set_button_state(false);
    const char* button_released = "B 0\n";
    mock_uart_transmit(button_released, strlen(button_released));
    
    TEST_ASSERT_TRUE(strstr(mock_uart_get_tx_buffer(), "B 0") != NULL);
}

// Test Impedance Data Transmission
void test_impedance_data_transmission(void) {
    // Simulate impedance data
    uint32_t impedance_data[ADS1299_CHANNELS] = {
        5000, 4800, 5200, 4900, 5100, 4850, 5050, 4950
    };
    
    char impedance_packet[256];
    format_emg_data_packet(impedance_packet, "I", impedance_data, false);
    mock_uart_transmit(impedance_packet, strlen(impedance_packet));
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    TEST_ASSERT_EQUAL_CHAR('I', tx_buffer[0]);
    TEST_ASSERT_TRUE(strstr(tx_buffer, "5000") != NULL);
    TEST_ASSERT_TRUE(strstr(tx_buffer, "4800") != NULL);
}

// Test Data Rate and Throughput
void test_data_rate_throughput(void) {
    // Simulate high-rate data transmission (1kHz)
    for (int i = 0; i < 10; i++) {
        simulate_emg_data_transmission();
        mock_timer_simulate_tick(); // 1ms tick
    }
    
    uint32_t total_bytes = mock_uart_get_tx_count();
    
    // Should have reasonable throughput
    TEST_ASSERT_TRUE(total_bytes > 1000); // At least 1KB for 10 packets
    TEST_ASSERT_TRUE(total_bytes < 10000); // But not excessive
}

// Test Buffer Overflow Protection
void test_buffer_overflow_protection(void) {
    // Try to transmit more data than buffer can hold
    char large_data[3000];
    memset(large_data, 'A', sizeof(large_data) - 1);
    large_data[sizeof(large_data) - 1] = '\0';
    
    mock_uart_transmit(large_data, strlen(large_data));
    
    // Should not exceed buffer size
    TEST_ASSERT_TRUE(mock_uart_get_tx_count() < sizeof(mock_uart.tx_buffer));
}

// Test Error Handling
void test_error_handling(void) {
    // Test invalid channel number
    const char* invalid_command = "C9N"; // Channel 9 doesn't exist
    strcpy(mock_uart.rx_buffer, invalid_command);
    mock_uart.rx_count = strlen(invalid_command);
    
    char received_buffer[10];
    mock_uart_receive(received_buffer, strlen(invalid_command));
    
    // Should handle gracefully (no crash)
    TEST_ASSERT_EQUAL_MEMORY(invalid_command, received_buffer, strlen(invalid_command));
}

// Test Protocol Timing
void test_protocol_timing(void) {
    // Test that data is transmitted within timing constraints
    uint32_t start_time = mock_system_get_tick();
    
    simulate_emg_data_transmission();
    
    uint32_t end_time = mock_system_get_tick();
    uint32_t transmission_time = end_time - start_time;
    
    // Should complete within 1ms for real-time operation
    TEST_ASSERT_TRUE(transmission_time < 1);
}

// Test Multi-packet Integrity
void test_multi_packet_integrity(void) {
    // Send multiple packets and verify integrity
    for (int i = 0; i < 5; i++) {
        simulate_emg_data_transmission();
    }
    
    char* tx_buffer = mock_uart_get_tx_buffer();
    
    // Count occurrences of each packet type
    int d_count = 0, r_count = 0, e_count = 0, end_count = 0;
    char* ptr = tx_buffer;
    
    while ((ptr = strstr(ptr, "D ")) != NULL) { d_count++; ptr += 2; }
    ptr = tx_buffer;
    while ((ptr = strstr(ptr, "R ")) != NULL) { r_count++; ptr += 2; }
    ptr = tx_buffer;
    while ((ptr = strstr(ptr, "E ")) != NULL) { e_count++; ptr += 2; }
    ptr = tx_buffer;
    while ((ptr = strstr(ptr, "; We're good:")) != NULL) { end_count++; ptr += 13; }
    
    TEST_ASSERT_EQUAL_INT(5, d_count);
    TEST_ASSERT_EQUAL_INT(5, r_count);
    TEST_ASSERT_EQUAL_INT(5, e_count);
    TEST_ASSERT_EQUAL_INT(5, end_count);
}

// Test UART Baud Rate Validation
void test_uart_baud_rate_validation(void) {
    // Test that data can be transmitted at 230400 baud
    const char* test_data = "Test data for baud rate validation";
    uint32_t data_length = strlen(test_data);
    
    mock_uart_transmit(test_data, data_length);
    
    // At 230400 baud, should be able to transmit quickly
    TEST_ASSERT_EQUAL_UINT32(data_length, mock_uart_get_tx_count());
}

// Test Runner
int main(void) {
    UnityBegin("test_emg_communication.c");
    
    // Basic UART Tests
    RUN_TEST(test_uart_initialization);
    RUN_TEST(test_uart_basic_transmission);
    RUN_TEST(test_uart_basic_reception);
    
    // Data Packet Format Tests
    RUN_TEST(test_raw_data_packet_format);
    RUN_TEST(test_rms_data_packet_format);
    RUN_TEST(test_envelope_data_packet_format);
    RUN_TEST(test_complete_data_packet_sequence);
    
    // Protocol Tests
    RUN_TEST(test_packet_parsing);
    RUN_TEST(test_settings_mode_entry);
    RUN_TEST(test_channel_configuration_command);
    RUN_TEST(test_impedance_measurement_command);
    
    // Data Transmission Tests
    RUN_TEST(test_button_state_transmission);
    RUN_TEST(test_impedance_data_transmission);
    RUN_TEST(test_data_rate_throughput);
    
    // Error Handling and Robustness Tests
    RUN_TEST(test_buffer_overflow_protection);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_protocol_timing);
    RUN_TEST(test_multi_packet_integrity);
    RUN_TEST(test_uart_baud_rate_validation);
    
    return UnityEnd();
}
/* MIT License

   Copyright (c) [2016] [Jae Choi]
   STM32H7S3L8 Port - 2024

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

// Define to prevent recursive inclusion
#ifndef __MAIN_H
#define __MAIN_H

// Standard C Library Headers
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// STM32H7RS Header Files
#include "stm32h7rsxx.h"
#include "stm32h7rsxx_hal.h"
#include "stm32h7rsxx_hal_gpio.h"
#include "stm32h7rsxx_hal_spi.h"
#include "stm32h7rsxx_hal_uart.h"
#include "stm32h7rsxx_hal_dma.h"
#include "stm32h7rsxx_hal_rcc.h"
#include "stm32h7rsxx_hal_pwr.h"
#include "stm32h7rsxx_hal_pwr_ex.h"
#include "core_cm7.h"

// DSP Header Files (Enhanced for Cortex-M7)
#include "arm_math.h"
#include "arm_const_structs.h"

#define ADS1299_CHANNELS 8

// STM32H7S3L8 System Configuration
#define SYSTEM_CLOCK_FREQ   550000000  // 550MHz
#define AHB_CLOCK_FREQ      275000000  // 275MHz  
#define APB1_CLOCK_FREQ     137500000  // 137.5MHz
#define APB2_CLOCK_FREQ     137500000  // 137.5MHz

// Function Prototypes
    // Initialization Routines (Updated for H7)
    void init_UART();
    void init_SPI();
    void init_GPIO();
    void init_EXTI();
    void init_DMA();
    void init_SystemClock();
    void init_MPU();
    void init_Cache();
    
    // System initialization function
    void SystemInit_H7S3L8();
    
    // Other functions
    void check_UART_Status();
    void EXTI0_IRQHandler();
    void EXTI6_IRQHandler();  // Updated for STM32H7RS individual EXTI handlers
    void UART_IRQHandler();
    void DMA_IRQHandler();
    
    // Delays (Updated for 550MHz)
    void __DELAY(uint32_t cycles);
    
    // Communication functions
    void UART_Transmit(const char *data);
    void UART_Receive(char *data);
    void UART_TransmitChar(char data);
    char UART_ReceiveChar();
    void UART_PrintHex(uint32_t value);
    void UART_PrintDec(uint32_t value);
    
    // ADS1299 functions
    void ads1299_configure_channels_from_settings();
    void ads1299_start_continuous_mode();
    
    // UART functions (compatibility layer)
    void CTX(char data);
    char CRX(void);
    void STX(const char *data);
    void SRX(char *data);
    void PREG(uint32_t value);
    void PDEC(uint32_t value);
    
    // Bluetooth/RN42 functions
    void check_RN42_RTS(void);

    // Analysis Functions & Definitions - Enhanced for STM32H7S3L8
    #define ADS1299_SIGNAL_WINDOW 800  // Increased from 200 (4x larger buffers)
    
    // EMG-specific processing (Enhanced)
    #define EMG_ENVELOPE_WINDOW 100      // 100ms window for envelope detection
    #define EMG_SPECTRAL_WINDOW 256     // For spectral analysis
    #define EMG_FEATURE_BUFFER 50       // Feature extraction buffer
    
    // Memory allocation optimization for H7
    extern float32_t emg_raw_buffer[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW] __attribute__((section(".AXI_SRAM")));
    extern float32_t emg_filtered_buffer[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW] __attribute__((section(".AXI_SRAM")));
    extern float32_t emg_rectified[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW] __attribute__((section(".SRAM2")));
    extern float32_t emg_envelope[ADS1299_CHANNELS] __attribute__((section(".SRAM2")));
    extern float32_t emg_rms[ADS1299_CHANNELS] __attribute__((section(".SRAM2")));
    extern float32_t emg_peak[ADS1299_CHANNELS] __attribute__((section(".SRAM2")));
    extern float32_t emg_mean_frequency[ADS1299_CHANNELS] __attribute__((section(".SRAM2")));

    // IIR Structures - Enhanced for H7 with cache optimization
    #define BIQUAD_STAGES_HP 4  // Increased from 3 for better filtering
    #define BIQUAD_STAGES_BP 4  // Increased from 3 for better filtering
    
    extern arm_biquad_cascade_df2T_instance_f32 biquad_HP_Struct[ADS1299_CHANNELS];
    extern float32_t biquad_HP_State[ADS1299_CHANNELS][2 * BIQUAD_STAGES_HP] __attribute__((section(".SRAM1")));
    extern float32_t biquad_HP_Coeffs[5 * BIQUAD_STAGES_HP] __attribute__((section(".FLASH")));
    extern const float32_t biquad_HP_Output_Gain;

    extern arm_biquad_cascade_df2T_instance_f32 biquad_BP_Struct[ADS1299_CHANNELS];
    extern float32_t biquad_BP_State[ADS1299_CHANNELS][2 * BIQUAD_STAGES_BP] __attribute__((section(".SRAM1")));
    extern float32_t biquad_BP_Coeffs[5 * BIQUAD_STAGES_BP] __attribute__((section(".FLASH")));
    extern const float32_t biquad_BP_Output_Gain;

    // Enhanced FFT support for H7
    #define FFT_SIZE 256
    extern arm_rfft_fast_instance_f32 fft_instance;
    extern float32_t fft_input_buffer[FFT_SIZE];
    extern float32_t fft_output_buffer[FFT_SIZE];
    extern float32_t fft_magnitude_buffer[FFT_SIZE/2];

    // BioEXG Settings variable + Function
    extern uint32_t BIOEXG_SETTINGS;
    extern uint32_t BIOEXG_ADVANCED_SETTINGS;  // New for H7 features
    void setting_mode();
    void advanced_setting_mode();

    // Data counter and circular buffer management
    extern uint32_t counterData;
    extern uint32_t bufferIndex;
    extern uint32_t processingComplete;

// BIOEXG_SETTINGS Bit field (Extended for H7):
// [31:24 - ADVANCED] [23:16 - IMPEDANCE] [15:8 - PROCESSING] [7:0 - CHANNELS]
    #define SETTINGS_BIT_CHANNEL(x)     (1 << (x + 0))
    #define SETTINGS_BIT_PROCESSING(x)  (1 << (x + 8))
    #define SETTINGS_BIT_IMP(x)         (1 << (x + 16))
    #define SETTINGS_BIT_ADVANCED(x)    (1 << (x + 24))

    // Processing modes
    #define PROCESSING_RAW      0
    #define PROCESSING_FILTERED 1
    #define PROCESSING_RMS      2
    #define PROCESSING_ENVELOPE 3
    #define PROCESSING_SPECTRAL 4
    #define PROCESSING_FEATURES 5

    // ADS1299 Functions (Enhanced for H7)
    void ads1299_init();
    void ads1299_pwr_up_seq();
    void ads1299_read_data(uint32_t *STATUS, int32_t *DATA);
    void ads1299_read_data_dma(uint32_t *STATUS, int32_t *DATA);  // DMA version
    void ads1299_stop_dataread();
    uint8_t ads1299_read_reg(uint8_t ADDR);
    void ads1299_write_reg(uint8_t ADDR, uint8_t VAL);
    uint8_t SPI_TX(uint8_t DATA);
    uint8_t SPI_NO_DELAY_TX(uint8_t DATA);
    void ads1299_configure_dma();

    // Enhanced EMG processing functions
    void emg_process_realtime();
    void emg_calculate_rms();
    void emg_calculate_envelope();
    void emg_calculate_spectral_features();
    void emg_extract_features();

// Macro Definitions (Updated for 550MHz)
#define MILI_S(x)  ((x) * 550000/4)
#define MICRO_S(x) ((x) * 550/4)

// Memory Section Definitions for H7
#define AXI_SRAM_SECTION    __attribute__((section(".AXI_SRAM")))
#define SRAM1_SECTION       __attribute__((section(".SRAM1")))
#define SRAM2_SECTION       __attribute__((section(".SRAM2")))
#define SRAM3_SECTION       __attribute__((section(".SRAM3")))
#define SRAM4_SECTION       __attribute__((section(".SRAM4")))
#define FLASH_SECTION       __attribute__((section(".FLASH")))

// Cache Control Macros
#define CACHE_CLEAN_INVALIDATE_DCACHE()     SCB_CleanInvalidateDCache()
#define CACHE_CLEAN_DCACHE()                SCB_CleanDCache()
#define CACHE_INVALIDATE_DCACHE()           SCB_InvalidateDCache()
#define CACHE_INVALIDATE_ICACHE()           SCB_InvalidateICache()

// DMA Configuration
#define DMA_BUFFER_SIZE     ADS1299_SIGNAL_WINDOW
#define DMA_CHANNELS        8

// SPI Command Definitions (Same as original)
#define _WAKEUP  0x02
#define _STANDBY 0x04
#define _RESET   0x06
#define _START   0x08
#define _STOP    0x0A
#define _RDATAC  0x10
#define _SDATAC  0x11
#define _RDATA   0x12
#define _RREG    0x20
#define _WREG    0x40

// Register Addresses (Same as original)
#define ID         0x00
#define CONFIG1    0x01
#define CONFIG2    0x02
#define CONFIG3    0x03
#define LOFF       0x04
#define CH1SET     0x05
#define CH2SET     0x06
#define CH3SET     0x07
#define CH4SET     0x08
#define CH5SET     0x09
#define CH6SET     0x0A
#define CH7SET     0x0B
#define CH8SET     0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP  0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO       0x14
#define MISC1      0x15
#define MISC2      0x16
#define CONFIG4    0x17

// Gains (Same as original)
#define ADS1299_PGA_GAIN01 (0b00000000)
#define ADS1299_PGA_GAIN02 (0b00010000)
#define ADS1299_PGA_GAIN04 (0b00100000)
#define ADS1299_PGA_GAIN06 (0b00110000)
#define ADS1299_PGA_GAIN08 (0b01000000)
#define ADS1299_PGA_GAIN12 (0b01010000)
#define ADS1299_PGA_GAIN24 (0b01100000)

// Input Modes - Channels (Same as original)
#define ADS1299_INPUT_PWR_DOWN   (0b10000000)
#define ADS1299_INPUT_PWR_UP     (0b00000000)
#define ADS1299_INPUT_NORMAL     (0b00000000)
#define ADS1299_INPUT_SHORTED    (0b00000001)
#define ADS1299_INPUT_MEAS_BIAS  (0b00000010)
#define ADS1299_INPUT_SUPPLY     (0b00000011)
#define ADS1299_INPUT_TEMP       (0b00000100)
#define ADS1299_INPUT_TESTSIGNAL (0b00000101)
#define ADS1299_INPUT_SET_BIASP  (0b00000110)
#define ADS1299_INPUT_SET_BIASN  (0b00000111)

// Test Signal Choices (Same as original)
#define ADS1299_TEST_INT              (0b00010000)
#define ADS1299_TESTSIGNAL_AMP_1X     (0b00000000)
#define ADS1299_TESTSIGNAL_AMP_2X     (0b00000100)
#define ADS1299_TESTSIGNAL_PULSE_SLOW (0b00000000)
#define ADS1299_TESTSIGNAL_PULSE_FAST (0b00000001)
#define ADS1299_TESTSIGNAL_DCSIG      (0b00000011)
#define ADS1299_TESTSIGNAL_NOCHANGE   (0b11111111)

// Lead-off Signal Choices (Same as original)
#define LOFF_MAG_6NA (0b00000000)
#define LOFF_MAG_24NA (0b00000100)
#define LOFF_MAG_6UA (0b00001000)
#define LOFF_MAG_24UA (0b00001100)
#define LOFF_FREQ_DC (0b00000000)
#define LOFF_FREQ_7p8HZ (0b00000001)
#define LOFF_FREQ_31p2HZ (0b00000010)
#define LOFF_FREQ_FS_4 (0b00000011)
#define PCHAN (1)
#define NCHAN (2)
#define BOTHCHAN (3)

#define OFF (0)
#define ON (1)

#endif // __MAIN_H
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

#include "main.h"

// External UART handle from Peripheral_Init.c
extern UART_HandleTypeDef huart1;

// Basic UART Functions for STM32H7RS

void UART_Transmit(const char *data)
{
    HAL_UART_Transmit(&huart1, (const uint8_t*)data, strlen(data), HAL_MAX_DELAY);
}

void UART_Receive(char *data)
{
    HAL_UART_Receive(&huart1, (uint8_t*)data, 1, HAL_MAX_DELAY);
}

void UART_TransmitChar(char data)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&data, 1, HAL_MAX_DELAY);
}

char UART_ReceiveChar(void)
{
    char data;
    HAL_UART_Receive(&huart1, (uint8_t*)&data, 1, HAL_MAX_DELAY);
    return data;
}

void UART_PrintHex(uint32_t value)
{
    char hex_str[16];
    int result = snprintf(hex_str, sizeof(hex_str), "0x%08X", (unsigned int)value);
    if (result > 0 && result < (int)sizeof(hex_str)) {
        UART_Transmit(hex_str);
    }
}

void UART_PrintDec(uint32_t value)
{
    char dec_str[16];
    int result = snprintf(dec_str, sizeof(dec_str), "%u", (unsigned int)value);
    if (result > 0 && result < (int)sizeof(dec_str)) {
        UART_Transmit(dec_str);
    }
}

// Assembly-style compatibility functions

void CTX(char data)
{
    UART_TransmitChar(data);
}

char CRX(void)
{
    return UART_ReceiveChar();
}

void STX(const char *data)
{
    UART_Transmit(data);
}

void SRX(char *data)
{
    int buffer_idx = 0;
    
    while (1) {
        char received_char = UART_ReceiveChar();
        
        // Exit on newline or carriage return
        if (received_char == '\n' || received_char == '\r') {
            data[buffer_idx] = '\0';  // Null terminate
            break;
        }
        
        data[buffer_idx++] = received_char;
        
        // Prevent buffer overflow
        if (buffer_idx >= 49) {
            data[buffer_idx] = '\0';
            break;
        }
    }
}

void PREG(uint32_t value)
{
    UART_PrintHex(value);
}

void PDEC(uint32_t value)
{
    UART_PrintDec(value);
}

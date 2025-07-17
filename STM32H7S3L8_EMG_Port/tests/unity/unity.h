/* Unity Test Framework Header
 * Embedded C Unit Testing Framework
 * Optimized for STM32H7S3L8 EMG Project
 */

#ifndef UNITY_H
#define UNITY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Unity Configuration for STM32H7S3L8
#define UNITY_INCLUDE_SETUP_STUBS
#define UNITY_INCLUDE_TEARDOWN_STUBS
#define UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_FLOAT
#define UNITY_INCLUDE_DOUBLE
#define UNITY_SUPPORT_64

// Test result tracking
typedef struct {
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    uint32_t CurrentTestFailed;
    uint32_t CurrentTestIgnored;
    const char* TestFile;
    const char* CurrentTestName;
    uint32_t CurrentTestLineNumber;
} UNITY_FIXTURE_T;

extern UNITY_FIXTURE_T Unity;

// Core Unity Functions
void UnityBegin(const char* filename);
int UnityEnd(void);
void UnityPrint(const char* string);
void UnityPrintNumberByStyle(const long number, const int style);
void UnityPrintFloat(const float number);
void UnityPrintDouble(const double number);

// Test Control
void UnityDefaultTestRun(void (*Func)(void), const char* FuncName, const int FuncLineNum);
void UnityIgnore(const char* message, const int line);
void UnityFail(const char* message, const int line);

// Assertions
void UnityAssertEqualNumber(const long expected, const long actual, const char* message, const int line);
void UnityAssertEqualIntArray(const int* expected, const int* actual, const uint32_t num_elements, const char* message, const int line);
void UnityAssertEqualFloatArray(const float* expected, const float* actual, const uint32_t num_elements, const float delta, const char* message, const int line);
void UnityAssertFloatsWithin(const float delta, const float expected, const float actual, const char* message, const int line);
void UnityAssertDoublesWithin(const double delta, const double expected, const double actual, const char* message, const int line);
void UnityAssertStringEqual(const char* expected, const char* actual, const char* message, const int line);
void UnityAssertBits(const long mask, const long expected, const long actual, const char* message, const int line);
void UnityAssertEqualMemory(const void* expected, const void* actual, const uint32_t length, const char* message, const int line);

// Macro Definitions
#define TEST_FAIL_MESSAGE(message)                                                                 UnityFail( (message), __LINE__ )
#define TEST_FAIL()                                                                                UnityFail( NULL, __LINE__ )
#define TEST_IGNORE_MESSAGE(message)                                                               UnityIgnore( (message), __LINE__ )
#define TEST_IGNORE()                                                                              UnityIgnore( NULL, __LINE__ )
#define TEST_ASSERT_TRUE(condition)                                                                { if (!(condition)) UnityFail("Expected TRUE Was FALSE", __LINE__); }
#define TEST_ASSERT_FALSE(condition)                                                               { if (condition) UnityFail("Expected FALSE Was TRUE", __LINE__); }
#define TEST_ASSERT_NULL(pointer)                                                                  { if ((pointer) != NULL) UnityFail("Expected NULL", __LINE__); }
#define TEST_ASSERT_NOT_NULL(pointer)                                                              { if ((pointer) == NULL) UnityFail("Expected Non-NULL", __LINE__); }
#define TEST_ASSERT_EQUAL_INT(expected, actual)                                                    UnityAssertEqualNumber((long)(expected), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT32(expected, actual)                                                 UnityAssertEqualNumber((long)(expected), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_CHAR(expected, actual)                                                   UnityAssertEqualNumber((long)(expected), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual)                                                  UnityAssertFloatsWithin((float)(0.00001), (float)(expected), (float)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual)                                                 UnityAssertDoublesWithin((double)(0.00001), (double)(expected), (double)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_STRING(expected, actual)                                                 UnityAssertStringEqual((const char*)(expected), (const char*)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, length)                                         UnityAssertEqualMemory((void*)(expected), (void*)(actual), (uint32_t)(length), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)                                UnityAssertEqualIntArray((expected), (actual), (uint32_t)(num_elements), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)                              UnityAssertEqualFloatArray((expected), (actual), (uint32_t)(num_elements), (float)(0.00001), NULL, __LINE__)
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)                                          UnityAssertFloatsWithin((float)(delta), (float)(expected), (float)(actual), NULL, __LINE__)
#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)                                         UnityAssertDoublesWithin((double)(delta), (double)(expected), (double)(actual), NULL, __LINE__)
#define TEST_ASSERT_BIT_HIGH(bit, actual)                                                          UnityAssertBits((long)(1 << bit), (long)(1 << bit), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_BIT_LOW(bit, actual)                                                           UnityAssertBits((long)(1 << bit), (long)(0), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_BITS_HIGH(mask, actual)                                                        UnityAssertBits((long)(mask), (long)(mask), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_BITS_LOW(mask, actual)                                                         UnityAssertBits((long)(mask), (long)(0), (long)(actual), NULL, __LINE__)
#define TEST_ASSERT_BITS(mask, expected, actual)                                                   UnityAssertBits((long)(mask), (long)(expected), (long)(actual), NULL, __LINE__)

// Test Runner Macros
#define RUN_TEST(func)                                                                             UnityDefaultTestRun(func, #func, __LINE__)
#define TEST_SETUP()                                                                               void setUp(void)
#define TEST_TEARDOWN()                                                                            void tearDown(void)

// EMG-Specific Test Macros
#define TEST_ASSERT_EMG_SIGNAL_RANGE(signal, min_val, max_val)                                     { if ((signal) < (min_val) || (signal) > (max_val)) UnityFail("EMG signal out of range", __LINE__); }
#define TEST_ASSERT_FREQUENCY_WITHIN(expected_freq, actual_freq, tolerance)                        UnityAssertFloatsWithin((float)(tolerance), (float)(expected_freq), (float)(actual_freq), NULL, __LINE__)
#define TEST_ASSERT_FILTER_RESPONSE(input, output, expected_gain, tolerance)                       { float gain = (output) / (input); UnityAssertFloatsWithin((float)(tolerance), (float)(expected_gain), gain, NULL, __LINE__); }
#define TEST_ASSERT_RMS_CALCULATION(signal_array, length, expected_rms, tolerance)                 { float calculated_rms = calculate_rms_test(signal_array, length); UnityAssertFloatsWithin((float)(tolerance), (float)(expected_rms), calculated_rms, NULL, __LINE__); }

#endif /* UNITY_H */
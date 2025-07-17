/* Unity Test Framework Implementation
 * Embedded C Unit Testing Framework
 * Optimized for STM32H7S3L8 EMG Project
 */

#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Global Unity fixture
UNITY_FIXTURE_T Unity;

// Test output function (can be redirected to UART for embedded testing)
void UnityPrint(const char* string) {
    printf("%s", string);
}

void UnityPrintNumberByStyle(const long number, const int style) {
    switch (style) {
        case 1:
            printf("%ld", number);
            break;
        case 2:
            printf("0x%lX", number);
            break;
        case 3:
            printf("0%lo", number);
            break;
        default:
            printf("%ld", number);
            break;
    }
}

void UnityPrintFloat(const float number) {
    printf("%.6f", number);
}

void UnityPrintDouble(const double number) {
    printf("%.15f", number);
}

void UnityBegin(const char* filename) {
    Unity.TestFile = filename;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestIgnores = 0;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;
}

int UnityEnd(void) {
    UnityPrint("\n-----------------------\n");
    UnityPrint("UNITY TEST RESULTS\n");
    UnityPrint("-----------------------\n");
    
    UnityPrintNumberByStyle(Unity.NumberOfTests, 1);
    UnityPrint(" Tests ");
    UnityPrintNumberByStyle(Unity.TestFailures, 1);
    UnityPrint(" Failures ");
    UnityPrintNumberByStyle(Unity.TestIgnores, 1);
    UnityPrint(" Ignored\n");
    
    if (Unity.TestFailures == 0) {
        UnityPrint("OK\n");
    } else {
        UnityPrint("FAIL\n");
    }
    
    return Unity.TestFailures;
}

void UnityDefaultTestRun(void (*Func)(void), const char* FuncName, const int FuncLineNum) {
    Unity.CurrentTestName = FuncName;
    Unity.CurrentTestLineNumber = FuncLineNum;
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;
    
    UnityPrint("TEST(");
    UnityPrint(Unity.TestFile);
    UnityPrint(", ");
    UnityPrint(FuncName);
    UnityPrint(")");
    
    if (Func) {
        Func();
    }
    
    if (Unity.CurrentTestIgnored) {
        UnityPrint(" IGNORED");
    } else if (Unity.CurrentTestFailed) {
        UnityPrint(" FAILED");
        Unity.TestFailures++;
    } else {
        UnityPrint(" PASSED");
    }
    UnityPrint("\n");
}

void UnityFail(const char* message, const int line) {
    Unity.CurrentTestFailed = 1;
    Unity.CurrentTestLineNumber = line;
    UnityPrint("\n  FAIL: ");
    UnityPrint(Unity.TestFile);
    UnityPrint(":");
    UnityPrintNumberByStyle(line, 1);
    UnityPrint(": ");
    if (message) {
        UnityPrint(message);
    } else {
        UnityPrint("Test Failed");
    }
    UnityPrint("\n");
}

void UnityIgnore(const char* message, const int line) {
    Unity.CurrentTestIgnored = 1;
    Unity.TestIgnores++;
    Unity.CurrentTestLineNumber = line;
    UnityPrint("\n  IGNORE: ");
    UnityPrint(Unity.TestFile);
    UnityPrint(":");
    UnityPrintNumberByStyle(line, 1);
    UnityPrint(": ");
    if (message) {
        UnityPrint(message);
    } else {
        UnityPrint("Test Ignored");
    }
    UnityPrint("\n");
}

void UnityAssertEqualNumber(const long expected, const long actual, const char* message, const int line) {
    if (expected != actual) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Expected ");
        UnityPrintNumberByStyle(expected, 1);
        UnityPrint(" Was ");
        UnityPrintNumberByStyle(actual, 1);
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

void UnityAssertEqualIntArray(const int* expected, const int* actual, const uint32_t num_elements, const char* message, const int line) {
    uint32_t i;
    for (i = 0; i < num_elements; i++) {
        if (expected[i] != actual[i]) {
            Unity.CurrentTestFailed = 1;
            Unity.CurrentTestLineNumber = line;
            UnityPrint("\n  FAIL: ");
            UnityPrint(Unity.TestFile);
            UnityPrint(":");
            UnityPrintNumberByStyle(line, 1);
            UnityPrint(": Expected ");
            UnityPrintNumberByStyle(expected[i], 1);
            UnityPrint(" Was ");
            UnityPrintNumberByStyle(actual[i], 1);
            UnityPrint(" At Element ");
            UnityPrintNumberByStyle(i, 1);
            if (message) {
                UnityPrint(": ");
                UnityPrint(message);
            }
            UnityPrint("\n");
            break;
        }
    }
}

void UnityAssertEqualFloatArray(const float* expected, const float* actual, const uint32_t num_elements, const float delta, const char* message, const int line) {
    uint32_t i;
    for (i = 0; i < num_elements; i++) {
        if (fabs(expected[i] - actual[i]) > delta) {
            Unity.CurrentTestFailed = 1;
            Unity.CurrentTestLineNumber = line;
            UnityPrint("\n  FAIL: ");
            UnityPrint(Unity.TestFile);
            UnityPrint(":");
            UnityPrintNumberByStyle(line, 1);
            UnityPrint(": Expected ");
            UnityPrintFloat(expected[i]);
            UnityPrint(" Was ");
            UnityPrintFloat(actual[i]);
            UnityPrint(" At Element ");
            UnityPrintNumberByStyle(i, 1);
            if (message) {
                UnityPrint(": ");
                UnityPrint(message);
            }
            UnityPrint("\n");
            break;
        }
    }
}

void UnityAssertFloatsWithin(const float delta, const float expected, const float actual, const char* message, const int line) {
    if (fabs(expected - actual) > delta) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Expected ");
        UnityPrintFloat(expected);
        UnityPrint(" Was ");
        UnityPrintFloat(actual);
        UnityPrint(" (Delta: ");
        UnityPrintFloat(delta);
        UnityPrint(")");
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

void UnityAssertDoublesWithin(const double delta, const double expected, const double actual, const char* message, const int line) {
    if (fabs(expected - actual) > delta) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Expected ");
        UnityPrintDouble(expected);
        UnityPrint(" Was ");
        UnityPrintDouble(actual);
        UnityPrint(" (Delta: ");
        UnityPrintDouble(delta);
        UnityPrint(")");
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

void UnityAssertStringEqual(const char* expected, const char* actual, const char* message, const int line) {
    if (strcmp(expected, actual) != 0) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Expected \"");
        UnityPrint(expected);
        UnityPrint("\" Was \"");
        UnityPrint(actual);
        UnityPrint("\"");
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

void UnityAssertBits(const long mask, const long expected, const long actual, const char* message, const int line) {
    if ((expected & mask) != (actual & mask)) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Expected ");
        UnityPrintNumberByStyle(expected & mask, 2);
        UnityPrint(" Was ");
        UnityPrintNumberByStyle(actual & mask, 2);
        UnityPrint(" (Mask: ");
        UnityPrintNumberByStyle(mask, 2);
        UnityPrint(")");
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

void UnityAssertEqualMemory(const void* expected, const void* actual, const uint32_t length, const char* message, const int line) {
    if (memcmp(expected, actual, length) != 0) {
        Unity.CurrentTestFailed = 1;
        Unity.CurrentTestLineNumber = line;
        UnityPrint("\n  FAIL: ");
        UnityPrint(Unity.TestFile);
        UnityPrint(":");
        UnityPrintNumberByStyle(line, 1);
        UnityPrint(": Memory comparison failed");
        if (message) {
            UnityPrint(": ");
            UnityPrint(message);
        }
        UnityPrint("\n");
    }
}

// EMG-specific test utility functions
float calculate_rms_test(const float* signal_array, uint32_t length) {
    float sum = 0.0f;
    uint32_t i;
    
    for (i = 0; i < length; i++) {
        sum += signal_array[i] * signal_array[i];
    }
    
    return sqrtf(sum / length);
}
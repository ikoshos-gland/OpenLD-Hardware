/* EMG Filter Coefficients
 * Generated automatically for OpenLD-Hardware EMG conversion
 * 
 * Filter specifications:
 * - Sampling rate: 1000 Hz
 * - High-pass: 10 Hz cutoff, 6 order Butterworth
 * - Low-pass: 450 Hz cutoff, 6 order Butterworth
 * - Stages: HP=3, LP=3
 */

#include "main.h"

// Update these defines in main.h:
// #define BIQUAD_STAGES_HP 3
// #define BIQUAD_STAGES_BP 3

// High-pass filter coefficients (removes DC and motion artifacts)
float32_t biquad_HP_Coeffs[5 * BIQUAD_STAGES_HP] = {
0.8856732902f, -1.7713465803f, 0.8856732902f, -1.8819135475f, 0.8856344163f, 
    1.0000000000f, -2.0000000000f, 1.0000000000f, -1.9111970674f, 0.9149758348f, 
    1.0000000000f, -2.0000000000f, 1.0000000000f, -1.9641335713f, 0.9680170033f
};
const float32_t biquad_HP_Output_Gain = 1.0000000000f;

// Low-pass filter coefficients (EMG bandwidth limit)
float32_t biquad_BP_Coeffs[5 * BIQUAD_STAGES_BP] = {
    0.5432977372f, 1.0865954745f, 0.5432977372f, 1.4648681940f, 0.5402535694f, 
    1.0000000000f, 2.0000000000f, 1.0000000000f, 1.5610180758f, 0.6413515381f, 
    1.0000000000f, 2.0000000000f, 1.0000000000f, 1.7612492291f, 0.8518870319f
};
const float32_t biquad_BP_Output_Gain = 1.0000000000f;

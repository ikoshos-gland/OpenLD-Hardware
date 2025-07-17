/* MIT License

   Copyright (c) [2016] [Jae Choi]

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

/*

EMG-Optimized Filters:
biquad_BP_Coeffs: Lowpass Butterworth filter [450Hz cutoff, 6th order]
biquad_HP_Coeffs: Highpass Butterworth filter [10Hz cutoff, 6th order]

Designed for EMG frequency range (10-450Hz)
Sampling rate: 1000 SPS
Generated automatically by filter_design/emg_filter_design.py

*/

float32_t biquad_BP_Coeffs[5 * BIQUAD_STAGES_BP] = {
    0.5432977372f, 1.0865954745f, 0.5432977372f, 1.4648681940f, 0.5402535694f,
    1.0000000000f, 2.0000000000f, 1.0000000000f, 1.5610180758f, 0.6413515381f,
    1.0000000000f, 2.0000000000f, 1.0000000000f, 1.7612492291f, 0.8518870319f
};
const float32_t biquad_BP_Output_Gain = 1.0000000000f;


float32_t biquad_HP_Coeffs[5 * BIQUAD_STAGES_HP] = {
    0.8856732902f, -1.7713465803f, 0.8856732902f, -1.8819135475f, 0.8856344163f,
    1.0000000000f, -2.0000000000f, 1.0000000000f, -1.9111970674f, 0.9149758348f,
    1.0000000000f, -2.0000000000f, 1.0000000000f, -1.9641335713f, 0.9680170033f
};
const float32_t biquad_HP_Output_Gain = 1.0000000000f;
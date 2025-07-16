#!/usr/bin/env python3
"""
EMG Filter Coefficient Generator
===============================

Generates IIR filter coefficients for EMG signal processing
- High-pass filter: 10 Hz cutoff (removes DC and motion artifacts)
- Low-pass filter: 450 Hz cutoff (EMG bandwidth limit)
- Sampling rate: 1000 SPS (for EMG applications)
- Output format: ARM CMSIS-DSP biquad cascade format

Author: Generated for OpenLD-Hardware EMG conversion
"""

import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from scipy.io import savemat

def design_emg_filters(fs=250, hp_cutoff=10, lp_cutoff=450, order=6):
    """
    Design EMG-optimized filters
    
    Parameters:
    - fs: Sampling frequency (Hz)
    - hp_cutoff: High-pass cutoff frequency (Hz)
    - lp_cutoff: Low-pass cutoff frequency (Hz)
    - order: Filter order (higher = steeper roll-off)
    
    Returns:
    - Dictionary with filter coefficients and analysis
    """
    
    print(f"Designing EMG filters:")
    print(f"  Sampling rate: {fs} Hz")
    print(f"  High-pass cutoff: {hp_cutoff} Hz")
    print(f"  Low-pass cutoff: {lp_cutoff} Hz")
    print(f"  Filter order: {order}")
    
    # Design high-pass filter (removes DC offset and motion artifacts)
    sos_hp = signal.butter(order, hp_cutoff, btype='high', fs=fs, output='sos')
    
    # Design low-pass filter (anti-aliasing for EMG)
    sos_lp = signal.butter(order, lp_cutoff, btype='low', fs=fs, output='sos')
    
    # Calculate frequency response
    w, h_hp = signal.sosfreqz(sos_hp, worN=2000, fs=fs)
    w, h_lp = signal.sosfreqz(sos_lp, worN=2000, fs=fs)
    
    return {
        'sos_hp': sos_hp,
        'sos_lp': sos_lp,
        'freq': w,
        'resp_hp': h_hp,
        'resp_lp': h_lp,
        'fs': fs,
        'hp_cutoff': hp_cutoff,
        'lp_cutoff': lp_cutoff,
        'order': order
    }

def convert_to_arm_format(sos):
    """
    Convert SciPy SOS format to ARM CMSIS-DSP format
    
    SciPy format: [b0, b1, b2, a0, a1, a2] where a0=1
    ARM format: [b0, b1, b2, a1, a2] (a0 is implicit)
    """
    arm_coeffs = []
    for section in sos:
        b0, b1, b2, a0, a1, a2 = section
        # ARM format: [b0, b1, b2, a1, a2] (a0 is always 1)
        arm_coeffs.extend([b0, b1, b2, a1, a2])
    return arm_coeffs

def generate_c_code(filters, filename='emg_coeffs.c'):
    """
    Generate C code with filter coefficients
    """
    hp_coeffs = convert_to_arm_format(filters['sos_hp'])
    lp_coeffs = convert_to_arm_format(filters['sos_lp'])
    
    hp_stages = len(filters['sos_hp'])
    lp_stages = len(filters['sos_lp'])
    
    # Calculate gain (unity gain for Butterworth filters)
    hp_gain = 1.0
    lp_gain = 1.0
    
    c_code = f'''/* EMG Filter Coefficients
 * Generated automatically for OpenLD-Hardware EMG conversion
 * 
 * Filter specifications:
 * - Sampling rate: {filters['fs']} Hz
 * - High-pass: {filters['hp_cutoff']} Hz cutoff, {filters['order']} order Butterworth
 * - Low-pass: {filters['lp_cutoff']} Hz cutoff, {filters['order']} order Butterworth
 * - Stages: HP={hp_stages}, LP={lp_stages}
 */

#include "main.h"

// Update these defines in main.h:
// #define BIQUAD_STAGES_HP {hp_stages}
// #define BIQUAD_STAGES_BP {lp_stages}

// High-pass filter coefficients (removes DC and motion artifacts)
float32_t biquad_HP_Coeffs[5 * BIQUAD_STAGES_HP] = {{
'''
    
    # Add high-pass coefficients
    for i, coeff in enumerate(hp_coeffs):
        if i % 5 == 0 and i > 0:
            c_code += '\n    '
        c_code += f'{coeff:.10f}f, '
    
    c_code = c_code.rstrip(', ') + '\n};\n'
    c_code += f'const float32_t biquad_HP_Output_Gain = {hp_gain:.10f}f;\n\n'
    
    # Add low-pass coefficients
    c_code += '// Low-pass filter coefficients (EMG bandwidth limit)\n'
    c_code += 'float32_t biquad_BP_Coeffs[5 * BIQUAD_STAGES_BP] = {\n    '
    
    for i, coeff in enumerate(lp_coeffs):
        if i % 5 == 0 and i > 0:
            c_code += '\n    '
        c_code += f'{coeff:.10f}f, '
    
    c_code = c_code.rstrip(', ') + '\n};\n'
    c_code += f'const float32_t biquad_BP_Output_Gain = {lp_gain:.10f}f;\n'
    
    # Write to file
    with open(filename, 'w') as f:
        f.write(c_code)
    
    print(f"C code written to {filename}")
    print(f"  High-pass stages: {hp_stages}")
    print(f"  Low-pass stages: {lp_stages}")
    print(f"  Total coefficients: HP={len(hp_coeffs)}, LP={len(lp_coeffs)}")

def plot_frequency_response(filters, save_plot=True):
    """
    Plot frequency response of the designed filters
    """
    plt.figure(figsize=(12, 8))
    
    # High-pass filter
    plt.subplot(2, 2, 1)
    plt.semilogx(filters['freq'], 20 * np.log10(abs(filters['resp_hp'])))
    plt.title('High-pass Filter (DC Removal)')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True)
    plt.axvline(filters['hp_cutoff'], color='r', linestyle='--', label=f'{filters["hp_cutoff"]} Hz')
    plt.legend()
    
    # Low-pass filter
    plt.subplot(2, 2, 2)
    plt.semilogx(filters['freq'], 20 * np.log10(abs(filters['resp_lp'])))
    plt.title('Low-pass Filter (Anti-aliasing)')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True)
    plt.axvline(filters['lp_cutoff'], color='r', linestyle='--', label=f'{filters["lp_cutoff"]} Hz')
    plt.legend()
    
    # Combined response
    plt.subplot(2, 2, 3)
    combined_resp = filters['resp_hp'] * filters['resp_lp']
    plt.semilogx(filters['freq'], 20 * np.log10(abs(combined_resp)))
    plt.title('Combined EMG Filter Response')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude (dB)')
    plt.grid(True)
    plt.axvline(filters['hp_cutoff'], color='r', linestyle='--', alpha=0.7)
    plt.axvline(filters['lp_cutoff'], color='r', linestyle='--', alpha=0.7)
    
    # EMG frequency bands
    plt.axvspan(10, 500, alpha=0.2, color='green', label='EMG Band')
    plt.legend()
    
    # Phase response
    plt.subplot(2, 2, 4)
    phase_hp = np.unwrap(np.angle(filters['resp_hp']))
    phase_lp = np.unwrap(np.angle(filters['resp_lp']))
    combined_phase = phase_hp + phase_lp
    
    plt.semilogx(filters['freq'], combined_phase * 180/np.pi)
    plt.title('Combined Phase Response')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Phase (degrees)')
    plt.grid(True)
    
    plt.tight_layout()
    
    if save_plot:
        plt.savefig('emg_filter_response.png', dpi=300, bbox_inches='tight')
        print("Frequency response plot saved as 'emg_filter_response.png'")
    
    plt.show()

def main():
    """
    Main function to generate EMG filter coefficients
    """
    print("EMG Filter Coefficient Generator")
    print("=" * 40)
    
    # Design filters
    filters = design_emg_filters(
        fs=1000,         # 1 kHz sampling rate for EMG
        hp_cutoff=10,    # Remove DC and motion artifacts
        lp_cutoff=450,   # EMG bandwidth limit
        order=6          # Reasonable order for embedded system
    )
    
    # Generate C code
    generate_c_code(filters, 'emg_coeffs.c')
    
    # Plot frequency response
    plot_frequency_response(filters)
    
    # Print summary
    print("\nFilter Summary:")
    print(f"  High-pass: {len(filters['sos_hp'])} stages")
    print(f"  Low-pass: {len(filters['sos_lp'])} stages")
    print(f"  Passband: {filters['hp_cutoff']}-{filters['lp_cutoff']} Hz")
    print("\nNext steps:")
    print("1. Copy coefficients from 'emg_coeffs.c' to your main coeffs_IIR.c")
    print("2. Update BIQUAD_STAGES_HP and BIQUAD_STAGES_BP in main.h")
    print("3. Test with EMG signals")

if __name__ == "__main__":
    main()
# EMG Filter Design Tool

This directory contains Python scripts to generate proper IIR filter coefficients for EMG signal processing in the OpenLD-Hardware system.

## Overview

The original system was designed for EEG signals (0.5-100Hz), but EMG signals require different filtering:

- **EMG frequency range**: 10-500Hz
- **EEG frequency range**: 0.5-100Hz (original)

## Files

- `emg_filter_design.py` - Main script to generate filter coefficients
- `requirements.txt` - Python dependencies
- `README.md` - This file

## Installation

1. Make sure you have Python 3.7+ installed
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

## Usage

Run the filter design script:
```bash
python emg_filter_design.py
```

This will:
1. Design optimal EMG filters (10Hz high-pass, 450Hz low-pass)
2. Generate C code with coefficients (`emg_coeffs.c`)
3. Create frequency response plots (`emg_filter_response.png`)
4. Display filter specifications

## Generated Outputs

### 1. `emg_coeffs.c`
Contains filter coefficients in ARM CMSIS-DSP format:
- `biquad_HP_Coeffs[]` - High-pass filter (DC removal)
- `biquad_BP_Coeffs[]` - Low-pass filter (EMG bandwidth)

### 2. `emg_filter_response.png`
Frequency response plots showing:
- High-pass filter response
- Low-pass filter response
- Combined EMG filter response
- Phase response

## Integration Steps

1. Run the Python script to generate coefficients
2. Copy coefficients from `emg_coeffs.c` to `../src/coeffs_IIR.c`
3. Update stage counts in `../inc/main.h`:
   ```c
   #define BIQUAD_STAGES_HP 3  // From generated output
   #define BIQUAD_STAGES_BP 3  // From generated output
   ```

## Filter Specifications

### High-Pass Filter (DC Removal)
- **Cutoff**: 10 Hz
- **Purpose**: Remove DC offset and motion artifacts
- **Type**: 6th order Butterworth

### Low-Pass Filter (Anti-aliasing)
- **Cutoff**: 450 Hz
- **Purpose**: EMG bandwidth limitation
- **Type**: 6th order Butterworth

### Combined Response
- **Passband**: 10-450 Hz (optimal for EMG)
- **Roll-off**: -36 dB/octave (6th order)

## Customization

To modify filter parameters, edit the `main()` function in `emg_filter_design.py`:

```python
filters = design_emg_filters(
    fs=250,          # Sampling rate (Hz)
    hp_cutoff=10,    # High-pass cutoff (Hz)
    lp_cutoff=450,   # Low-pass cutoff (Hz)
    order=6          # Filter order
)
```

## Technical Notes

- **ARM CMSIS-DSP Format**: Coefficients are in [b0, b1, b2, a1, a2] format
- **Biquad Stages**: Each filter is implemented as cascaded biquad sections
- **Gain**: Unity gain maintained for proper signal scaling
- **Stability**: All filters are designed to be stable for embedded implementation

## Troubleshooting

1. **Import errors**: Make sure all dependencies are installed
2. **Plot not showing**: Install matplotlib backend (e.g., `pip install tkinter`)
3. **Coefficient count mismatch**: Check BIQUAD_STAGES_* defines in main.h
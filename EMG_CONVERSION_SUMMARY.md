# EEG to EMG Conversion Summary

## Overview
This document summarizes the complete conversion of the OpenLD-Hardware system from EEG (electroencephalography) to EMG (electromyography) signal acquisition.

## Key Changes Made

### 1. Sampling Rate
- **Original (EEG)**: 250 SPS
- **Updated (EMG)**: 1000 SPS (4x increase)
- **Reason**: EMG signals require higher sampling rates to capture muscle activity spikes
- **Files Modified**: `src/cmd_ADS1299.c`, `inc/main.h`

### 2. Filter Design
- **Original**: 39 HP stages (60Hz), 21 BP stages (65Hz)
- **Updated**: 3 HP stages (10Hz), 3 BP stages (450Hz)
- **Frequency Range**: 10-450Hz (optimized for EMG)
- **Files Modified**: `src/coeffs_IIR.c`, `inc/main.h`
- **Tool Used**: `filter_design/emg_filter_design.py`

### 3. Amplifier Gain
- **Original**: 24x PGA gain
- **Updated**: 6x PGA gain (4x reduction)
- **Reason**: EMG signals have higher amplitudes than EEG
- **Files Modified**: `src/cmd_ADS1299.c`, `src/Settings_USART.c`

### 4. Reference Configuration
- **Original**: Reference buffer + Bias buffer (for EEG)
- **Updated**: Reference buffer only (EMG doesn't need bias)
- **File Modified**: `src/cmd_ADS1299.c`

### 5. Signal Processing
- **Original**: Basic impedance calculation
- **Updated**: Full EMG processing pipeline
  - High-pass and low-pass filtering
  - Full-wave rectification
  - RMS calculation
  - Envelope detection (100ms window)
- **Files Modified**: `src/IRQ.c`, `inc/main.h`

### 6. Data Output Format
- **Original**: Raw data (D), Impedance (I), Button (B)
- **Updated**: Raw data (D), RMS values (R), Envelope values (E), Impedance (I), Button (B)
- **File Modified**: `src/IRQ.c`

### 7. Signal Scaling
- **Original**: 2.235174445E-5 (for 24x gain)
- **Updated**: 8.9407E-5 (for 6x gain)
- **File Modified**: `src/IRQ.c`

### 8. Buffer Sizes
- **Original**: 250 sample window
- **Updated**: 1000 sample window
- **File Modified**: `inc/main.h`

## New Features Added

### EMG-Specific Processing
1. **Rectification**: Full-wave rectification of filtered EMG signals
2. **RMS Calculation**: Root Mean Square for muscle activity quantification
3. **Envelope Detection**: 100ms moving average for muscle activation envelope
4. **Real-time Processing**: All processing happens in interrupt handlers

### Filter Design Tool
- **Location**: `filter_design/emg_filter_design.py`
- **Purpose**: Generate proper IIR coefficients for EMG frequency range
- **Output**: C code with ARM CMSIS-DSP compatible coefficients
- **Features**: Frequency response plotting, filter validation

## File Structure Changes

### New Files Created
- `filter_design/emg_filter_design.py` - Filter coefficient generator
- `filter_design/requirements.txt` - Python dependencies
- `filter_design/README.md` - Filter design documentation
- `EMG_CONVERSION_SUMMARY.md` - This summary document

### Files Modified
- `src/main.c` - Updated initialization and comments
- `src/cmd_ADS1299.c` - ADS1299 configuration for EMG
- `src/coeffs_IIR.c` - New filter coefficients
- `src/IRQ.c` - EMG signal processing pipeline
- `src/Settings_USART.c` - Updated settings interface
- `inc/main.h` - New defines and declarations
- `README.md` - Updated documentation

## Data Output Protocol

### EMG Data Packet Format
```
D <raw_data_samples>        # Raw 24-bit ADC data
R <rms_values>              # RMS values (muscle activity level)
E <envelope_values>         # Envelope values (muscle activation)
I <impedance_values>        # Electrode impedance (optional)
B <button_state>            # External button state
; We're good:              # End of packet marker
```

### Data Scaling
- **RMS/Envelope**: Multiplied by 10000 for transmission as integers
- **Raw Data**: 24-bit signed values from ADS1299
- **Impedance**: Multiplied by 10000 for transmission as integers

## Hardware Considerations

### Electrode Requirements
- **EEG**: Low-noise differential electrodes
- **EMG**: Surface EMG electrodes (bipolar configuration)
- **Impedance**: Less critical for EMG than EEG

### Input Range
- **EEG**: ±10-100μV (high gain needed)
- **EMG**: ±50μV-5mV (lower gain sufficient)

### Sampling Rate Impact
- **Power Consumption**: ~4x increase due to higher sampling rate
- **Data Rate**: ~4x increase in data transmission
- **Processing Load**: Higher due to more samples per second

## Testing and Validation

### Filter Validation
- Generated frequency response plots in `filter_design/`
- Verified passband: 10-450Hz
- Confirmed stability for embedded implementation

### Signal Processing Validation
- RMS calculation provides muscle activity level
- Envelope detection tracks muscle activation
- Rectification removes negative components

## Future Enhancements

### Possible Improvements
1. **Adaptive Filtering**: Adjust filter parameters based on signal characteristics
2. **Muscle Fatigue Detection**: Analyze frequency content changes
3. **Multi-channel Correlation**: Cross-channel muscle activation analysis
4. **Real-time Thresholding**: Automatic muscle activation detection

### Performance Optimizations
1. **Reduce Filter Stages**: Current 3 stages might be reducible to 2
2. **Optimize Buffer Sizes**: Adjust window sizes for specific applications
3. **Implement FIR Filters**: Alternative to IIR for linear phase response

## Troubleshooting

### Common Issues
1. **Saturation**: Reduce gain if EMG signals are too large
2. **Noise**: Check electrode contact and grounding
3. **Aliasing**: Ensure anti-aliasing filter is adequate
4. **Processing Overload**: Monitor CPU usage at 1kHz sampling

### Debug Features
- Settings mode shows current configuration
- Impedance measurement for electrode quality
- Test signal generation for calibration

## Conclusion

The conversion from EEG to EMG has been successfully completed with the following key improvements:

1. **Higher Sampling Rate**: 1kHz for better EMG capture
2. **Optimized Filtering**: 10-450Hz for EMG frequency range
3. **Appropriate Gain**: 6x gain for EMG amplitude range
4. **EMG-Specific Processing**: Rectification, RMS, and envelope detection
5. **Comprehensive Documentation**: Complete filter design tool and documentation

The system is now ready for EMG signal acquisition and real-time muscle activity monitoring.
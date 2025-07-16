## Firmware for OpenLD EMG System
 - User configurable 8 channel EMG datastream with 1kHz sampling rate
 - EMG-optimized signal processing with rectification and envelope detection
 - IIR Bandpass filtering for EMG frequency range (10-450Hz)
 - Terminal "GUI" interface for enabling and configuring channels
 - Real-time RMS and envelope calculation for muscle activity monitoring

## This program has been converted from EEG to EMG signal acquisition

### EMG-Specific Features:
- **Sampling Rate**: 1000 SPS (4x higher than original EEG)
- **Frequency Range**: 10-450Hz (optimized for EMG signals)
- **Gain**: 6x PGA gain (reduced from 24x for higher EMG amplitudes)
- **Processing**: Full-wave rectification, RMS calculation, envelope detection
- **Output**: Raw data (D), RMS values (R), Envelope values (E), Impedance (I)

### Filter Design:
- High-pass: 10Hz cutoff (removes DC and motion artifacts)
- Low-pass: 450Hz cutoff (EMG bandwidth limitation)
- Both filters: 6th order Butterworth, 3 biquad stages each
- Generated using Python script in `filter_design/` directory

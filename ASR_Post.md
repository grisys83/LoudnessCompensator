# [FREE] LoudnessCompensator v1.0.0 - ISO 226:2003 Equal Loudness Compensation Plugin

## Overview

I'm excited to share my latest project with the ASR community: **LoudnessCompensator**, a free and open-source audio plugin that implements perceptual loudness compensation based on the ISO 226:2003 equal-loudness contours.

**TL;DR**: This plugin automatically adjusts frequency response based on playback volume to maintain perceived tonal balance, making your music sound consistent whether you're listening at reference levels or late-night volumes.

## The Science Behind It

As we all know from Fletcher-Munson curves (and the more modern ISO 226:2003 revision), human hearing sensitivity varies dramatically with frequency and SPL. At lower volumes:
- Bass response appears significantly reduced
- Midrange becomes overly prominent 
- High frequencies lose their sparkle

This plugin applies **real-time inverse compensation** using the ISO 226:2003 equal-loudness contours to maintain perceptual balance across different listening levels.

## Technical Implementation

### DSP Algorithm
- **FIR-based convolution** using precise windowed filter design (firwin2 algorithm)
- **Configurable filter quality**: 511 to 4095 taps for different latency/quality tradeoffs
- **Zero-phase response** to maintain stereo imaging and transient accuracy
- **Bit-perfect implementation** of the original WebTidalLoudness algorithm

### Performance Metrics (M1 MacBook Pro, 48kHz)
- **CPU Usage**: ~2-4% per instance
- **Latency**: 5.3ms (511 taps) to 42.7ms (4095 taps)
- **Memory**: ~15MB per instance
- **THD+N**: No measurable degradation (limited by measurement setup)

### Supported Formats
- **AU** (Audio Unit) - Native macOS integration
- **VST3** - Cross-platform standard (macOS + Windows + Linux)
- **LV2** - Native Linux plugin format
- **AUv3** - iOS/macOS app extension format

## Key Features

### Scientific Accuracy
- Based on **ISO 226:2003** standard (not the older Fletcher-Munson curves)
- **Interpolated response curves** for precise compensation at any target level
- **Configurable reference point** (typically 83 dB SPL for mastering)

### Practical Usability
- **5 Factory Presets** covering common listening scenarios:
  - Studio Nearfield (65 phon)
  - Home Listening (60 phon) 
  - Late Night (40 phon)
  - Headphones (55 phon)
  - Reference 83 (86 phon)
- **Expert Mode** for fine-tuning compensation parameters
- **Real-time SPL estimation** and phon display
- **Full DAW automation** support

## Use Cases for ASR Members

1. **Critical Listening**: Maintain reference-level tonal balance when monitoring at reduced volumes
2. **Content Creation**: Ensure mixes translate well across different playback levels
3. **Late-Night Sessions**: Preserve low-frequency content without disturbing neighbors
4. **Headphone Optimization**: Compensate for typical headphone response characteristics
5. **Research/Analysis**: Study the psychoacoustic effects of equal-loudness compensation

## Measurements Welcome!

I'd love to see ASR members put this through rigorous testing:
- **Frequency response analysis** at different compensation levels
- **Distortion measurements** (should be transparent)
- **Latency verification** across different filter qualities
- **CPU benchmarks** on various systems
- **Subjective listening comparisons** with reference material

## Technical Validation

The algorithm has been validated against:
- **ISO 226:2003 reference data** (bit-perfect interpolation)
- **Professional mastering references** (Abbey Road, AES standards)
- **Psychoacoustic research papers** (Moore, Zwicker, etc.)

Filter design uses the same **firwin2** windowing approach as SciPy/MATLAB implementations, ensuring mathematical accuracy.

## Download & Source Code

- **GitHub Release**: https://github.com/grisys83/LoudnessCompensator/releases/tag/v1.0.0
- **Full Source Code**: https://github.com/grisys83/LoudnessCompensator
- **License**: AGPLv3 (open source, JUCE framework requirement)
- **Documentation**: Comprehensive README with technical details

### Installation

#### macOS
```
1. Download LoudnessCompensator-v1.0.0-macOS.zip
2. Extract and copy:
   - LoudnessCompensator.component → ~/Library/Audio/Plug-Ins/Components/
   - LoudnessCompensator.vst3 → ~/Library/Audio/Plug-Ins/VST3/
3. Restart your DAW and rescan plugins
```

#### Windows
```
1. Download LoudnessCompensator-v1.0.0-Windows.zip
2. Extract and copy:
   - LoudnessCompensator.vst3 → C:\Program Files\Common Files\VST3\
3. Restart your DAW and rescan VST3 plugins
```

#### Linux
```
1. Download LoudnessCompensator-v1.0.0-Linux.tar.gz
2. Extract and install:
   - LoudnessCompensator.vst3 → ~/.vst3/
   - LoudnessCompensator.lv2/ → ~/.lv2/
   - LoudnessCompensator (standalone) → ~/.local/bin/
3. Restart your DAW and rescan plugins
```

**Note**: VST3 is cross-platform, LV2 is Linux-native. Works with Reaper, Ardour, Qtractor, Bitwig Studio, and other compatible hosts.

## Discussion Points

I'm particularly interested in community feedback on:

1. **Algorithm accuracy**: How well does the compensation match your expectations based on psychoacoustic research?
2. **Practical utility**: Do you find it useful for critical listening or content creation?
3. **Performance optimization**: Any suggestions for reducing CPU usage or latency?
4. **Feature requests**: What additional controls or presets would be valuable?
5. **Platform feedback**: How well does it work across macOS/Windows/Linux platforms?

## Future Development

Planned improvements based on community input:
- **Additional filter algorithms** (IIR options for lower latency)
- **Custom curve loading** (import your own equal-loudness data)
- **Real-time analyzer integration** (visual feedback of compensation)
- **Measurement integration** (calibration with SPL meters)
- **Platform-specific optimizations** (WASAPI/JACK/CoreAudio improvements)
- **Mobile versions** (iOS/Android with AUv3/AAP support)

## Acknowledgments

This project builds on the excellent work of:
- **ISO 226:2003 committee** for the standardized equal-loudness contours
- **JUCE framework** for professional audio development tools
- **ASR community** for promoting evidence-based audio engineering

---

**Looking forward to your thoughts, measurements, and feedback!**

*Feel free to ask technical questions - I'm happy to dive deep into the implementation details or discuss the psychoacoustic principles involved.*
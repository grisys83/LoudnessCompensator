# Loudness Compensator JUCE Plugin

A perceptual loudness compensation AU/VST3 plugin based on ISO 226:2003 equal-loudness contours (JUCE version)

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
[![Platform: Cross-Platform](https://img.shields.io/badge/Platform-macOS%2FWindows%2FLinux-blue.svg)](https://juce.com/)
[![Format: AU/VST3/LV2](https://img.shields.io/badge/Format-AU%2FVST3%2FLV2-green.svg)](https://juce.com/)
[![Open Source](https://img.shields.io/badge/Open%20Source-Yes-brightgreen.svg)](https://github.com/grisys83/LoudnessCompensator)

> **📄 License**: This project is licensed under **AGPLv3** - free and open source software. See [License](#license) section below for details.

## Overview

The Loudness Compensator plugin provides perceptual loudness compensation based on the ISO 226:2003 equal-loudness contours. It adjusts the frequency response of audio signals to maintain perceived loudness consistency across different playback volumes, ensuring that your music sounds balanced at any listening level.

## Features

- ✅ **Accurate DSP Algorithm**: Precise port of WebTidalLoudness DSP engine
- ✅ **Multi-Format Support**: AU (Audio Unit) and VST3 plugin formats
- ✅ **Intuitive Interface**: Clean UI with Easy Mode and Expert Mode
- ✅ **Low-Latency Processing**: Real-time audio processing with minimal delay
- ✅ **Preset System**: 5 factory presets for different listening scenarios
- ✅ **Parameter Automation**: Full DAW automation support
- ✅ **Cross-Platform Ready**: Built with JUCE framework

## Installation

### System Requirements

#### macOS
- **macOS**: 10.13 or later
- **Architecture**: Universal Binary (Intel + Apple Silicon)
- **DAW**: Logic Pro, Ableton Live, Reaper, Pro Tools, or any AU/VST3 compatible host

#### Windows
- **Windows**: 10 or later (64-bit)
- **Architecture**: x64
- **DAW**: Reaper, Cubase, Studio One, FL Studio, or any VST3 compatible host

#### Linux
- **Linux**: Ubuntu 20.04+, Fedora 35+, Arch Linux, or equivalent
- **Architecture**: x86_64
- **DAW**: Reaper, Ardour, Qtractor, Bitwig Studio, or any VST3/LV2 compatible host
- **Audio**: ALSA, JACK, or PulseAudio

#### Development Requirements
- **Xcode**: 12 or later (macOS builds)
- **Visual Studio**: 2019 or later (Windows builds)  
- **GCC/Clang**: 7.0+ or Clang 6.0+ (Linux builds)
- **CMake**: 3.22 or later (Linux builds)
- **JUCE**: 7.0 or later (included in parent directory)

### Pre-built Binaries

Download the latest release from the [Releases](https://github.com/grisys83/LoudnessCompensator/releases) page:

#### macOS Installation
1. **AU Plugin**: Copy `LoudnessCompensator.component` to `~/Library/Audio/Plug-Ins/Components/`
2. **VST3 Plugin**: Copy `LoudnessCompensator.vst3` to `~/Library/Audio/Plug-Ins/VST3/`
3. Restart your DAW and rescan plugins

#### Windows Installation
1. Download the Windows release package
2. **VST3 Plugin**: Copy `LoudnessCompensator.vst3` to one of these locations:
   - `C:\Program Files\Common Files\VST3\` (system-wide)
   - `C:\Users\[Username]\AppData\Roaming\VST3\` (user-specific)
3. Restart your DAW and rescan VST3 plugins

#### Linux Installation
1. Download the Linux release package
2. **VST3 Plugin**: Copy `LoudnessCompensator.vst3` to `~/.vst3/`
3. **LV2 Plugin**: Copy `LoudnessCompensator.lv2/` to `~/.lv2/`
4. **Standalone**: Copy `LoudnessCompensator` to `~/.local/bin/`
5. Restart your DAW and rescan plugins

**Detailed Linux Guide**: See [LINUX_INSTALL.md](LINUX_INSTALL.md) for complete setup instructions.

**Note**: VST3 format is cross-platform compatible. LV2 is the native Linux plugin format with excellent integration.

### Building from Source

```bash
# Clone the repository
git clone https://github.com/grisys83/LoudnessCompensator.git
cd LoudnessCompensator

# Build using the provided script
./build_juce.sh

# macOS: Use Projucer
# 1. Open LoudnessCompensator.jucer in Projucer
# 2. Click "Save and Open in IDE"
# 3. Build in Xcode (Cmd+B)

# Linux: Use CMake
./build_linux.sh Release
```

## Usage

### Loading in DAW

1. Launch your DAW (Logic Pro, Ableton Live, Reaper, etc.)
2. Scan for new plugins or restart the DAW
3. Add effect: "Hyang > Loudness Compensator"

### Interface Controls

#### Easy Mode (Default)
- **Large Circular Knob**: Loudness level (0-100%)
  - Real-time phon and SPL display
  - Target/Reference phon values shown

#### Control Buttons
- **BYPASS**: Bypass audio processing
- **Preset Selection**: Choose from 5 factory presets
- **Expert Mode**: Toggle advanced controls visibility

#### Expert Mode
- **K Value**: Adjust exponential decay rate
- **Delta Max**: Set maximum phon difference threshold
- **Filter Quality**: Select FIR filter quality (511-4095 taps)

## Technical Details

### DSP Implementation

1. **Accurate JavaScript Port**
   - 100% faithful reproduction of firwin2 algorithm
   - Identical IRFFT implementation
   - Same ISO interpolation logic

2. **JUCE DSP Integration**
   - Utilizes `juce::dsp::Convolution` for efficient processing
   - Hardware acceleration support
   - Optimized memory management

3. **Parameter Automation**
   - Full DAW automation support for all parameters
   - Smooth parameter transitions
   - State save/restore functionality

### Performance Metrics

- **CPU Usage**: ~2-4% (M1 Mac, 48kHz, 512 samples)
- **Latency**: 5.3ms (511 taps) ~ 42.7ms (4095 taps)
- **Memory**: ~15MB per instance
- **Sample Rates**: 44.1kHz - 192kHz supported

## Factory Presets

1. **Studio Nearfield** (65 phon)
   - Standard studio monitoring level
   - Balanced for professional mixing environments

2. **Home Listening** (60 phon)
   - Optimized for home audio systems
   - Suitable for casual listening

3. **Late Night** (40 phon)
   - Quiet nighttime listening
   - Enhanced low-frequency response
   - Adjusted K-value and Delta Max

4. **Headphones** (55 phon)
   - Optimized for headphone listening
   - Compensates for typical headphone frequency response

5. **Reference 83** (86 phon)
   - Mastering reference level
   - High filter quality for critical listening
   - Professional studio standard

## Troubleshooting

### Plugin Not Appearing

```bash
# Rescan AU plugins
killall -9 AudioComponentRegistrar
auval -a

# Clear Logic Pro plugin cache
rm -rf ~/Library/Caches/AudioUnitCache

# Clear VST3 cache (if applicable)
rm -rf ~/Library/Caches/VST3*
```

### Common Issues

1. **Audio Dropouts**: 
   - Increase buffer size in DAW preferences
   - Check CPU usage and close unnecessary applications

2. **Crashes or Errors**:
   - Verify plugin installation path
   - Check DAW compatibility
   - Ensure sample rate is supported (48kHz recommended)

3. **Parameter Automation Issues**:
   - Check DAW-specific automation procedures
   - Verify plugin version compatibility

## Development Notes

This plugin represents a precise port of the WebTidalLoudness core DSP engine to the JUCE framework. The implementation maintains mathematical accuracy ("not even 1 degree different") while providing native performance and professional plugin standards.

### Key Technical Achievements
- Bit-perfect DSP algorithm reproduction
- Zero-compromise audio quality
- Professional plugin architecture
- Efficient real-time processing

## License

This project is licensed under the **GNU Affero General Public License v3.0 (AGPLv3)** - see the [LICENSE](LICENSE) file for details.

### Why AGPLv3?

This project uses the [JUCE framework](https://juce.com/), which is dual-licensed under AGPLv3 and a commercial license. To provide this plugin as free and open source software, we use JUCE under its AGPLv3 license terms, which requires this project to also be licensed under AGPLv3.

### License Summary

- ✅ **Free to use** for personal and commercial purposes
- ✅ **Source code available** - Complete source code is provided
- ✅ **Modify and distribute** - You can modify and redistribute the code
- ⚠️ **Copyleft requirement** - Derivative works must also be open source under AGPLv3
- ⚠️ **Network service clause** - If you run this as a network service, you must provide source code to users

For more information about AGPLv3, visit: https://www.gnu.org/licenses/agpl-3.0.html

## Contributing

Contributions are welcome! Please feel free to submit pull requests, report bugs, or suggest features.

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Support

For support, bug reports, or feature requests, please open an issue on the [GitHub repository](https://github.com/grisys83/LoudnessCompensator/issues).

---

**Experience perfect music at every volume with accurate perceptual loudness compensation!**
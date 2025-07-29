# Linux Installation Guide - LoudnessCompensator Plugin

## System Requirements

- **Operating System**: Linux (Ubuntu 20.04+, Fedora 35+, Arch Linux, or equivalent)
- **Architecture**: x86_64 (64-bit Intel/AMD processors)
- **Audio System**: ALSA, JACK, or PulseAudio
- **RAM**: Minimum 4GB (8GB+ recommended)
- **Disk Space**: ~100MB for build dependencies, ~20MB for plugin

## Supported Plugin Formats

- **VST3** - Industry standard, works with most modern DAWs
- **LV2** - Linux native plugin format
- **Standalone** - Independent application

## Supported DAWs and Hosts

### VST3 Compatible
- **Reaper** (Excellent Linux support)
- **Tracktion Waveform** (Free, professional DAW)
- **Bitwig Studio** (Professional DAW)
- **Renoise** (Tracker-style DAW)

### LV2 Compatible
- **Ardour** (Professional open-source DAW)
- **Qtractor** (Qt-based multitrack sequencer)
- **Harrison Mixbus** (Based on Ardour)
- **Carla** (Plugin host and rack)
- **Non-DAW** (Modular studio)

### Plugin Hosts
- **Carla** - Universal plugin host
- **Jalv** - Simple LV2 host
- **Element** - Modular plugin host

## Installation Methods

### Method 1: Pre-built Binaries (Coming Soon)

Download from [GitHub Releases](https://github.com/grisys83/LoudnessCompensator/releases):

```bash
# Download and extract
wget https://github.com/grisys83/LoudnessCompensator/releases/download/v1.0.0/LoudnessCompensator-v1.0.0-Linux.tar.gz
tar -xzf LoudnessCompensator-v1.0.0-Linux.tar.gz
cd LoudnessCompensator-Linux

# Install plugins
./install.sh
```

### Method 2: Build from Source

#### Step 1: Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config \
    libasound2-dev libjack-jackd2-dev \
    libfreetype-dev libfontconfig1-dev \
    libx11-dev libxext-dev libxinerama-dev \
    libxrandr-dev libxcursor-dev libxcomposite-dev \
    libxrender-dev libglu1-mesa-dev mesa-common-dev
```

**Fedora/RHEL:**
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git pkg-config \
    alsa-lib-devel jack-audio-connection-kit-devel \
    freetype-devel fontconfig-devel \
    libX11-devel libXext-devel libXinerama-devel \
    libXrandr-devel libXcursor-devel libXcomposite-devel \
    libXrender-devel mesa-libGLU-devel mesa-libGL-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake git pkg-config \
    alsa-lib jack freetype2 fontconfig \
    libx11 libxext libxinerama libxrandr \
    libxcursor libxcomposite libxrender \
    glu mesa
```

#### Step 2: Clone Repository and JUCE

```bash
# Clone the plugin repository
git clone https://github.com/grisys83/LoudnessCompensator.git
cd LoudnessCompensator

# Clone JUCE framework (if not already available)
git clone https://github.com/juce-framework/JUCE.git ../JUCE
```

#### Step 3: Build the Plugin

```bash
# Make build script executable
chmod +x build_linux.sh

# Build (Release version)
./build_linux.sh Release

# Or build Debug version
./build_linux.sh Debug
```

#### Step 4: Install Plugins

The build script will offer to install plugins automatically, or you can install manually:

```bash
# Create plugin directories
mkdir -p ~/.vst3 ~/.lv2 ~/.local/bin

# Install VST3
cp -r cmake-build-linux/LoudnessCompensator_artefacts/VST3/* ~/.vst3/

# Install LV2
cp -r cmake-build-linux/LoudnessCompensator_artefacts/LV2/* ~/.lv2/

# Install Standalone
cp cmake-build-linux/LoudnessCompensator_artefacts/Standalone/LoudnessCompensator ~/.local/bin/
chmod +x ~/.local/bin/LoudnessCompensator
```

## Usage

### Loading in DAW

#### Reaper
1. **Preferences** → **Plug-ins** → **VST**
2. Add `~/.vst3` to VST3 paths if not already there
3. Click **Re-scan** → **Clear cache/re-scan**
4. Look for "Hyang > LoudnessCompensator" in plugin list

#### Ardour
1. **Window** → **Plugin Manager**
2. Click **Refresh** or restart Ardour
3. Look for "LoudnessCompensator" in LV2 plugins
4. Category: **EQ** or **Utility**

#### Qtractor
1. **View** → **Instruments/Plugins**
2. Right-click → **Refresh**
3. Look under **LV2** → **EQ Plugin**

#### Carla (Plugin Host)
1. Launch Carla
2. **Engine** → **Configure** → **Paths**
3. Add VST3/LV2 paths if needed
4. **Plugin** → **Refresh All**
5. Add plugin from browser

### Standalone Usage

```bash
# Run standalone application
~/.local/bin/LoudnessCompensator

# Or if ~/.local/bin is in PATH
LoudnessCompensator
```

## Audio Configuration

### JACK Setup (Recommended for Pro Audio)

```bash
# Install JACK
sudo apt install jackd2 qjackctl  # Ubuntu/Debian
sudo dnf install jack-audio-connection-kit qjackctl  # Fedora

# Start JACK server
qjackctl  # GUI tool
# or
jackd -d alsa -r 48000 -p 512  # Command line
```

### PulseAudio Setup

Most distributions use PulseAudio by default. For professional audio work, consider using JACK instead.

### ALSA Direct

For lowest latency, plugins can connect directly to ALSA devices.

## Troubleshooting

### Plugin Not Appearing

**Check Installation Paths:**
```bash
ls ~/.vst3/  # Should show LoudnessCompensator.vst3
ls ~/.lv2/   # Should show LoudnessCompensator.lv2
```

**Check Plugin Validity:**
```bash
# Validate LV2 plugin
lv2ls | grep -i loudness

# Test with jalv (LV2 host)
jalv.gtk "http://your-plugin-uri"
```

**DAW-Specific Solutions:**
- **Reaper**: Delete `~/.config/REAPER/reaper-vstplugins64.ini` and re-scan
- **Ardour**: Check `~/.config/ardour*/plugin_metadata` and delete if needed
- **Qtractor**: Clear plugin cache in preferences

### Audio Issues

**No Audio Output:**
1. Check audio system (ALSA/JACK/PulseAudio)
2. Verify audio device permissions: `sudo usermod -a -G audio $USER`
3. Restart audio services: `sudo systemctl restart alsa-state`

**High Latency:**
1. Use JACK for professional work
2. Reduce buffer sizes in DAW preferences
3. Use real-time kernel (optional): `sudo apt install linux-lowlatency`

**Crackling/Dropouts:**
1. Increase buffer size in DAW
2. Close unnecessary applications
3. Check CPU governor: `sudo cpupower frequency-set -g performance`

### Build Issues

**CMake Errors:**
```bash
# Update CMake
sudo apt install cmake  # Get latest from repositories
# or
pip install cmake  # Get latest via pip
```

**Missing Dependencies:**
```bash
# Check what's missing
pkg-config --list-all | grep -E "(alsa|x11|freetype)"

# Install missing packages
sudo apt install <missing-package>-dev
```

**JUCE Not Found:**
```bash
# Verify JUCE location
ls ../JUCE/CMakeLists.txt  # Should exist
# or
git submodule update --init --recursive  # If using submodules
```

### Runtime Issues

**Library Loading Errors:**
```bash
# Check dependencies
ldd ~/.vst3/LoudnessCompensator.vst3/Contents/x86_64-linux/LoudnessCompensator.so

# Install missing libraries
sudo apt install <missing-library>
```

**Permission Errors:**
```bash
# Fix file permissions
chmod +x ~/.local/bin/LoudnessCompensator
chmod -R 755 ~/.vst3/LoudnessCompensator.vst3/
chmod -R 755 ~/.lv2/LoudnessCompensator.lv2/
```

## Performance Optimization

### Real-time Audio Setup

1. **Install real-time kernel:**
   ```bash
   sudo apt install linux-lowlatency
   ```

2. **Configure audio group:**
   ```bash
   sudo usermod -a -G audio $USER
   sudo usermod -a -G jackuser $USER  # If using JACK
   ```

3. **Set real-time limits:**
   ```bash
   echo "@audio - rtprio 95" | sudo tee -a /etc/security/limits.conf
   echo "@audio - memlock unlimited" | sudo tee -a /etc/security/limits.conf
   ```

### CPU Governor

```bash
# Set performance mode
sudo cpupower frequency-set -g performance

# Check current governor
cpupower frequency-info
```

## Uninstallation

```bash
# Remove plugins
rm -rf ~/.vst3/LoudnessCompensator.vst3
rm -rf ~/.lv2/LoudnessCompensator.lv2
rm ~/.local/bin/LoudnessCompensator

# Clear DAW caches
rm ~/.config/REAPER/reaper-vstplugins64.ini  # Reaper
rm -rf ~/.config/ardour*/plugin_metadata     # Ardour
```

## Technical Notes

**Plugin Format**: LV2 Category: `http://lv2plug.in/ns/lv2core#EQPlugin`
**VST3 Category**: `Fx|EQ`
**Architecture**: x86_64 only (32-bit not supported)
**License**: AGPLv3 (open source, JUCE framework requirement)
**Dependencies**: JUCE framework, system audio libraries

## Support

**For Linux-specific issues:**
- Include distribution and version (e.g., "Ubuntu 22.04")
- Audio system in use (ALSA/JACK/PulseAudio)
- DAW name and version
- Build or runtime error messages

**Get Help:**
- GitHub Issues: https://github.com/grisys83/LoudnessCompensator/issues
- Linux Audio Community: https://linuxaudio.org/

---

**Experience professional loudness compensation on Linux! 🐧🎧**
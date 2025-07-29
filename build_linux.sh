#!/bin/bash

# LoudnessCompensator Linux Build Script
# Builds VST3, LV2, and Standalone versions for Linux

set -e  # Exit on any error

echo "🐧 Building LoudnessCompensator for Linux..."

# Color output functions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}▶ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    print_error "This script is designed for Linux systems only"
    print_warning "Current OS: $OSTYPE"
    exit 1
fi

# Configuration
BUILD_TYPE=${1:-Release}
BUILD_DIR="cmake-build-linux"
INSTALL_PREFIX=${2:-"$HOME/.local"}

print_status "Build configuration:"
echo "  Build type: $BUILD_TYPE"
echo "  Build directory: $BUILD_DIR"
echo "  Install prefix: $INSTALL_PREFIX"
echo ""

# Check for required tools
print_status "Checking build requirements..."

# Check CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake not found. Please install CMake 3.22 or later."
    echo "  Ubuntu/Debian: sudo apt install cmake"
    echo "  Fedora/RHEL: sudo dnf install cmake"
    echo "  Arch: sudo pacman -S cmake"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | grep -oP 'cmake version \K[0-9]+\.[0-9]+')
print_success "CMake found: version $CMAKE_VERSION"

# Check compiler
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | grep -oP 'g\+\+ \(.*\) \K[0-9]+\.[0-9]+\.[0-9]+')
    print_success "g++ found: version $GCC_VERSION"
elif command -v clang++ &> /dev/null; then
    CLANG_VERSION=$(clang++ --version | grep -oP 'clang version \K[0-9]+\.[0-9]+\.[0-9]+')
    print_success "clang++ found: version $CLANG_VERSION"
else
    print_error "No C++ compiler found. Please install g++ or clang++."
    echo "  Ubuntu/Debian: sudo apt install build-essential"
    echo "  Fedora/RHEL: sudo dnf groupinstall 'Development Tools'"
    echo "  Arch: sudo pacman -S base-devel"
    exit 1
fi

# Check for JUCE
print_status "Checking for JUCE framework..."
if [ -d "../JUCE" ]; then
    print_success "JUCE found in parent directory"
elif [ -d "JUCE" ]; then
    print_success "JUCE found in current directory"
else
    print_error "JUCE framework not found"
    echo "Please ensure JUCE is available in one of these locations:"
    echo "  - ../JUCE (parent directory)"
    echo "  - ./JUCE (current directory)"
    echo ""
    echo "You can clone JUCE with:"
    echo "  git clone https://github.com/juce-framework/JUCE.git"
    exit 1
fi

# Check Linux dependencies
print_status "Checking Linux audio/GUI dependencies..."

MISSING_DEPS=()

# Check for essential packages
check_package() {
    if ! pkg-config --exists "$1" 2>/dev/null; then
        MISSING_DEPS+=("$2")
    fi
}

check_package "alsa" "libasound2-dev"
check_package "freetype2" "libfreetype-dev"
check_package "x11" "libx11-dev"
check_package "xext" "libxext-dev"
check_package "xinerama" "libxinerama-dev"
check_package "xrandr" "libxrandr-dev"
check_package "xcursor" "libxcursor-dev"
check_package "xcomposite" "libxcomposite-dev"
check_package "xrender" "libxrender-dev"
check_package "fontconfig" "libfontconfig1-dev"

# Check for optional but common packages
if ! pkg-config --exists "jack" 2>/dev/null; then
    print_warning "JACK not found (optional): libjack-jackd2-dev"
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    print_error "Missing required dependencies:"
    for dep in "${MISSING_DEPS[@]}"; do
        echo "  - $dep"
    done
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt install ${MISSING_DEPS[*]}"
    echo "  Fedora/RHEL: sudo dnf install (equivalent packages)"
    echo "  Arch: sudo pacman -S (equivalent packages)"
    exit 1
fi

print_success "All dependencies found!"

# Create build directory
print_status "Setting up build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
print_status "Configuring build with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_BUILD_EXTRAS=OFF

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

print_success "CMake configuration completed"

# Build the plugin
print_status "Building plugin..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

print_success "Build completed successfully!"

# Check what was built
print_status "Build artifacts:"
find . -name "*.so" -o -name "LoudnessCompensator_*" | while read -r file; do
    if [ -f "$file" ]; then
        echo "  📦 $(basename "$file")"
    fi
done

# Optional: Install plugins
read -p "Install plugins to user directories? [y/N]: " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_status "Installing plugins..."
    
    # Create plugin directories
    mkdir -p "$HOME/.vst3"
    mkdir -p "$HOME/.lv2"
    
    # Copy VST3
    if [ -d "LoudnessCompensator_artefacts/VST3" ]; then
        cp -r LoudnessCompensator_artefacts/VST3/* "$HOME/.vst3/"
        print_success "VST3 installed to ~/.vst3/"
    fi
    
    # Copy LV2
    if [ -d "LoudnessCompensator_artefacts/LV2" ]; then
        cp -r LoudnessCompensator_artefacts/LV2/* "$HOME/.lv2/"
        print_success "LV2 installed to ~/.lv2/"
    fi
    
    # Copy Standalone
    if [ -f "LoudnessCompensator_artefacts/Standalone/LoudnessCompensator" ]; then
        mkdir -p "$HOME/.local/bin"
        cp "LoudnessCompensator_artefacts/Standalone/LoudnessCompensator" "$HOME/.local/bin/"
        chmod +x "$HOME/.local/bin/LoudnessCompensator"
        print_success "Standalone app installed to ~/.local/bin/LoudnessCompensator"
    fi
fi

echo ""
print_success "Linux build process completed!"
echo ""
echo "🎵 Available formats:"
echo "  • VST3: Use in Reaper, Ardour, Tracktion Waveform"
echo "  • LV2: Use in Ardour, Qtractor, Carla, Harrison Mixbus"
echo "  • Standalone: Run independently"
echo ""
echo "🔧 Testing:"
echo "  • Test VST3: Open your DAW and rescan plugins"
echo "  • Test LV2: Open Ardour or Qtractor and look for 'LoudnessCompensator'"
echo "  • Test Standalone: Run '$HOME/.local/bin/LoudnessCompensator'"
echo ""
print_success "Enjoy your cross-platform loudness compensation! 🐧🎧"
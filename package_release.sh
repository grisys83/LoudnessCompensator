#!/bin/bash

# LoudnessCompensator Release Packaging Script
# Packages builds for macOS, Windows, and Linux

set -e

VERSION=${1:-"1.1.0"}
RELEASE_DIR="release-$VERSION"

echo "🎵 Packaging LoudnessCompensator v$VERSION for all platforms..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

# Clean and create release directory
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

# macOS Build
print_status "Packaging macOS build..."
if [ -d "Builds/MacOSX/build/Release" ]; then
    mkdir -p "$RELEASE_DIR/macOS"
    
    # Copy macOS plugins
    if [ -d "Builds/MacOSX/build/Release/LoudnessCompensator.component" ]; then
        cp -R "Builds/MacOSX/build/Release/LoudnessCompensator.component" "$RELEASE_DIR/macOS/"
        print_success "AU component copied"
    fi
    
    if [ -d "Builds/MacOSX/build/Release/LoudnessCompensator.vst3" ]; then
        cp -R "Builds/MacOSX/build/Release/LoudnessCompensator.vst3" "$RELEASE_DIR/macOS/"
        print_success "VST3 plugin copied"
    fi
    
    if [ -d "Builds/MacOSX/build/Release/LoudnessCompensator.appex" ]; then
        cp -R "Builds/MacOSX/build/Release/LoudnessCompensator.appex" "$RELEASE_DIR/macOS/"
        print_success "AUv3 extension copied"
    fi
    
    # Create macOS archive
    cd "$RELEASE_DIR"
    zip -r "LoudnessCompensator-v$VERSION-macOS.zip" macOS/
    cd ..
    print_success "macOS archive created"
else
    print_warning "macOS build not found, skipping..."
fi

# Windows Build (if available)
print_status "Checking for Windows build..."
if [ -d "Builds/Windows" ] || [ -d "cmake-build-windows" ]; then
    mkdir -p "$RELEASE_DIR/Windows"
    print_warning "Windows build directory found but no automated build process yet"
    # TODO: Add Windows build artifacts when available
else
    print_warning "Windows build not found, skipping..."
fi

# Linux Build
print_status "Building and packaging Linux version..."

# Check if Docker is available
if command -v docker &> /dev/null; then
    print_status "Using Docker for Linux build..."
    
    # Build Docker image
    docker build -f Dockerfile.linux -t loudness-compensator-linux .
    
    # Run container and extract artifacts
    mkdir -p "$RELEASE_DIR/Linux"
    docker run --rm -v "$(pwd)/$RELEASE_DIR/Linux:/output-host" loudness-compensator-linux \
        bash -c "cp -r /output/* /output-host/ 2>/dev/null || echo 'No artifacts to copy'"
    
    if [ "$(ls -A $RELEASE_DIR/Linux)" ]; then
        # Create Linux archive
        cd "$RELEASE_DIR"
        tar -czf "LoudnessCompensator-v$VERSION-Linux.tar.gz" Linux/
        cd ..
        print_success "Linux archive created"
    else
        print_warning "Linux build produced no artifacts"
    fi
else
    print_warning "Docker not available, attempting local Linux build..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Run Linux build script
        ./build_linux.sh Release
        
        if [ -d "cmake-build-linux/LoudnessCompensator_artefacts" ]; then
            mkdir -p "$RELEASE_DIR/Linux"
            cp -r cmake-build-linux/LoudnessCompensator_artefacts/* "$RELEASE_DIR/Linux/"
            
            # Create Linux archive
            cd "$RELEASE_DIR"
            tar -czf "LoudnessCompensator-v$VERSION-Linux.tar.gz" Linux/
            cd ..
            print_success "Linux archive created"
        else
            print_error "Linux build failed or produced no artifacts"
        fi
    else
        print_warning "Not running on Linux and Docker unavailable, skipping Linux build"
    fi
fi

# Create install scripts for each platform
print_status "Creating installation scripts..."

# macOS install script
if [ -d "$RELEASE_DIR/macOS" ]; then
    cat > "$RELEASE_DIR/macOS/install.sh" << 'EOF'
#!/bin/bash
echo "Installing LoudnessCompensator for macOS..."

# Create plugin directories
mkdir -p ~/Library/Audio/Plug-Ins/Components
mkdir -p ~/Library/Audio/Plug-Ins/VST3

# Install plugins
if [ -d "LoudnessCompensator.component" ]; then
    cp -R LoudnessCompensator.component ~/Library/Audio/Plug-Ins/Components/
    echo "✅ AU component installed"
fi

if [ -d "LoudnessCompensator.vst3" ]; then
    cp -R LoudnessCompensator.vst3 ~/Library/Audio/Plug-Ins/VST3/
    echo "✅ VST3 plugin installed"
fi

if [ -d "LoudnessCompensator.appex" ]; then
    cp -R LoudnessCompensator.appex ~/Library/Audio/Plug-Ins/Components/
    echo "✅ AUv3 extension installed"
fi

echo "🎵 Installation complete! Restart your DAW and rescan plugins."
EOF
    chmod +x "$RELEASE_DIR/macOS/install.sh"
fi

# Linux install script
if [ -d "$RELEASE_DIR/Linux" ]; then
    cat > "$RELEASE_DIR/Linux/install.sh" << 'EOF'
#!/bin/bash
echo "Installing LoudnessCompensator for Linux..."

# Create plugin directories
mkdir -p ~/.vst3
mkdir -p ~/.lv2
mkdir -p ~/.local/bin

# Install plugins
if [ -d "VST3" ]; then
    cp -r VST3/* ~/.vst3/
    echo "✅ VST3 plugin installed"
fi

if [ -d "LV2" ]; then
    cp -r LV2/* ~/.lv2/
    echo "✅ LV2 plugin installed"
fi

if [ -f "Standalone/LoudnessCompensator" ]; then
    cp Standalone/LoudnessCompensator ~/.local/bin/
    chmod +x ~/.local/bin/LoudnessCompensator
    echo "✅ Standalone application installed"
fi

echo "🎵 Installation complete! Restart your DAW and rescan plugins."
echo "💡 Standalone: Run 'LoudnessCompensator' or '~/.local/bin/LoudnessCompensator'"
EOF
    chmod +x "$RELEASE_DIR/Linux/install.sh"
fi

# Create README for release
cat > "$RELEASE_DIR/README.txt" << EOF
LoudnessCompensator v$VERSION
============================

Perceptual loudness compensation plugin based on ISO 226:2003 equal-loudness contours.

Supported Platforms:
- macOS 10.13+ (AU, VST3, AUv3)
- Windows 10+ (VST3)
- Linux (VST3, LV2, Standalone)

Installation:
1. Extract the archive for your platform
2. Run the install.sh script (macOS/Linux) or follow manual instructions
3. Restart your DAW and rescan plugins

Manual Installation:
- macOS: Copy plugins to ~/Library/Audio/Plug-Ins/[Format]/
- Windows: Copy VST3 to C:/Program Files/Common Files/VST3/
- Linux: Copy to ~/.vst3/, ~/.lv2/, or ~/.local/bin/

Documentation:
- Full documentation: https://github.com/grisys83/LoudnessCompensator
- Linux guide: LINUX_INSTALL.md
- Windows guide: WINDOWS_INSTALL.md

License: AGPLv3
Source: https://github.com/grisys83/LoudnessCompensator

Support: Open an issue on GitHub for help or bug reports.

Enjoy accurate loudness compensation at any volume! 🎧
EOF

# Summary
print_status "Release packaging summary:"
echo "📦 Release directory: $RELEASE_DIR"
ls -la "$RELEASE_DIR"

echo ""
print_success "Release v$VERSION packaging completed!"
echo ""
echo "🚀 Next steps:"
echo "1. Test packages on target platforms"
echo "2. Upload to GitHub Releases"
echo "3. Update documentation"
echo ""
echo "📁 Generated files:"
find "$RELEASE_DIR" -name "*.zip" -o -name "*.tar.gz" | while read -r file; do
    echo "  📦 $(basename "$file") ($(du -h "$file" | cut -f1))"
done
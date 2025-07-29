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

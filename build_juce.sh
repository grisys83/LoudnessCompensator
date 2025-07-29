#!/bin/bash

# JUCE Loudness Compensator 빌드 스크립트

echo "🎵 Building Loudness Compensator with JUCE..."

# JUCE 경로
JUCE_PATH="../JUCE"
PROJUCER="$JUCE_PATH/Projucer.app/Contents/MacOS/Projucer"

# Projucer가 있는지 확인
if [ ! -f "$PROJUCER" ]; then
    echo "❌ Error: Projucer not found at $PROJUCER"
    echo "Please make sure JUCE is installed in the parent directory"
    exit 1
fi

# .jucer 파일이 있는지 확인
if [ ! -f "LoudnessCompensator.jucer" ]; then
    echo "❌ Error: LoudnessCompensator.jucer not found"
    exit 1
fi

# Xcode 프로젝트 생성
echo "📦 Generating Xcode project..."
"$PROJUCER" --resave LoudnessCompensator.jucer

# Xcode 프로젝트로 이동
cd Builds/MacOSX

# 빌드
echo "🔨 Building with xcodebuild..."
xcodebuild -project LoudnessCompensator.xcodeproj \
           -configuration Release \
           -alltargets \
           build

# 빌드 성공 확인
if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    
    # 플러그인 위치 표시
    echo ""
    echo "📍 Built plugins location:"
    echo "AU: ~/Library/Audio/Plug-Ins/Components/LoudnessCompensator.component"
    echo "VST3: ~/Library/Audio/Plug-Ins/VST3/LoudnessCompensator.vst3"
    
    # 설치 옵션
    echo ""
    read -p "Install plugins to system directories? [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        # AU 설치
        if [ -d "build/Release/LoudnessCompensator.component" ]; then
            echo "Installing AU..."
            cp -R "build/Release/LoudnessCompensator.component" ~/Library/Audio/Plug-Ins/Components/
        fi
        
        # VST3 설치
        if [ -d "build/Release/LoudnessCompensator.vst3" ]; then
            echo "Installing VST3..."
            cp -R "build/Release/LoudnessCompensator.vst3" ~/Library/Audio/Plug-Ins/VST3/
        fi
        
        echo "✅ Plugins installed!"
        
        # AU 검증
        echo ""
        echo "🔍 Validating Audio Unit..."
        auval -v aufx LDCP HYAN
    fi
else
    echo "❌ Build failed!"
    exit 1
fi
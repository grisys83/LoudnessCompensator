/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Custom Look and Feel
LoudnessCompensatorAudioProcessorEditor::CustomLookAndFeel::CustomLookAndFeel()
{
    // 색상 설정
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff4a90e2));
    setColour(juce::Slider::trackColourId, juce::Colour(0xff2c5aa0));
    setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1e1e1e));
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour(juce::Label::textColourId, juce::Colours::white);
}

void LoudnessCompensatorAudioProcessorEditor::CustomLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    auto radius = juce::jmin(width / 2, height / 2) - 8.0f;
    auto centreX = x + width * 0.5f;
    auto centreY = y + height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    
    // 배경 원
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillEllipse(rx, ry, rw, rw);
    
    // 트랙
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                               rotaryStartAngle, rotaryEndAngle, true);
    
    g.setColour(juce::Colour(0xff404040));
    g.strokePath(backgroundArc, juce::PathStrokeType(4.0f));
    
    // 값 아크
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                              rotaryStartAngle, angle, true);
        
        g.setColour(juce::Colour(0xff4a90e2));
        g.strokePath(valueArc, juce::PathStrokeType(4.0f));
    }
    
    // 포인터
    juce::Path pointer;
    auto pointerLength = radius * 0.8f;
    auto pointerThickness = 3.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour(slider.isEnabled() ? juce::Colours::white : juce::Colours::grey);
    g.fillPath(pointer);
}

//==============================================================================
LoudnessCompensatorAudioProcessorEditor::LoudnessCompensatorAudioProcessorEditor (LoudnessCompensatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // 윈도우 설정
    setSize (500, 400);
    setLookAndFeel(&customLookAndFeel);
    
    // 메인 슬라이더
    easyLoudnessSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    easyLoudnessSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    easyLoudnessSlider.setRange(20.0, 70.0, 0.1);
    addAndMakeVisible(easyLoudnessSlider);
    
    easyLoudnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "easyLoudness", easyLoudnessSlider);
    
    // 라벨들
    easyLoudnessLabel.setText("LOUDNESS", juce::dontSendNotification);
    easyLoudnessLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    easyLoudnessLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(easyLoudnessLabel);
    
    phonLabel.setText("65 phon", juce::dontSendNotification);
    phonLabel.setFont(juce::Font(16.0f));
    phonLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phonLabel);
    
    splLabel.setText("≈ 65 dB SPL", juce::dontSendNotification);
    splLabel.setFont(juce::Font(14.0f));
    splLabel.setJustificationType(juce::Justification::centred);
    splLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(splLabel);
    
    compensationLabel.setText("Reference: 65 > Target: 56 phon", juce::dontSendNotification);
    compensationLabel.setFont(juce::Font(12.0f));
    compensationLabel.setJustificationType(juce::Justification::centred);
    compensationLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(compensationLabel);
    
    // Bypass 버튼
    bypassButton.setButtonText("BYPASS");
    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "bypass", bypassButton);
    
    // Expert Mode 토글
    expertModeButton.setButtonText("EXPERT MODE");
    addAndMakeVisible(expertModeButton);
    expertModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "expertMode", expertModeButton);
    
    // Expert controls 항상 표시
    showExpertControls = true;
    
    // Expert Mode 버튼 상태 변경 리스너
    expertModeButton.onStateChange = [this]() { updateExpertControlsState(); };
    
    // 프리셋 선택 제거
    
    // Expert controls (항상 표시)
    addAndMakeVisible(kValueSlider);
    addAndMakeVisible(deltaMaxSlider);
    addAndMakeVisible(filterQualitySelector);
    addAndMakeVisible(kValueLabel);
    addAndMakeVisible(deltaMaxLabel);
    addAndMakeVisible(filterQualityLabel);
    
    setSize(500, 650);  // Expert controls와 Gain controls를 위한 높이
    kValueSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    kValueSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    kValueSlider.setRange(5.0, 30.0, 0.1);
    kValueAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "kValue", kValueSlider);
    
    deltaMaxSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    deltaMaxSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    deltaMaxSlider.setRange(10.0, 40.0, 0.1);
    deltaMaxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "deltaMax", deltaMaxSlider);
    
    filterQualitySelector.addItem("Low (511)", 1);
    filterQualitySelector.addItem("Medium (1023)", 2);
    filterQualitySelector.addItem("High (2047)", 3);
    filterQualitySelector.addItem("Ultra (4095)", 4);
    filterQualitySelector.setSelectedId(4);  // Ultra (4095) 기본값
    filterQualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), "filterTaps", filterQualitySelector);
    
    kValueLabel.setText("K Value:", juce::dontSendNotification);
    deltaMaxLabel.setText("Delta Max:", juce::dontSendNotification);
    filterQualityLabel.setText("Filter Quality:", juce::dontSendNotification);
    
    // Gain controls 초기화
    inputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    inputGainSlider.setRange(-20.0, 20.0, 0.1);
    inputGainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(inputGainSlider);
    
    outputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    outputGainSlider.setRange(-20.0, 20.0, 0.1);
    outputGainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(outputGainSlider);
    
    inputGainLabel.setText("Input Gain:", juce::dontSendNotification);
    outputGainLabel.setText("Output Gain:", juce::dontSendNotification);
    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(outputGainLabel);
    
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "inputGain", inputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "outputGain", outputGainSlider);
    
    // 타이머 시작 (UI 업데이트용)
    startTimerHz(10);
    
    // 초기 Expert Mode 상태 적용
    updateExpertControlsState();
}

LoudnessCompensatorAudioProcessorEditor::~LoudnessCompensatorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void LoudnessCompensatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // 배경
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // 타이틀
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawFittedText("LOUDNESS COMPENSATOR", 0, 10, getWidth() - 60, 30, juce::Justification::centred, 1);
    
    // Ver2 표시 제거
    
    // 서브타이틀
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::grey);
    g.drawFittedText("ISO 226:2003 Equal-Loudness Compensation", 0, 35, getWidth(), 20, juce::Justification::centred, 1);
    
    // SciPy 라이센스 고지
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colours::grey.withAlpha(0.6f));
    g.drawFittedText("FIR filter design derived from SciPy", 0, 52, getWidth(), 15, juce::Justification::centred, 1);
    
    // Expert mode 구분선
    if (showExpertControls)
    {
        bool expertMode = expertModeButton.getToggleState();
        float alpha = expertMode ? 1.0f : 0.4f;
        
        g.setColour(juce::Colour(0xff3a3a3a).withAlpha(alpha));
        g.drawLine(20, 380, getWidth() - 20, 380, 1);
        
        g.setColour(juce::Colours::grey.withAlpha(alpha));
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawFittedText("EXPERT CONTROLS", 0, 385, getWidth(), 20, juce::Justification::centred, 1);
        
        // Gain controls 구분선
        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawLine(20, 530, getWidth() - 20, 530, 1);
        
        g.setColour(juce::Colours::grey);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawFittedText("GAIN CONTROLS", 0, 535, getWidth(), 20, juce::Justification::centred, 1);
    }
}

void LoudnessCompensatorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // 타이틀 공간
    
    // 메인 컨트롤 영역
    auto mainArea = bounds.removeFromTop(320);
    
    // 중앙 슬라이더
    auto sliderArea = mainArea.removeFromTop(200);
    easyLoudnessSlider.setBounds(sliderArea.withSizeKeepingCentre(180, 180));
    
    // 라벨들
    auto labelArea = mainArea.removeFromTop(80);
    easyLoudnessLabel.setBounds(labelArea.removeFromTop(25));
    phonLabel.setBounds(labelArea.removeFromTop(20));
    splLabel.setBounds(labelArea.removeFromTop(18));
    compensationLabel.setBounds(labelArea);
    
    // 하단 컨트롤 - Bypass와 Expert Mode 버튼
    auto bottomArea = mainArea.removeFromTop(40);
    auto buttonWidth = 100;
    auto spacing = 20;
    auto totalWidth = buttonWidth * 2 + spacing;
    auto x = (getWidth() - totalWidth) / 2;
    
    bypassButton.setBounds(x, bottomArea.getY(), buttonWidth, 30);
    expertModeButton.setBounds(x + buttonWidth + spacing, bottomArea.getY(), buttonWidth, 30);
    
    // Expert controls
    if (showExpertControls)
    {
        bounds.removeFromTop(30); // 구분선 공간
        
        auto expertArea = bounds.removeFromTop(120);
        auto sliderHeight = 25;
        auto labelWidth = 100;
        auto margin = 40;
        
        auto kValueArea = expertArea.removeFromTop(sliderHeight + 10);
        kValueLabel.setBounds(margin, kValueArea.getY(), labelWidth, sliderHeight);
        kValueSlider.setBounds(margin + labelWidth, kValueArea.getY(), getWidth() - margin * 2 - labelWidth, sliderHeight);
        addAndMakeVisible(kValueLabel);
        addAndMakeVisible(kValueSlider);
        
        auto deltaMaxArea = expertArea.removeFromTop(sliderHeight + 10);
        deltaMaxLabel.setBounds(margin, deltaMaxArea.getY(), labelWidth, sliderHeight);
        deltaMaxSlider.setBounds(margin + labelWidth, deltaMaxArea.getY(), getWidth() - margin * 2 - labelWidth, sliderHeight);
        addAndMakeVisible(deltaMaxLabel);
        addAndMakeVisible(deltaMaxSlider);
        
        auto filterArea = expertArea.removeFromTop(sliderHeight + 10);
        filterQualityLabel.setBounds(margin, filterArea.getY(), labelWidth, sliderHeight);
        filterQualitySelector.setBounds(margin + labelWidth, filterArea.getY(), 150, sliderHeight);
        addAndMakeVisible(filterQualityLabel);
        addAndMakeVisible(filterQualitySelector);
        
        // Gain controls (Expert controls 아래에 배치)
        bounds.removeFromTop(30); // 구분선과 헤더를 위한 간격
        
        auto gainArea = bounds.removeFromTop(80);
        
        auto inputGainArea = gainArea.removeFromTop(sliderHeight + 10);
        inputGainLabel.setBounds(margin, inputGainArea.getY(), labelWidth, sliderHeight);
        inputGainSlider.setBounds(margin + labelWidth, inputGainArea.getY(), getWidth() - margin * 2 - labelWidth, sliderHeight);
        
        auto outputGainArea = gainArea.removeFromTop(sliderHeight + 10);
        outputGainLabel.setBounds(margin, outputGainArea.getY(), labelWidth, sliderHeight);
        outputGainSlider.setBounds(margin + labelWidth, outputGainArea.getY(), getWidth() - margin * 2 - labelWidth, sliderHeight);
    }
    else
    {
        kValueLabel.setVisible(false);
        kValueSlider.setVisible(false);
        deltaMaxLabel.setVisible(false);
        deltaMaxSlider.setVisible(false);
        filterQualityLabel.setVisible(false);
        filterQualitySelector.setVisible(false);
        inputGainLabel.setVisible(false);
        inputGainSlider.setVisible(false);
        outputGainLabel.setVisible(false);
        outputGainSlider.setVisible(false);
    }
}

void LoudnessCompensatorAudioProcessorEditor::timerCallback()
{
    updateLabels();
}

void LoudnessCompensatorAudioProcessorEditor::updateLabels()
{
    auto& dsp = audioProcessor.getDSP();
    float targetPhon = dsp.getTargetPhon();
    float referencePhon = dsp.getReferencePhon();
    
    phonLabel.setText(formatPhonDisplay(targetPhon), juce::dontSendNotification);
    splLabel.setText(formatSPLDisplay(targetPhon), juce::dontSendNotification);
    
    compensationLabel.setText(
        juce::String::formatted("Reference: %.0f > Target: %.0f phon", 
                               referencePhon, targetPhon),
        juce::dontSendNotification);
}

juce::String LoudnessCompensatorAudioProcessorEditor::formatPhonDisplay(float phon)
{
    return juce::String::formatted("%.0f phon", phon);
}

juce::String LoudnessCompensatorAudioProcessorEditor::formatSPLDisplay(float phon)
{
    // 1kHz에서 phon = SPL
    return juce::String::formatted("= %.0f dB SPL", phon);
}

void LoudnessCompensatorAudioProcessorEditor::updateExpertControlsState()
{
    bool expertMode = expertModeButton.getToggleState();
    
    // Expert controls 활성화/비활성화
    kValueSlider.setEnabled(expertMode);
    deltaMaxSlider.setEnabled(expertMode);
    filterQualitySelector.setEnabled(expertMode);
    
    // 비활성화 시 컬러 변경
    juce::Colour labelColour = expertMode ? juce::Colours::grey : juce::Colours::grey.withAlpha(0.5f);
    kValueLabel.setColour(juce::Label::textColourId, labelColour);
    deltaMaxLabel.setColour(juce::Label::textColourId, labelColour);
    filterQualityLabel.setColour(juce::Label::textColourId, labelColour);
    
    // 슬라이더 및 콤보박스 투명도 조정
    float alpha = expertMode ? 1.0f : 0.5f;
    kValueSlider.setAlpha(alpha);
    deltaMaxSlider.setAlpha(alpha);
    filterQualitySelector.setAlpha(alpha);
    
    repaint();
}

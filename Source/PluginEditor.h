/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class LoudnessCompensatorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                  private juce::Timer
{
public:
    LoudnessCompensatorAudioProcessorEditor (LoudnessCompensatorAudioProcessor&);
    ~LoudnessCompensatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // Reference to processor
    LoudnessCompensatorAudioProcessor& audioProcessor;
    
    // UI Components
    juce::Slider easyLoudnessSlider;
    juce::Label easyLoudnessLabel;
    juce::Label phonLabel;
    juce::Label splLabel;
    juce::Label compensationLabel;
    
    juce::ToggleButton bypassButton;
    juce::ToggleButton expertModeButton;  // Expert Mode 토글 추가
    
    // Expert mode controls - 항상 표시
    bool showExpertControls = true;
    juce::Slider kValueSlider;
    juce::Slider deltaMaxSlider;
    juce::ComboBox filterQualitySelector;
    juce::Label kValueLabel;
    juce::Label deltaMaxLabel;
    juce::Label filterQualityLabel;
    
    // Gain controls
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Label inputGainLabel;
    juce::Label outputGainLabel;
    
    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> easyLoudnessAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> expertModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kValueAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> deltaMaxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterQualityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    
    // Look and Feel
    class CustomLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        CustomLookAndFeel();
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider& slider) override;
    };
    
    CustomLookAndFeel customLookAndFeel;
    
    // Helper methods
    void updateLabels();
    juce::String formatPhonDisplay(float phon);
    juce::String formatSPLDisplay(float phon);
    void updateExpertControlsState();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessCompensatorAudioProcessorEditor)
};
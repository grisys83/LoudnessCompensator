/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/LoudnessCompensatorDSP.h"

//==============================================================================
/**
*/
class LoudnessCompensatorAudioProcessor  : public juce::AudioProcessor,
                                          public juce::AudioProcessorValueTreeState::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    LoudnessCompensatorAudioProcessor();
    ~LoudnessCompensatorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    // 파라미터 접근
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    LoudnessCompensatorDSP& getDSP() { return dsp; }
    
    // AudioProcessorValueTreeState::Listener
    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    //==============================================================================
    // DSP 엔진
    LoudnessCompensatorDSP dsp;
    
    // 파라미터 관리
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // 파라미터 리스너
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> easyLoudnessListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> bypassListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> kValueListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> deltaMaxListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> filterTapsListener;
    
    // 프리셋 없음
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessCompensatorAudioProcessor)
};
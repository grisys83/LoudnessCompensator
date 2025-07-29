/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// 프리셋 없음

//==============================================================================
LoudnessCompensatorAudioProcessor::LoudnessCompensatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr, "LoudnessCompensator", createParameterLayout())
#else
     : parameters(*this, nullptr, "LoudnessCompensator", createParameterLayout())
#endif
{
    // 파라미터 리스너 설정
    parameters.addParameterListener("easyLoudness", this);
    parameters.addParameterListener("bypass", this);
    parameters.addParameterListener("kValue", this);
    parameters.addParameterListener("deltaMax", this);
    parameters.addParameterListener("filterTaps", this);
    parameters.addParameterListener("expertMode", this);
    parameters.addParameterListener("inputGain", this);
    parameters.addParameterListener("outputGain", this);
}

LoudnessCompensatorAudioProcessor::~LoudnessCompensatorAudioProcessor()
{
    parameters.removeParameterListener("easyLoudness", this);
    parameters.removeParameterListener("bypass", this);
    parameters.removeParameterListener("kValue", this);
    parameters.removeParameterListener("deltaMax", this);
    parameters.removeParameterListener("filterTaps", this);
    parameters.removeParameterListener("expertMode", this);
    parameters.removeParameterListener("inputGain", this);
    parameters.removeParameterListener("outputGain", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout LoudnessCompensatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Easy Loudness (메인 파라미터) - 20-70 범위로 변경
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "easyLoudness",
        "Loudness",
        juce::NormalisableRange<float>(20.0f, 70.0f, 0.1f),
        55.0f,
        "phon",
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " phon"; },
        [](const juce::String& text) { return text.upToFirstOccurrenceOf(" ", false, false).getFloatValue(); }
    ));
    
    // Bypass
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "bypass",
        "Bypass",
        false
    ));
    
    // K Value (Expert)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "kValue",
        "K Value",
        juce::NormalisableRange<float>(5.0f, 30.0f, 0.1f),
        20.0f
    ));
    
    // Delta Max (Expert)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "deltaMax",
        "Delta Max",
        juce::NormalisableRange<float>(10.0f, 40.0f, 0.1f),
        20.0f
    ));
    
    // Filter Taps
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "filterTaps",
        "Filter Quality",
        juce::StringArray{"Low (511)", "Medium (1023)", "High (2047)", "Ultra (4095)"},
        3
    ));
    
    // Expert Mode 파라미터 추가
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "expertMode",
        "Expert Mode",
        false
    ));
    
    // Gain parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "inputGain",
        "Input Gain",
        juce::NormalisableRange<float>(-20.0f, 20.0f, 0.1f),
        0.0f,
        "dB"
    ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "outputGain",
        "Output Gain",
        juce::NormalisableRange<float>(-20.0f, 20.0f, 0.1f),
        0.0f,
        "dB"
    ));
    
    return layout;
}

//==============================================================================
const juce::String LoudnessCompensatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LoudnessCompensatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LoudnessCompensatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LoudnessCompensatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LoudnessCompensatorAudioProcessor::getTailLengthSeconds() const
{
    return dsp.getLatencySamples() / getSampleRate();
}

int LoudnessCompensatorAudioProcessor::getNumPrograms()
{
    return 1; // 프리셋 없음
}

int LoudnessCompensatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LoudnessCompensatorAudioProcessor::setCurrentProgram (int index)
{
    // 프리셋 없음
}

const juce::String LoudnessCompensatorAudioProcessor::getProgramName (int index)
{
    return "Default";
}

void LoudnessCompensatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    // 프리셋 없음
}

//==============================================================================
void LoudnessCompensatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // DSP 준비
    dsp.prepare(sampleRate, samplesPerBlock);
    
    // 레이턴시 보고
    setLatencySamples(dsp.getLatencySamples());
}

void LoudnessCompensatorAudioProcessor::releaseResources()
{
    dsp.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LoudnessCompensatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // 모노와 스테레오만 지원
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LoudnessCompensatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // 사용하지 않는 출력 채널 클리어
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Input gain 적용
    float inputGainDB = parameters.getRawParameterValue("inputGain")->load();
    if (inputGainDB != 0.0f)
    {
        float inputGainLinear = juce::Decibels::decibelsToGain(inputGainDB);
        buffer.applyGain(inputGainLinear);
    }

    // DSP 처리
    dsp.process(buffer);
    
    // Output gain 적용
    float outputGainDB = parameters.getRawParameterValue("outputGain")->load();
    if (outputGainDB != 0.0f)
    {
        float outputGainLinear = juce::Decibels::decibelsToGain(outputGainDB);
        buffer.applyGain(outputGainLinear);
    }
}

//==============================================================================
bool LoudnessCompensatorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LoudnessCompensatorAudioProcessor::createEditor()
{
    return new LoudnessCompensatorAudioProcessorEditor (*this);
}

//==============================================================================
void LoudnessCompensatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // 파라미터 상태 저장
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void LoudnessCompensatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // 파라미터 상태 복원
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// 파라미터 변경 콜백
void LoudnessCompensatorAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "easyLoudness")
    {
        dsp.setEasyLoudness(newValue);
    }
    else if (parameterID == "bypass")
    {
        dsp.setBypass(newValue > 0.5f);
    }
    else if (parameterID == "kValue")
    {
        dsp.setKValue(newValue);
    }
    else if (parameterID == "deltaMax")
    {
        dsp.setDeltaMax(newValue);
    }
    else if (parameterID == "filterTaps")
    {
        int taps = 511;
        int index = static_cast<int>(newValue * 3.0f + 0.5f);
        switch (index)
        {
            case 0: taps = 511; break;
            case 1: taps = 1023; break;
            case 2: taps = 2047; break;
            case 3: taps = 4095; break;
        }
        dsp.setFilterTaps(taps);
    }
    else if (parameterID == "expertMode")
    {
        dsp.setExpertMode(newValue > 0.5f);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LoudnessCompensatorAudioProcessor();
}
/*
  ==============================================================================

    LoudnessCompensatorDSP.h
    ISO 226:2003 기반 등청감 보정 DSP 엔진

  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <complex>
#include <map>

class LoudnessCompensatorDSP
{
public:
    LoudnessCompensatorDSP();
    ~LoudnessCompensatorDSP();
    
    // 파라미터 설정
    void setEasyLoudness(float value); // 0-100
    void setBypass(bool bypass);
    void setKValue(float k);
    void setDeltaMax(float delta);
    void setFilterTaps(int taps);
    void setExpertMode(bool expert);
    
    // 오디오 처리
    void prepare(double sampleRate, int maximumBlockSize);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // 정보 획득
    float getTargetPhon() const { return targetPhon; }
    float getReferencePhon() const { return referencePhon; }
    float getPreampGain() const { return preampGain; }
    int getLatencySamples() const { return filterTaps / 2; }
    
private:
    // DSP 핵심 함수들 (AudioUnit 코드에서 포팅)
    void updateFIRCoefficients();
    std::vector<float> generateFIRFilter(float targetPhon, float referencePhon);
    std::vector<float> calculateISOGains(float targetPhon, float referencePhon);
    std::vector<float> firwin2(int numtaps, const std::vector<float>& freq, 
                              const std::vector<float>& gain, float fs);
    std::vector<float> irfft(const std::vector<std::complex<float>>& spectrum);
    
    // ISO 226:2003 데이터
    void loadISOData();
    float interpolateISO(float phon, float frequency);
    
    // RMS 계산
    float calculateRMSOffset(float targetPhon, float referencePhon);
    
    // 파라미터
    float easyLoudness = 55.0f;  // 40-70 범위의 중간값
    float targetPhon = 65.0f;
    float referencePhon = 83.0f;
    float kValue = 20.0f;  // 기본값 20
    float deltaMax = 20.0f;
    float preampGain = 0.0f;
    int filterTaps = 4095;  // Ultra 기본값
    bool bypass = false;
    bool expertMode = false;  // Expert Mode 플래그
    bool coefficientsNeedUpdate = true;
    
    // 적응형 파라미터 계산
    void calculateAdaptiveParameters();
    float getMasterGain() const;
    
    // 샘플레이트
    double currentSampleRate = 48000.0;
    
    // FIR 필터
    std::vector<float> firCoefficients;
    juce::dsp::Convolution convolution;
    
    // ISO 데이터
    std::map<float, std::vector<float>> isoCurves;
    std::vector<float> isoFrequencies;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoudnessCompensatorDSP)
};
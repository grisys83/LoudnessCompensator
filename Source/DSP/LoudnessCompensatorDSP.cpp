/*
  ==============================================================================

    LoudnessCompensatorDSP.cpp
    ISO 226:2003 기반 등청감 보정 DSP 구현

  ==============================================================================
*/

#include "LoudnessCompensatorDSP.h"
#include "ISO226Data.h"
#include <cmath>
#include <complex>

LoudnessCompensatorDSP::LoudnessCompensatorDSP()
{
    loadISOData();
}

LoudnessCompensatorDSP::~LoudnessCompensatorDSP()
{
}

void LoudnessCompensatorDSP::setEasyLoudness(float value)
{
    easyLoudness = juce::jlimit(20.0f, 70.0f, value);
    
    // Easy Loudness (20-70) → Target Phon (20-70)
    targetPhon = easyLoudness;
    
    // Expert Mode가 아닐 때만 적응형 파라미터 적용
    if (!expertMode)
    {
        calculateAdaptiveParameters();
    }
    
    // Reference Phon 계산 (exponential decay)
    const float deltaMin = 1.0f;
    float gap = deltaMin + (deltaMax - deltaMin) * std::exp(-(targetPhon - 20.0f) / kValue);
    
    // Expert Mode가 아닐 때 페이드아웃 및 헤드룸 보호 적용
    if (!expertMode)
    {
        // 페이드아웃 (80-85 phon)
        float fadeScale = 1.0f;
        if (targetPhon >= 80.0f && targetPhon < 85.0f)
        {
            fadeScale = 1.0f - (targetPhon - 80.0f) / 5.0f;
        }
        else if (targetPhon >= 85.0f)
        {
            fadeScale = 0.0f;
        }
        
        // 헤드룸 보호 (70 phon 이상)
        float headroomScale = 1.0f;
        if (targetPhon > 70.0f)
        {
            headroomScale = 1.0f - 0.3f * (targetPhon - 70.0f) / 20.0f;
            headroomScale = juce::jmax(0.7f, headroomScale);
        }
        
        gap *= fadeScale * headroomScale;
    }
    
    referencePhon = targetPhon + gap;
    referencePhon = juce::jmax(referencePhon, targetPhon + 0.1f);
    
    coefficientsNeedUpdate = true;
}

void LoudnessCompensatorDSP::setBypass(bool shouldBypass)
{
    bypass = shouldBypass;
}

void LoudnessCompensatorDSP::setKValue(float k)
{
    kValue = juce::jlimit(5.0f, 30.0f, k);
    setEasyLoudness(easyLoudness); // 재계산
}

void LoudnessCompensatorDSP::setDeltaMax(float delta)
{
    deltaMax = juce::jlimit(10.0f, 40.0f, delta);
    setEasyLoudness(easyLoudness); // 재계산
}

void LoudnessCompensatorDSP::setFilterTaps(int taps)
{
    filterTaps = taps;
    coefficientsNeedUpdate = true;
}

void LoudnessCompensatorDSP::setExpertMode(bool expert)
{
    expertMode = expert;
    setEasyLoudness(easyLoudness); // 재계산
}

void LoudnessCompensatorDSP::prepare(double sampleRate, int maximumBlockSize)
{
    currentSampleRate = sampleRate;
    
    // Convolution 준비
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(maximumBlockSize);
    spec.numChannels = 2;
    
    convolution.prepare(spec);
    
    // 초기 FIR 계수 계산
    updateFIRCoefficients();
}

void LoudnessCompensatorDSP::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
        return;
    
    // FIR 계수 업데이트 필요 시
    if (coefficientsNeedUpdate)
    {
        updateFIRCoefficients();
        coefficientsNeedUpdate = false;
    }
    
    // JUCE DSP 블록으로 변환
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Convolution 처리
    convolution.process(context);
    
    // Master Gain 적용 (-3dB + preamp + 헤드룸/페이드아웃 보정)
    float masterGainDB = getMasterGain();
    float linearGain = std::pow(10.0f, masterGainDB / 20.0f);
    
    buffer.applyGain(linearGain);
}

void LoudnessCompensatorDSP::reset()
{
    convolution.reset();
}

void LoudnessCompensatorDSP::updateFIRCoefficients()
{
    // FIR 필터 생성
    firCoefficients = generateFIRFilter(targetPhon, referencePhon);
    
    // RMS offset 계산
    float rmsOffset = calculateRMSOffset(targetPhon, referencePhon);
    preampGain = -rmsOffset; // 보상
    
    // JUCE Convolution에 적용
    if (!firCoefficients.empty())
    {
        // AudioBuffer로 변환
        juce::AudioBuffer<float> irBuffer(1, static_cast<int>(firCoefficients.size()));
        irBuffer.copyFrom(0, 0, firCoefficients.data(), static_cast<int>(firCoefficients.size()));
        
        // Convolution 로드
        convolution.loadImpulseResponse(std::move(irBuffer), currentSampleRate, 
                                       juce::dsp::Convolution::Stereo::yes, 
                                       juce::dsp::Convolution::Trim::no,
                                       juce::dsp::Convolution::Normalise::no);
    }
}

std::vector<float> LoudnessCompensatorDSP::generateFIRFilter(float targetPhon, float referencePhon)
{
    // ISO gain 계산
    std::vector<float> gainsDB = calculateISOGains(targetPhon, referencePhon);
    
    // Linear gain으로 변환
    std::vector<float> gainsLinear(gainsDB.size());
    for (size_t i = 0; i < gainsDB.size(); ++i)
    {
        gainsLinear[i] = std::pow(10.0f, gainsDB[i] / 20.0f);
    }
    
    // 정규화된 주파수 (0-1)
    std::vector<float> normalizedFreq;
    float nyquist = currentSampleRate / 2.0f;
    for (float freq : ISO226::FREQUENCIES)
    {
        normalizedFreq.push_back(freq / nyquist);
    }
    
    // Firwin2 호출
    return firwin2(filterTaps, normalizedFreq, gainsLinear, currentSampleRate);
}

std::vector<float> LoudnessCompensatorDSP::calculateISOGains(float targetPhon, float referencePhon)
{
    std::vector<float> gains;
    
    for (float freq : ISO226::FREQUENCIES)
    {
        float targetSPL = interpolateISO(targetPhon, freq);
        float referenceSPL = interpolateISO(referencePhon, freq);
        
        // 핵심: reference - target (NOT target - reference!)
        float gainDB = referenceSPL - targetSPL;
        gains.push_back(gainDB);
    }
    
    // 1kHz 정규화
    auto it = std::find(std::begin(ISO226::FREQUENCIES), std::end(ISO226::FREQUENCIES), 1000.0f);
    if (it != std::end(ISO226::FREQUENCIES))
    {
        size_t idx = std::distance(std::begin(ISO226::FREQUENCIES), it);
        float norm = gains[idx];
        for (float& gain : gains)
        {
            gain -= norm;
        }
    }
    
    return gains;
}

std::vector<float> LoudnessCompensatorDSP::firwin2(int numtaps,
                                                  const std::vector<float>& freq,
                                                  const std::vector<float>& gain,
                                                  float fs)
{
    // Python scipy.signal.firwin2의 정확한 포팅
    float nyq = fs / 2.0f;
    
    // nfreqs 계산: 1 + 2^ceil(log2(numtaps))
    int nfreqs = 1 + (1 << static_cast<int>(std::ceil(std::log2(numtaps))));
    
    // x = linspace(0.0, nyq, nfreqs)
    std::vector<float> x(nfreqs);
    for (int i = 0; i < nfreqs; ++i)
    {
        x[i] = nyq * static_cast<float>(i) / static_cast<float>(nfreqs - 1);
    }
    
    // freq/gain을 실제 주파수로 변환 (정규화된 주파수를 Hz로)
    std::vector<float> freq_hz;
    for (float f : freq)
    {
        freq_hz.push_back(f * nyq);
    }
    
    // fx = interp(x, freq_hz, gain)
    std::vector<float> fx(nfreqs);
    for (int i = 0; i < nfreqs; ++i)
    {
        float target_freq = x[i];
        
        // Find interpolation points
        size_t j = 0;
        for (j = 0; j < freq_hz.size() - 1; ++j)
        {
            if (target_freq >= freq_hz[j] && target_freq <= freq_hz[j + 1])
            {
                break;
            }
        }
        
        if (j >= freq_hz.size() - 1)
        {
            // 범위 밖: 마지막 값 사용
            fx[i] = gain.back();
        }
        else
        {
            // 선형 보간
            float t = (target_freq - freq_hz[j]) / (freq_hz[j + 1] - freq_hz[j]);
            fx[i] = gain[j] * (1.0f - t) + gain[j + 1] * t;
        }
    }
    
    // shift = exp(-(numtaps - 1) / 2. * 1j * pi * x / nyq)
    std::vector<std::complex<float>> fx2(nfreqs);
    for (int i = 0; i < nfreqs; ++i)
    {
        float phase = -(numtaps - 1) / 2.0f * juce::MathConstants<float>::pi * x[i] / nyq;
        std::complex<float> shift(std::cos(phase), std::sin(phase));
        fx2[i] = fx[i] * shift;
    }
    
    // IRFFT로 임펄스 응답 생성
    std::vector<float> out_full = irfft(fx2);
    
    // 첫 numtaps 샘플만 유지
    std::vector<float> out(numtaps);
    for (int i = 0; i < numtaps; ++i)
    {
        out[i] = out_full[i];
    }
    
    // Window 적용 (Hann)
    for (int i = 0; i < numtaps; ++i)
    {
        float window = 0.5f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (numtaps - 1));
        out[i] *= window;
    }
    
    // 1kHz 정규화 (Python과 동일)
    float omega = 2.0f * juce::MathConstants<float>::pi * 1000.0f / fs;
    std::complex<float> h(0.0f, 0.0f);
    
    for (int n = 0; n < numtaps; ++n)
    {
        float angle = -omega * n;
        h += out[n] * std::complex<float>(std::cos(angle), std::sin(angle));
    }
    
    float magnitude = std::abs(h);
    if (magnitude > 0.0f)
    {
        for (float& sample : out)
        {
            sample /= magnitude;
        }
    }
    
    return out;
}

std::vector<float> LoudnessCompensatorDSP::irfft(const std::vector<std::complex<float>>& spectrum)
{
    // Python scipy.fft.irfft와 동일한 구현
    // output size = 2 * (input_size - 1)
    const int n = static_cast<int>((spectrum.size() - 1) * 2);
    std::vector<float> result(n);
    
    // 전체 스펙트럼 구성 (Hermitian symmetry)
    std::vector<std::complex<float>> fullSpectrum(n);
    
    // 양의 주파수 복사
    for (size_t i = 0; i < spectrum.size(); ++i)
    {
        fullSpectrum[i] = spectrum[i];
    }
    
    // 음의 주파수 (켤레 대칭)
    for (size_t i = 1; i < spectrum.size() - 1; ++i)
    {
        fullSpectrum[n - i] = std::conj(spectrum[i]);
    }
    
    // IFFT 계산
    for (int k = 0; k < n; ++k)
    {
        std::complex<float> sum(0.0f, 0.0f);
        for (int j = 0; j < n; ++j)
        {
            float angle = 2.0f * juce::MathConstants<float>::pi * j * k / n;
            std::complex<float> w(std::cos(angle), std::sin(angle));
            sum += fullSpectrum[j] * w;
        }
        result[k] = sum.real() / n;  // 실수부만 취하고 정규화
    }
    
    return result;
}

float LoudnessCompensatorDSP::calculateRMSOffset(float targetPhon, float referencePhon)
{
    // Pink noise RMS offset 계산
    // JavaScript 구현과 동일한 로직
    
    // Pink noise의 주파수별 가중치 (1/f)
    std::vector<float> pinkWeights;
    for (float freq : isoFrequencies)
    {
        pinkWeights.push_back(1.0f / std::sqrt(freq));
    }
    
    // 가중치 정규화
    float weightSum = 0.0f;
    for (float w : pinkWeights)
    {
        weightSum += w;
    }
    for (float& w : pinkWeights)
    {
        w /= weightSum;
    }
    
    // 각 주파수에서의 gain 계산
    float totalSquaredGain = 0.0f;
    for (size_t i = 0; i < isoFrequencies.size(); ++i)
    {
        float targetSPL = interpolateISO(targetPhon, isoFrequencies[i]);
        float referenceSPL = interpolateISO(referencePhon, isoFrequencies[i]);
        float gainDB = referenceSPL - targetSPL;
        float gainLinear = std::pow(10.0f, gainDB / 20.0f);
        
        // Pink noise 가중치 적용
        totalSquaredGain += gainLinear * gainLinear * pinkWeights[i];
    }
    
    // RMS gain을 dB로 변환
    float rmsGain = std::sqrt(totalSquaredGain);
    float rmsDB = 20.0f * std::log10(rmsGain);
    
    return rmsDB;
}

void LoudnessCompensatorDSP::loadISOData()
{
    // ISO 226:2003 데이터 로드
    // AudioUnit 코드에서 가져옴
    
    // 10 phon curve
    isoCurves[10.0f] = {
        31.0f, 27.4f, 24.2f, 21.4f, 19.2f, 17.1f, 15.3f, 13.7f, 12.2f, 10.8f,
        9.6f, 8.5f, 7.5f, 6.5f, 5.6f, 4.9f, 4.2f, 3.7f, 3.4f, 3.3f,
        3.7f, 4.9f, 7.1f, 9.7f, 12.0f, 13.8f, 14.8f, 14.4f, 13.2f, 11.1f, 8.1f
    };
    
    // 20 phon curve
    isoCurves[20.0f] = {
        42.4f, 38.2f, 34.7f, 31.5f, 28.9f, 26.3f, 23.9f, 21.7f, 19.8f, 17.8f,
        16.0f, 14.4f, 12.9f, 11.4f, 10.2f, 9.2f, 8.3f, 7.6f, 7.0f, 6.8f,
        7.0f, 8.0f, 10.2f, 13.2f, 16.3f, 19.2f, 21.7f, 23.1f, 23.6f, 22.9f, 20.7f
    };
    
    // 30 phon curve
    isoCurves[30.0f] = {
        52.1f, 47.9f, 44.3f, 40.8f, 37.9f, 35.0f, 32.3f, 29.8f, 27.4f, 25.0f,
        22.8f, 20.8f, 19.0f, 17.1f, 15.5f, 14.1f, 12.9f, 12.0f, 11.4f, 11.1f,
        11.3f, 12.3f, 14.6f, 17.8f, 21.3f, 25.0f, 28.5f, 31.2f, 33.0f, 33.8f, 32.5f
    };
    
    // 40 phon curve
    isoCurves[40.0f] = {
        61.8f, 57.6f, 53.7f, 49.9f, 46.7f, 43.5f, 40.5f, 37.6f, 34.9f, 32.1f,
        29.6f, 27.2f, 25.0f, 22.8f, 20.8f, 19.0f, 17.5f, 16.3f, 15.6f, 15.2f,
        15.4f, 16.5f, 18.9f, 22.3f, 26.2f, 30.5f, 34.8f, 38.5f, 41.3f, 43.0f, 42.7f
    };
    
    // 50 phon curve
    isoCurves[50.0f] = {
        71.5f, 67.3f, 63.3f, 59.3f, 55.9f, 52.5f, 49.2f, 46.0f, 42.9f, 39.8f,
        36.8f, 34.0f, 31.4f, 28.7f, 26.3f, 24.1f, 22.3f, 20.8f, 19.9f, 19.4f,
        19.6f, 20.8f, 23.5f, 27.3f, 31.8f, 36.9f, 42.3f, 47.2f, 51.2f, 53.8f, 54.4f
    };
    
    // 60 phon curve
    isoCurves[60.0f] = {
        81.2f, 77.0f, 73.0f, 68.9f, 65.4f, 61.8f, 58.2f, 54.7f, 51.2f, 47.6f,
        44.2f, 40.9f, 37.8f, 34.6f, 31.8f, 29.2f, 27.0f, 25.3f, 24.1f, 23.5f,
        23.6f, 24.9f, 27.7f, 31.9f, 37.0f, 43.0f, 49.6f, 55.9f, 61.3f, 65.2f, 66.8f
    };
    
    // 70 phon curve
    isoCurves[70.0f] = {
        90.9f, 86.8f, 82.7f, 78.5f, 74.9f, 71.2f, 67.5f, 63.8f, 60.0f, 56.1f,
        52.3f, 48.7f, 45.1f, 41.5f, 38.2f, 35.2f, 32.6f, 30.5f, 29.0f, 28.2f,
        28.2f, 29.5f, 32.5f, 37.1f, 42.9f, 49.9f, 57.8f, 65.7f, 72.7f, 78.1f, 80.8f
    };
    
    // 80 phon curve
    isoCurves[80.0f] = {
        100.7f, 96.5f, 92.5f, 88.2f, 84.5f, 80.7f, 76.8f, 72.9f, 68.9f, 64.7f,
        60.5f, 56.5f, 52.5f, 48.5f, 44.8f, 41.3f, 38.3f, 35.8f, 34.0f, 33.0f,
        32.8f, 34.2f, 37.3f, 42.3f, 48.7f, 56.8f, 66.1f, 75.8f, 84.6f, 91.4f, 95.1f
    };
    
    // 90 phon curve
    isoCurves[90.0f] = {
        110.4f, 106.3f, 102.3f, 97.9f, 94.1f, 90.2f, 86.3f, 82.2f, 78.0f, 73.5f,
        68.9f, 64.5f, 60.1f, 55.6f, 51.4f, 47.5f, 44.1f, 41.2f, 39.0f, 37.8f,
        37.5f, 38.8f, 42.1f, 47.5f, 54.6f, 63.8f, 74.8f, 86.5f, 97.3f, 105.5f, 110.0f
    };
    
    // 주파수 배열 초기화
    isoFrequencies.assign(std::begin(ISO226::FREQUENCIES), std::end(ISO226::FREQUENCIES));
}

void LoudnessCompensatorDSP::calculateAdaptiveParameters()
{
    // targetPhon 레벨에 따라 k와 deltaMax를 자동 조정
    if (targetPhon < 30.0f)
    {
        kValue = 20.0f;
        deltaMax = 25.0f;
    }
    else if (targetPhon < 40.0f)
    {
        kValue = 19.0f;
        deltaMax = 22.0f;
    }
    else if (targetPhon < 55.0f)
    {
        kValue = 18.0f;
        deltaMax = 20.0f;
    }
    else if (targetPhon < 60.0f)
    {
        kValue = 18.0f;
        deltaMax = 16.0f;
    }
    else if (targetPhon < 70.0f)
    {
        kValue = 16.0f;
        deltaMax = 12.0f;
    }
    else if (targetPhon < 80.0f)
    {
        kValue = 14.0f;
        deltaMax = 8.0f;
    }
    else
    {
        kValue = 12.0f;
        deltaMax = 4.0f;
    }
}

float LoudnessCompensatorDSP::getMasterGain() const
{
    // 기본 -3dB
    float masterGain = -0.0f;
    
    // Preamp gain 추가
    masterGain += preampGain;
    
    // Expert Mode가 아닐 때만 헤드룸 보호 적용
    if (!expertMode && targetPhon > 70.0f)
    {
        float headroomReduction = -2.0f * (targetPhon - 70.0f) / 20.0f;
        headroomReduction = juce::jmax(-4.0f, headroomReduction);
        masterGain += headroomReduction;
    }
    
    return masterGain;
}

float LoudnessCompensatorDSP::interpolateISO(float phon, float frequency)
{
    // ISO 226:2003 기반 정확한 보간
    // AudioUnit 코드와 동일
    
    // 1. Phon 범위 제한
    phon = juce::jlimit(20.0f, 90.0f, phon);
    
    // 2. 가장 가까운 phon curve 찾기
    float lowerPhon = std::floor(phon / 10.0f) * 10.0f;
    float upperPhon = std::ceil(phon / 10.0f) * 10.0f;
    
    if (lowerPhon == upperPhon)
    {
        upperPhon = lowerPhon + 10.0f;
    }
    
    if (isoCurves.find(lowerPhon) == isoCurves.end() ||
        isoCurves.find(upperPhon) == isoCurves.end())
    {
        return phon;  // 기본값
    }
    
    // 3. 주파수에 대한 선형 보간
    // 주파수가 정확히 일치하는 경우 찾기
    auto it = std::find(isoFrequencies.begin(), isoFrequencies.end(), frequency);
    if (it != isoFrequencies.end())
    {
        // 정확한 주파수 - phon 보간만 수행
        size_t idx = std::distance(isoFrequencies.begin(), it);
        float t = (phon - lowerPhon) / (upperPhon - lowerPhon);
        float lowerSPL = isoCurves[lowerPhon][idx];
        float upperSPL = isoCurves[upperPhon][idx];
        return lowerSPL * (1.0f - t) + upperSPL * t;
    }
    
    // 4. 주파수 보간 필요
    // 인접한 두 주파수 찾기
    size_t freqIdx = 0;
    for (size_t i = 0; i < isoFrequencies.size() - 1; ++i)
    {
        if (frequency >= isoFrequencies[i] && frequency <= isoFrequencies[i + 1])
        {
            freqIdx = i;
            break;
        }
    }
    
    // 주파수 범위 밖인 경우
    if (frequency < isoFrequencies.front())
    {
        freqIdx = 0;
    }
    else if (frequency > isoFrequencies.back())
    {
        freqIdx = isoFrequencies.size() - 2;
    }
    
    // 5. 이중 선형 보간 (Bilinear interpolation)
    float freqRatio = (frequency - isoFrequencies[freqIdx]) / 
                      (isoFrequencies[freqIdx + 1] - isoFrequencies[freqIdx]);
    float phonRatio = (phon - lowerPhon) / (upperPhon - lowerPhon);
    
    // 4개의 코너 값
    float spl00 = isoCurves[lowerPhon][freqIdx];
    float spl01 = isoCurves[lowerPhon][freqIdx + 1];
    float spl10 = isoCurves[upperPhon][freqIdx];
    float spl11 = isoCurves[upperPhon][freqIdx + 1];
    
    // 주파수 방향 보간
    float spl0 = spl00 * (1.0f - freqRatio) + spl01 * freqRatio;
    float spl1 = spl10 * (1.0f - freqRatio) + spl11 * freqRatio;
    
    // Phon 방향 보간
    return spl0 * (1.0f - phonRatio) + spl1 * phonRatio;
}

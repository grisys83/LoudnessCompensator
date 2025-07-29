// 디버그용 코드 추가

void debugFIRResponse(const std::vector<float>& firCoeffs, float sampleRate) {
    // FFT를 사용하여 주파수 응답 분석
    const int fftSize = 2048;
    std::vector<std::complex<float>> fftData(fftSize, 0);
    
    // FIR 계수를 FFT 입력에 복사
    for (size_t i = 0; i < std::min(firCoeffs.size(), fftData.size()); ++i) {
        fftData[i] = std::complex<float>(firCoeffs[i], 0);
    }
    
    // FFT 수행 (간단한 DFT)
    std::vector<std::complex<float>> spectrum(fftSize);
    for (int k = 0; k < fftSize; ++k) {
        std::complex<float> sum(0, 0);
        for (int n = 0; n < fftSize; ++n) {
            float angle = -2.0f * M_PI * k * n / fftSize;
            sum += fftData[n] * std::complex<float>(cosf(angle), sinf(angle));
        }
        spectrum[k] = sum;
    }
    
    // 주요 주파수에서의 응답 출력
    float nyquist = sampleRate / 2.0f;
    int testFreqs[] = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    
    DBG("=== FIR Filter Frequency Response ===");
    for (int freq : testFreqs) {
        if (freq < nyquist) {
            int bin = (int)((float)freq / nyquist * (fftSize / 2));
            float magnitude = std::abs(spectrum[bin]);
            float dB = 20.0f * log10f(magnitude + 1e-10f);
            DBG(String(freq) + " Hz: " + String(dB, 2) + " dB");
        }
    }
    DBG("=====================================");
}

// updateFIRCoefficients에 추가할 디버그 코드
void LoudnessCompensatorDSP::updateFIRCoefficients()
{
    // FIR 필터 생성
    firCoefficients = generateFIRFilter(targetPhon, referencePhon);
    
    // 디버그: FIR 계수 분석
    if (!firCoefficients.empty()) {
        DBG("Target Phon: " + String(targetPhon));
        DBG("Reference Phon: " + String(referencePhon));
        DBG("FIR Taps: " + String(firCoefficients.size()));
        
        // 최대/최소값 확인
        float maxCoeff = *std::max_element(firCoefficients.begin(), firCoefficients.end());
        float minCoeff = *std::min_element(firCoefficients.begin(), firCoefficients.end());
        DBG("Max coefficient: " + String(maxCoeff));
        DBG("Min coefficient: " + String(minCoeff));
        
        // 계수 합계 (DC gain)
        float sum = 0;
        for (float c : firCoefficients) sum += c;
        DBG("Sum of coefficients: " + String(sum));
        DBG("DC gain: " + String(20.0f * log10f(sum + 1e-10f)) + " dB");
        
        // 주파수 응답 분석
        debugFIRResponse(firCoefficients, currentSampleRate);
    }
    
    // RMS offset 계산
    float rmsOffset = calculateRMSOffset(targetPhon, referencePhon);
    preampGain = -rmsOffset; // 보상
    
    DBG("RMS Offset: " + String(rmsOffset) + " dB");
    DBG("Preamp Gain: " + String(preampGain) + " dB");
    
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
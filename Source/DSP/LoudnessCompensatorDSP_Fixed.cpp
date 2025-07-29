// 수정된 firwin2 구현

std::vector<float> LoudnessCompensatorDSP::firwin2(int numtaps,
                                                  const std::vector<float>& freq,
                                                  const std::vector<float>& gain,
                                                  float fs)
{
    // JavaScript와 동일하게 구현
    float nyq = 0.5f * fs;
    
    // 정규화된 주파수 (0-1)
    std::vector<float> freqNorm;
    for (float f : freq) {
        freqNorm.push_back(f / nyq);
    }
    
    // 입력 검증
    if (freq.size() != gain.size()) {
        throw std::runtime_error("freq and gain must be of same length");
    }
    
    // JavaScript와 동일한 nfreqs 계산
    // 2의 거듭제곱 + 1, numtaps보다 큼
    int nfreqs = 1;
    while (nfreqs < numtaps) {
        nfreqs *= 2;
    }
    nfreqs = nfreqs + 1;
    
    // 균일한 주파수 그리드 생성 (0 to 1)
    std::vector<float> x(nfreqs);
    for (int i = 0; i < nfreqs; ++i) {
        x[i] = static_cast<float>(i) / static_cast<float>(nfreqs - 1);
    }
    
    // 보간된 주파수 응답
    std::vector<std::complex<float>> fx(nfreqs);
    
    for (int i = 0; i < nfreqs; ++i) {
        float xi = x[i];
        float yi = 0.0f;
        
        // 경계 처리
        if (xi <= freqNorm[0]) {
            yi = gain[0];
        } else if (xi >= freqNorm.back()) {
            yi = gain.back();
        } else {
            // 선형 보간
            for (size_t j = 0; j < freqNorm.size() - 1; ++j) {
                if (xi >= freqNorm[j] && xi <= freqNorm[j + 1]) {
                    float t = (xi - freqNorm[j]) / (freqNorm[j + 1] - freqNorm[j]);
                    yi = gain[j] * (1.0f - t) + gain[j + 1] * t;
                    break;
                }
            }
        }
        
        fx[i] = std::complex<float>(yi, 0.0f);
    }
    
    // IRFFT로 임펄스 응답 생성
    std::vector<float> h = irfft(fx);
    
    // numtaps 크기로 자르기
    if (h.size() > numtaps) {
        h.resize(numtaps);
    }
    
    // Shift하여 중앙에 배치
    std::vector<float> impulse(numtaps);
    int shift = numtaps / 2;
    for (int i = 0; i < numtaps; ++i) {
        impulse[i] = h[(i + shift) % h.size()];
    }
    
    // Window 적용 (Hamming)
    for (int i = 0; i < numtaps; ++i) {
        float window = 0.54f - 0.46f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (numtaps - 1));
        impulse[i] *= window;
    }
    
    // 정규화
    float sum = 0.0f;
    for (float val : impulse) {
        sum += val;
    }
    if (sum != 0.0f) {
        for (float& val : impulse) {
            val /= sum;
        }
    }
    
    return impulse;
}
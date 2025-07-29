# Loudness Compensator JUCE Plugin

ISO 226:2003 기반 등청감 보정 AU/VST3 플러그인 (JUCE 버전)

## 특징

- ✅ WebTidalLoudness의 정확한 DSP 알고리즘 포팅
- ✅ AU (Audio Unit) 및 VST3 포맷 지원
- ✅ 직관적인 UI와 Expert Mode
- ✅ 저지연 실시간 처리
- ✅ 프리셋 시스템

## 빌드 방법

### 요구사항

- macOS 10.13 이상
- Xcode 12 이상
- JUCE 7.0 이상 (상위 폴더에 설치됨)

### 빌드 단계

```bash
# 1. 빌드 스크립트 실행
./build_juce.sh

# 또는 Projucer 사용
1. Projucer로 LoudnessCompensator.jucer 열기
2. "Save and Open in IDE" 클릭
3. Xcode에서 빌드 (Cmd+B)
```

## 사용 방법

### DAW에서 사용

1. DAW 실행 (Logic Pro, Ableton Live, Reaper 등)
2. 플러그인 스캔 또는 재스캔
3. 이펙트 추가에서 "Hyang > Loudness Compensator" 선택

### UI 컨트롤

#### Easy Mode (기본)
- **큰 원형 노브**: Loudness 레벨 (0-100%)
  - 실시간으로 phon과 SPL 표시
  - Target/Reference phon 값 표시

#### 버튼
- **BYPASS**: 처리 우회
- **Preset 선택**: 5개의 팩토리 프리셋
- **Expert Mode**: 고급 컨트롤 표시/숨김

#### Expert Mode
- **K Value**: Exponential decay rate 조정
- **Delta Max**: 최대 phon 차이 설정
- **Filter Quality**: FIR 필터 품질 선택

## 기술적 세부사항

### DSP 구현

1. **정확한 JavaScript 포팅**
   - firwin2 알고리즘 100% 재현
   - IRFFT 구현 동일
   - ISO 보간 로직 동일

2. **JUCE DSP 모듈 활용**
   - juce::dsp::Convolution 사용
   - 하드웨어 가속 지원
   - 효율적인 메모리 관리

3. **파라미터 자동화**
   - 모든 파라미터 DAW 자동화 가능
   - 부드러운 파라미터 변경
   - 상태 저장/복원

### 성능

- CPU: ~2-4% (M1 Mac, 48kHz, 512 samples)
- 레이턴시: 5.3ms (511 taps) ~ 42.7ms (4095 taps)
- 메모리: ~15MB per instance

## 프리셋 설명

1. **Studio Nearfield** (65 phon)
   - 일반적인 스튜디오 모니터링 레벨

2. **Home Listening** (60 phon)
   - 가정용 청취 환경

3. **Late Night** (40 phon)
   - 조용한 야간 청취
   - K값과 Delta Max 조정됨

4. **Headphones** (55 phon)
   - 헤드폰 청취 최적화

5. **Reference 83** (86 phon)
   - 마스터링 참조 레벨
   - 높은 필터 품질

## 문제 해결

### 플러그인이 보이지 않는 경우

```bash
# AU 재스캔
killall -9 AudioComponentRegistrar
auval -a

# Logic Pro 플러그인 캐시 초기화
rm -rf ~/Library/Caches/AudioUnitCache
```

### 크래시 또는 오류

1. 최신 버전 확인
2. DAW 호환성 확인
3. 샘플레이트 설정 확인 (48kHz 권장)

## 개발 노트

이 플러그인은 WebTidalLoudness의 핵심 DSP 엔진을 JUCE로 포팅한 것입니다.
"1도라도 다르면 안 되는" 정확성을 유지하면서 네이티브 성능을 제공합니다.

## 라이선스

MIT License

---

**정확한 등청감 보정으로 모든 볼륨에서 완벽한 음악을 즐기세요!**
# Docker를 사용한 Linux 빌드 가이드

## 1. Docker Desktop 설치 완료 후

### 1-1. Docker Desktop 실행
```bash
open -a Docker
```

### 1-2. Docker 상태 확인
```bash
docker --version
docker info
```

## 2. Linux 빌드 실행

### 2-1. 기본 빌드
```bash
# LoudnessCompensator 디렉토리에서 실행
docker build -f Dockerfile.linux -t loudness-compensator-linux .
```

### 2-2. 빌드 아티팩트 추출
```bash
# 빌드 결과를 로컬로 복사
mkdir -p linux-build-output
docker run --rm -v "$(pwd)/linux-build-output:/output-host" loudness-compensator-linux \
  bash -c "cp -r /output/* /output-host/ 2>/dev/null && echo 'Build artifacts copied'"
```

### 2-3. 결과 확인
```bash
ls -la linux-build-output/
```

## 3. 예상 결과

빌드가 성공하면 다음 파일들이 생성됩니다:
- `VST3/LoudnessCompensator.vst3/` - VST3 플러그인
- `LV2/LoudnessCompensator.lv2/` - LV2 플러그인  
- `Standalone/LoudnessCompensator` - 독립 실행 파일

## 4. 문제 해결

### Docker Desktop이 시작되지 않는 경우:
1. macOS 시스템 환경설정에서 Docker에 권한 부여
2. Docker Desktop을 수동으로 실행: `open -a Docker`
3. Docker 데몬이 시작될 때까지 기다림 (보통 1-2분)

### 빌드 실패 시:
```bash
# 상세한 로그 확인
docker build -f Dockerfile.linux -t loudness-compensator-linux . --no-cache

# 컨테이너 내부 디버깅
docker run -it loudness-compensator-linux bash
```

## 5. 빌드 완료 후

Linux 빌드 결과를 새로운 GitHub Release에 추가:
```bash
# 패키징
cd linux-build-output
tar -czf ../LoudnessCompensator-v1.0.1-Linux.tar.gz .
cd ..

# GitHub Release에 추가
gh release upload v1.0.1 LoudnessCompensator-v1.0.1-Linux.tar.gz
```
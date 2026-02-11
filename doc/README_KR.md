# Hilt

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C23%20%7C%20C%2B%2B17-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)

**Hilt**는 [CrossWire SWORD Project](https://crosswire.org/sword/index.jsp)를 기반으로 만든 **초경량 CLI 성경 리더**입니다.

성경 한 구절 읽는데 무거운 GUI 프로그램 띄우기 싫어서 만들었습니다. **C**와 **C++17**을 사용해 그냥 빠릅니다.

## 특징 (Features)

- **빠름**: C23 네이티브 구현에 얇은 C++17 브릿지만 얹었습니다. 찍으면 바로 나옵니다.
- **유연한 검색**: `John 3:16`, `Rev 7:1-14`, `Rom 8:` 등 자유롭게 입력 가능.
- **한국어 지원**: 제가 한국인이니 일단 제가 쓰기 편해야죠. 한국어 지원합니다. `John` 대신 `요`, `Gen` 대신 `창` 입력해도 알아먹습니다.
- **깔끔한 출력**: 눈 아픈 XML 태그 싹 걷어내고, 보기 좋게 컬러 입혔습니다.
- **설정 저장**: 성경 버전 매번 명령어로 넣긴 귀찮으니까, 세팅 해놓으면 `~/.hilt/config`에 기본 성경 버전 저장됩니다. 그 후엔 따로 명령어 안줘도 그 버전으로 나와요.

## 준비물 (Dependencies)

**SWORD 라이브러리**랑 C++17 지원하는 컴파일러만 있으면 됩니다.

- **Arch Linux**: `pacman -S sword base-devel`
- **Debian/Ubuntu**: `apt install libsword-dev build-essential`
- **그 외**: 다른 배포판은 직접 테스트 못 해봤는데, SWORD 라이브러리만 깔려 있으면 알아서 잘 돌아갈 겁니다. 안되면 이슈 걸어주세요.

## 설치 (Install)

**우측 [Releases] 섹션에서 바이너리만 받아서 바로 쓰시면 됩니다.**

```bash
chmod +x hilt
sudo mv hilt /usr/local/bin/
```

## 사용법 (Usage)

### 1. 기본 버전 설정 (딱 한 번만)
매번 `KorRV` 치기 귀찮으니까 기본값으로 잡아줍니다.

```bash
hilt -s KorRV   # 개역한글 기본 설정
# 또는
hilt -s KJV     # 킹제임스 기본 설정

# 그 외에 다른 성경 버전이 설치되어있다면 그걸로 하시면 됩니다. 다만 SWORD로 설치 먼저 하시고 하세요. 없으면 hilt가 못잡습니다.
```

### 2. 성경 읽기
설정 끝났으면 그냥 보고 싶은 곳 치세요.

```bash
# 단일 절
hilt 요 3:16

# 범위 읽기 (이게 진짜 편함)
hilt 계 7:1-14

# 한 장 통째로
hilt 롬 8:

# 이번만 다른 버전으로 보고 싶을 때
hilt KJV 창 1:1
```

### 3. 유틸리티

```bash
# 시편 119편 절이 몇개인지 그리드로 출력
hilt 시 119 -l
```

## 모듈 관리 (Module Management)

Hilt는 엔진일 뿐입니다. 성경 데이터가 없으면 깡통이니 `installmgr`로 설치해 주세요.

```bash
# 리스트 갱신
sudo installmgr -init
sudo installmgr -r CrossWire

# 성경 데이터 설치
sudo installmgr -ri CrossWire KJV    # 영어 (KJV)
sudo installmgr -ri CrossWire KorRV  # 한국어 (개역한글)
```

## 구조 (Architecture)

- `src/main.c`: CLI 파싱, 메인 로직, C23
- `src/bridge.cpp`: `libsword`와 통신, C++17
- `include/bridge.h`: C 호환성을 위한 포인터 정의

## 라이선스

MIT. 마음대로 가져다 쓰세요.

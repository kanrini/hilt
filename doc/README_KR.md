# Hilt

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)

**Hilt**는 [CrossWire SWORD Project](https://crosswire.org/sword/index.jsp)를 기반으로 만든 **초경량 CLI 성경 리더**입니다.

성경 한 구절 읽는데 무거운 GUI 프로그램 띄우기 싫어서 만들었습니다. **C++17**로 구현했습니다.

## 특징 (Features)

- **빠름**: C++17에서 SWORD API를 직접 사용합니다.
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

[GitHub Releases](https://github.com/kanrini/hilt/releases)에서 운영체제에 맞는 x86-64 설치 파일을 받으세요.

### Arch Linux

```bash
sudo pacman -U ./hilt-<버전>-1-x86_64.pkg.tar.zst
```

패키지에 SWORD 엔진이 포함됩니다. 성경 모듈 ZIP을 [CrossWire](https://crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles)에서 받아 `~/.sword`에 푸세요. 압축을 풀면 해당 폴더 바로 아래에 `mods.d`와 `modules`가 있어야 합니다.

### Windows

`hilt-<버전>-windows-x86_64-setup.exe`를 실행하고 PATH 추가 옵션을 선택한 뒤 새 PowerShell 창을 여세요. 설치 파일에는 SWORD 런타임과 언어 데이터가 포함됩니다. 성경 모듈 ZIP은 [CrossWire](https://crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles)에서 별도로 받으세요.

```powershell
$moduleDir = Join-Path $env:USERPROFILE 'sword'
Expand-Archive .\KJV.zip -DestinationPath $moduleDir
[Environment]::SetEnvironmentVariable('SWORD_PATH', $moduleDir, 'User')
$env:SWORD_PATH = $moduleDir
hilt -s KJV
hilt John 3:16
```

압축을 풀면 `$moduleDir` 바로 아래에 `mods.d`와 `modules` 폴더가 있어야 합니다.

### 릴리스 게시

릴리스 워크플로가 포함된 커밋에 `1.1.0` 또는 `v1.1.0` 형식의 새 태그를 붙여 푸시하세요. GitHub Actions가 두 설치 파일을 모두 빌드하고 검사한 뒤 `SHA256SUMS`와 함께 같은 릴리스에 게시합니다. Actions에서 워크플로를 수동 실행하면 릴리스 게시 없이 패키징 결과를 내려받을 수 있습니다.


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

- src/main.cpp에서 CLI 인자를 처리하고 구절을 출력합니다.
- src/query.cpp에서 SWORD로 성경 참조를 해석합니다.
- src/config.cpp에서 기본 모듈 설정을 읽고 씁니다.

## 라이선스

MIT. 마음대로 가져다 쓰세요.

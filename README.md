# Hilt

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)

**Hilt** is a minimal, blazing fast CLI Bible reader based on the [CrossWire SWORD Project](https://crosswire.org/sword/index.jsp).

I hate bloated GUI apps, so I made this. It uses **C++17** and the SWORD C++ API. No nonsense, just scripture in your terminal.

[한국어 리드미](https://github.com/kanrini/hilt/blob/main/doc/README_KR.md)

## Features

- **Speed**: A native C++17 command-line application.
- **Flexible**: `John 3:16`, `Rev 7:1-14`, or `Rom 8:` — Hilt gets it.
- **Korean Support**: I'm Korean, so I added native abbreviations (e.g., `요` -> `John`).
- **Clean Output**: No XML/OSIS garbage. Just clean, colored text.
- **Config**: Save your default version in `~/.hilt/config`. Set it and forget it.

## Dependencies

You just need the **SWORD library** and a decent C++17 compiler.

- **Arch Linux**: `pacman -S sword base-devel`
- **Debian/Ubuntu**: `apt install libsword-dev build-essential`
- **Others**: I couldn't check out on other distros, but it will just work if you have the SWORD library installed. If not working, then open the Issue about it please.

## Install

Download the x86-64 installer for your system from [GitHub Releases](https://github.com/kanrini/hilt/releases).

### Arch Linux

```bash
sudo pacman -U ./hilt-<version>-1-x86_64.pkg.tar.zst
```

The package includes the SWORD engine. Install a Bible module from [CrossWire](https://crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles) by extracting its ZIP into ~/.sword, with mods.d and modules directly inside.

### Windows

Run `hilt-<version>-windows-x86_64-setup.exe` and select the option to add Hilt to PATH. Open a new PowerShell window after installation. The installer includes the SWORD runtime and locale data. Download a Bible module separately from [CrossWire](https://crosswire.org/sword/modules/ModDisp.jsp?modType=Bibles).

```powershell
$moduleDir = Join-Path $env:USERPROFILE 'sword'
Expand-Archive .\KJV.zip -DestinationPath $moduleDir
[Environment]::SetEnvironmentVariable('SWORD_PATH', $moduleDir, 'User')
$env:SWORD_PATH = $moduleDir
hilt -s KJV
hilt John 3:16
```

Keep the extracted `mods.d` and `modules` directories directly under `$moduleDir`.

### Publishing a release

Push a new `MAJOR.MINOR.PATCH` tag (an optional `v` prefix is supported) on a commit containing the release workflow. GitHub Actions builds and tests both x86-64 installers, then publishes them with `SHA256SUMS` to the same release. Run the workflow manually from Actions to test packaging and download the artifacts without publishing a release.


## Usage

### Configuration

Tell Hilt which Bible module to use (do this once).

```bash
hilt -s KorRV   # Set Korean Revised Version as default
# or
hilt -s KJV     # Set King James Version as default

# You can use any other installed version.
# Just make sure to install it via SWORD first, or Hilt won't find it.
```

### Reading

Just type what you want to see.

```bash
# Single verse
hilt John 3:16

# Verse range
hilt Rev 7:1-14

# Full chapter
hilt Rom 8:

# Override version just for this command
hilt KJV Gen 1:1

# no need upper/lower case classified
```

### Utilities

```bash
# Grid view (Check verse counts)
hilt Ps 119 -l
```

## Module Management

Hilt is just the engine. You need Bible modules. Use `installmgr`.

```bash
# Init
sudo installmgr -init
sudo installmgr -r CrossWire

# Install
sudo installmgr -ri CrossWire KJV    # English
sudo installmgr -ri CrossWire KorRV  # Korean
```

## Architecture

Separation of concerns. Simple as that.

- src/main.cpp processes CLI arguments and displays verses.
- src/query.cpp parses references with SWORD.
- src/config.cpp reads and writes the default module.

## License

MIT. Do whatever you want with it.

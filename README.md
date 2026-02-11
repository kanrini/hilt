# Hilt

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C23%20%7C%20C%2B%2B17-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)

**Hilt** is a minimal, blazing fast CLI Bible reader based on the [CrossWire SWORD Project](https://crosswire.org/sword/index.jsp).

I hate bloated GUI apps, so I made this. It bridges modern **C (C23)** and **C++17** to give you raw speed. No nonsense, just scripture in your terminal.

[한국어 리드미](https://github.com/kanrini/hilt/blob/main/doc/README_KR.md)

## Features

- **Speed**: Native C23 with a thin C++17 layer. It flies.
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

**Just grab the binary from the [Releases] section on the right.**

```bash
chmod +x hilt
sudo mv hilt /usr/local/bin/
```

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

- `src/main.c`: CLI parsing, C23.
- `src/bridge.cpp`: Talks to libsword, C++17.
- `include/bridge.h`: Opaque pointers.

## License

MIT. Do whatever you want with it.

# Changelog

All notable changes to the "Hilt" project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## TODO

- Not planned any further yet.

## [1.0.0] - 2026-02-11

### Added
- **Initial Release:** Core CLI architecture implemented in C (C23) with C++ Bridge for `libsword`.
- **Smart Querying:**
    - Support for explicit version selection (e.g., `hilt KJV John 3:16`).
    - Support for implicit default version (e.g., `hilt John 3:16`).
- **Configuration Management:**
    - `-s` flag to set and save the default Bible module (e.g., `hilt -s KorRV`).
    - Configuration saved locally in `~/.hilt/config`.
- **Range & Iterator Support:**
    - Ability to fetch verse ranges (e.g., `Rev 7:1-14`) and entire chapters.
    - Implemented `SwordIterator` pattern for efficient memory handling and traversal.
- **Locale Support:**
    - Integrated Korean locale setting to support book abbreviations (e.g., `요` -> `John`).
- **CLI Options:**
    - `-l` flag for "Listing Mode" (displays verse numbers only in a grid).
- **Output Formatting:**
    - ANSI color support for better readability (Cyan for headers, Red for errors).
    - Clean text output (stripped OSIS/XML tags).
    - Verse formatting style set to `:Verse Content`.
- **Build System:**
    - Added `Makefile` for standard GCC/G++ compilation.
    - Added `CMakeLists.txt` for IDE integration (CLion/VSCode).
- **Testing:**
    - Added unit test suite (`tests/test_bridge.c`) verifying engine init, single fetch, and range queries.

### Technical
- Implemented C++ shim (`bridge.cpp`) to interface with `libsword` classes (`SWMgr`, `SWModule`, `VerseKey`, `ListKey`).
- Used `pkg-config` for dynamic library resolution.
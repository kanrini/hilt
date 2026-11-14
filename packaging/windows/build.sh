#!/usr/bin/env bash
set -euo pipefail

source packaging/build-sword.sh

export PATH="$sword_prefix/bin:$PATH"
export PKG_CONFIG_PATH="$(cygpath -m "$sword_prefix/lib/pkgconfig")"
hilt_options=(
    -DCMAKE_BUILD_TYPE=Release
    "-DHILT_VERSION=$HILT_VERSION"
    "-DHILT_SWORD_ARCHIVE=$(cygpath -m "$PWD/$archive")"
    "-DHILT_SWORD_SOURCE=$(cygpath -m "$sword_source")"
    "-DHILT_RUNTIME_DIRS=$(cygpath -m "$sword_prefix/bin");$(cygpath -m "$MINGW_PREFIX/bin")"
)
cmake -S . -B build/hilt -G Ninja "${hilt_options[@]}"
cmake --build build/hilt

curl --fail --location --retry 3 https://crosswire.org/ftpmirror/pub/sword/packages/rawzip/KJV.zip -o build/KJV.zip
mkdir -p build/sword-data
unzip -oq build/KJV.zip -d build/sword-data
curl --fail --location --retry 3 https://crosswire.org/ftpmirror/pub/sword/packages/rawzip/KorRV.zip -o build/KorRV.zip
unzip -oq build/KorRV.zip -d build/sword-data
SWORD_PATH="$(cygpath -m "$PWD/build/sword-data")" ctest --test-dir build/hilt --output-on-failure
cpack --config build/hilt/CPackConfig.cmake -B build/release

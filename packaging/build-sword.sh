version=1.9.0
archive="build/sword-$version.tar.gz"
mkdir -p build
curl --fail --location --retry 3 "https://crosswire.org/ftpmirror/pub/sword/source/v1.9/sword-$version.tar.gz" -o "$archive"
echo "42409cf3de2faf1108523e2c5ac0745d21f9ed2a5c78ed878ee9dcc303426b8a  $archive" | sha256sum -c -
tar --exclude="sword-$version/bindings/Android" -xf "$archive" -C build

sword_source="$PWD/build/sword-$version"
sword_prefix="$PWD/build/sword-prefix"
sword_install_prefix="$sword_prefix"
sword_no_curl=Yes
if [[ ${MSYSTEM:-} == UCRT64 ]]; then
    sword_no_curl=No
    sword_install_prefix="$(cygpath -m "$sword_prefix")"
    # SWORD 1.9.0 casts pointers to unsigned long, which is 32-bit on Win64.
    for filter in "$sword_source"/src/modules/filters/*.cpp; do
        if grep -qF '(unsigned long)key' "$filter"; then
            sed -i '1i#include <cstdint>' "$filter"
            sed -i 's/(unsigned long)key/reinterpret_cast<std::uintptr_t>(key)/g' "$filter"
        fi
    done
    sed -i 's/(unsigned long)datfd > 0/datfd != nullptr/; s/(unsigned long)idxfd > 0/idxfd != nullptr/' "$sword_source/src/modules/common/rawstr4.cpp"
fi
sword_options=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_C_STANDARD=99
    -DCMAKE_CXX_STANDARD=17
    -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON
    "-DCMAKE_INSTALL_PREFIX=$sword_install_prefix"
    -DLIBSWORD_LIBRARY_TYPE=Shared
    -DSWORD_BUILD_UTILS=No
    "-DSWORD_NO_CURL=$sword_no_curl"
    -DSWORD_NO_CLUCENE=Yes
)
cmake -S "$sword_source" -B build/sword-build -G Ninja "${sword_options[@]}"
cmake --build build/sword-build
cmake --install build/sword-build

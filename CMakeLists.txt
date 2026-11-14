cmake_minimum_required(VERSION 3.20)
project(hilt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SWORD REQUIRED IMPORTED_TARGET sword)

pkg_check_modules(INSTALL_DEPS REQUIRED IMPORTED_TARGET libcurl libarchive)

add_executable(hilt src/main.cpp src/config.cpp src/query.cpp src/install.cpp src/update.cpp)
target_include_directories(hilt PRIVATE include)
target_link_libraries(hilt PRIVATE PkgConfig::SWORD PkgConfig::INSTALL_DEPS)

add_executable(run_tests tests/test_query.cpp src/query.cpp)
target_include_directories(run_tests PRIVATE include)
target_link_libraries(run_tests PRIVATE PkgConfig::SWORD)

enable_testing()
add_test(NAME sword_query COMMAND run_tests)

if(WIN32)
    enable_language(RC)
    target_include_directories(hilt PRIVATE packaging/windows)
    target_sources(hilt PRIVATE packaging/windows/hilt.rc)
    target_link_libraries(hilt PRIVATE shell32)
endif()

if(MINGW)
    target_link_options(hilt PRIVATE -municode)
endif()

set(HILT_VERSION "0.0.0" CACHE STRING "Version used for release packaging")
target_compile_definitions(hilt PRIVATE HILT_BUILD_VERSION="${HILT_VERSION}")
set(HILT_SWORD_SOURCE "" CACHE PATH "Source tree for the bundled SWORD library")
set(HILT_SWORD_ARCHIVE "" CACHE FILEPATH "Original source archive for the bundled SWORD library")
set(HILT_RUNTIME_DIRS "" CACHE STRING "Directories containing Windows runtime DLLs")
if(WIN32 AND HILT_SWORD_SOURCE)
    install(TARGETS hilt RUNTIME_DEPENDENCY_SET hilt_runtime RUNTIME DESTINATION bin)
    install(RUNTIME_DEPENDENCY_SET hilt_runtime
        DIRECTORIES ${HILT_RUNTIME_DIRS}
        PRE_EXCLUDE_REGEXES "^api-ms-" "^ext-ms-"
        POST_EXCLUDE_REGEXES "[/\\\\][Ss][Yy][Ss][Tt][Ee][Mm]32[/\\\\]"
        RUNTIME DESTINATION bin)
    install(DIRECTORY "${HILT_SWORD_SOURCE}/locales.d" DESTINATION bin)
    install(FILES LICENSE DESTINATION share/licenses/hilt)
    install(FILES "${HILT_SWORD_SOURCE}/LICENSE" DESTINATION share/licenses/sword)
    if(HILT_SWORD_ARCHIVE)
        install(FILES "${HILT_SWORD_ARCHIVE}" DESTINATION share/source)
    endif()
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/hilt.ico")
    set(CPACK_GENERATOR NSIS)
    set(CPACK_PACKAGE_NAME "Hilt")
    set(CPACK_PACKAGE_VENDOR "Heyday")
    set(CPACK_PACKAGE_VERSION "${HILT_VERSION}")
    set(CPACK_PACKAGE_FILE_NAME "hilt-${HILT_VERSION}-windows-x86_64-setup")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Hilt")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY bin)
    include(CPack)
else()
    install(TARGETS hilt RUNTIME DESTINATION bin)
endif()

option(HILT_BUNDLED_SWORD "Use an application-private SWORD deployment on Linux" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND HILT_BUNDLED_SWORD)
    target_compile_definitions(hilt PRIVATE HILT_LOCALE_DIR="/usr/share/hilt/locales.d")
    set_target_properties(hilt PROPERTIES BUILD_WITH_INSTALL_RPATH ON
        INSTALL_RPATH "$ORIGIN/../lib/hilt")
endif()

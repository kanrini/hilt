cmake_minimum_required(VERSION 3.20)
project(hilt C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

# Add 'include' directory to header search path globally
include_directories(include)

# Find SWORD library via PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(SWORD REQUIRED sword)

# ==========================================
# 1. Main Executable (hilt)
# ==========================================
add_executable(hilt
        src/main.c
        src/bridge.cpp
        src/config.c
)

target_include_directories(hilt PRIVATE ${SWORD_INCLUDE_DIRS})
target_link_libraries(hilt PRIVATE ${SWORD_LIBRARIES})

# ==========================================
# 2. Test Executable (run_tests)
# ==========================================
# Tests need 'test_bridge.c' AND the implementation 'bridge.cpp'
add_executable(run_tests
        tests/test_bridge.c
        src/bridge.cpp
)

target_include_directories(run_tests PRIVATE ${SWORD_INCLUDE_DIRS})
target_link_libraries(run_tests PRIVATE ${SWORD_LIBRARIES})
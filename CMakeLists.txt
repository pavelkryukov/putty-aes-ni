project(putty-aes-ni LANGUAGES C)
cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

ExternalProject_Add(Putty
    GIT_REPOSITORY git://git.tartarus.org/simon/putty.git
    STEP_TARGETS builds
    BINARY_DIR ${CMAKE_BINARY_DIR}/Putty
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/Putty
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    EXCLUDE_FROM_ALL TRUE
)

include_directories(${PROJECT_SOURCE_DIR}/Putty)
link_directories(${CMAKE_BINARY_DIR}/Putty)

function(add_crypto_executable)
    add_executable(${ARGV})
    target_link_libraries(${ARGV0} crypto utils)
    add_dependencies(${ARGV0} Putty)
endfunction()

add_executable(aescpuid aescpuid.c)

add_crypto_executable(sshaes-perf aesperf.c)
add_crypto_executable(sshaes-perf-ni aesperf.c)
target_compile_definitions(sshaes-perf-ni PUBLIC _HW_AES)

add_crypto_executable(sshaes-demo-sw-encode aesdemo.c)
add_crypto_executable(sshaes-demo-hw-encode aesdemo.c)
add_crypto_executable(sshaes-demo-sw-decode aesdemo.c)
add_crypto_executable(sshaes-demo-hw-decode aesdemo.c)
target_compile_definitions(sshaes-demo-hw-encode PUBLIC _HW_AES)
target_compile_definitions(sshaes-demo-sw-decode PUBLIC DECODE)
target_compile_definitions(sshaes-demo-hw-decode PUBLIC DECODE _HW_AES)


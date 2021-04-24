project(putty-aes-ni LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)

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
    add_executable(${ARGV} intf.c)
    target_link_libraries(${ARGV0} crypto utils)
    add_dependencies(${ARGV0} Putty)
endfunction()

add_executable(aescpuid aescpuid.c)

add_crypto_executable(aesperf benchmark.cpp)
target_link_libraries(aesperf benchmark pthread rt)

add_crypto_executable(aes-demo-sw-encode demo.c)
add_crypto_executable(aes-demo-hw-encode demo.c)
add_crypto_executable(aes-demo-sw-decode demo.c)
add_crypto_executable(aes-demo-hw-decode demo.c)
target_compile_definitions(aes-demo-hw-encode PUBLIC _HW_AES)
target_compile_definitions(aes-demo-sw-decode PUBLIC DECODE)
target_compile_definitions(aes-demo-hw-decode PUBLIC DECODE _HW_AES)


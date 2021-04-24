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

add_library(aes intf.c)
add_dependencies(aes Putty)
target_link_libraries(aes crypto utils)

add_executable(aescpuid aescpuid.c)

add_executable(aesperf benchmark.cpp)
target_link_libraries(aesperf aes benchmark pthread rt)

function(add_demo_executable)
    add_executable(${ARGV0} demo.c)
    target_link_libraries(${ARGV0} aes)
    target_compile_definitions(${ARGV})
endfunction()

add_demo_executable(aes-demo-sw-encode PUBLIC ENCODE)
add_demo_executable(aes-demo-hw-encode PUBLIC ENCODE _HW_AES)
add_demo_executable(aes-demo-sw-decode PUBLIC DECODE)
add_demo_executable(aes-demo-hw-decode PUBLIC DECODE _HW_AES)

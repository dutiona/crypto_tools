cmake_minimum_required(VERSION 3.14)

project(crypto_toolsTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/windows-set-path.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(crypto_tools REQUIRED)
  enable_testing()
endif()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG "v3.0.0-preview5"
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG "8.1.1"
)

FetchContent_MakeAvailable(Catch2 fmt)

add_executable(prime_test source/prime.cpp)
add_executable(shamir_test source/shamir.cpp)

target_link_libraries(prime_test
                      PRIVATE
                      crypto_tools::crypto_tools
                      Catch2::Catch2WithMain
                      fmt
                      )
target_compile_features(prime_test PRIVATE cxx_std_23)
add_test(NAME prime_test COMMAND prime_test)
windows_set_path(prime_test crypto_tools::crypto_tools)

target_link_libraries(shamir_test
                      PRIVATE
                      crypto_tools::crypto_tools
                      Catch2::Catch2WithMain
                      fmt
                      )
target_compile_features(shamir_test PRIVATE cxx_std_23)
add_test(NAME shamir_test COMMAND shamir_test)
windows_set_path(shamir_test crypto_tools::crypto_tools)

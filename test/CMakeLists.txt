cmake_minimum_required(VERSION 3.14)

project(crypto_toolsTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/windows-set-path.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(crypto_tools REQUIRED)
  enable_testing()
endif()

add_executable(crypto_tools_test source/crypto_tools_test.cpp)

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

target_link_libraries(crypto_tools_test
                      PRIVATE
                      crypto_tools::crypto_tools
                      Catch2::Catch2WithMain
                      fmt
                      )

target_compile_features(crypto_tools_test PRIVATE cxx_std_23)

add_test(NAME crypto_tools_test COMMAND crypto_tools_test)
windows_set_path(crypto_tools_test crypto_tools::crypto_tools)

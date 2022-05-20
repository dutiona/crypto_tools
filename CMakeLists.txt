cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    crypto_tools
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://github.com/dutiona/crypto_tools"
    LANGUAGES CXX
)

docs_early_return()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    crypto_tools_crypto_tools
    source/prime.cpp
    source/shamir.cpp
)
add_library(crypto_tools::crypto_tools ALIAS crypto_tools_crypto_tools)

include(GenerateExportHeader)
generate_export_header(
    crypto_tools_crypto_tools
    BASE_NAME crypto_tools
    EXPORT_FILE_NAME export/crypto_tools/crypto_tools_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(crypto_tools_crypto_tools PUBLIC CRYPTO_TOOLS_STATIC_DEFINE)
endif()

set_target_properties(
    crypto_tools_crypto_tools PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME crypto_tools
    OUTPUT_NAME crypto_tools
)

target_include_directories(
    crypto_tools_crypto_tools ${crypto_tools_warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    crypto_tools_crypto_tools SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

include(FetchContent)

FetchContent_Declare(
    expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected
    GIT_TAG "v1.0.0"
)

FetchContent_Declare(
    boostorg
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG "boost-1.79.0"
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG "8.1.1"
)

FetchContent_MakeAvailable(expected boostorg fmt)

target_link_libraries(crypto_tools_crypto_tools PUBLIC expected boost_multiprecision)
target_link_libraries(crypto_tools_crypto_tools PRIVATE fmt boost_random)

target_compile_features(crypto_tools_crypto_tools PUBLIC cxx_std_23)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT crypto_tools_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of crypto_tools"
  )
endif()

include(cmake/dev-mode.cmake)

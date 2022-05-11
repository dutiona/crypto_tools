if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/crypto_tools CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package crypto_tools)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT crypto_tools_Development
)

install(
    TARGETS crypto_tools_crypto_tools
    EXPORT crypto_toolsTargets
    RUNTIME #
    COMPONENT crypto_tools_Runtime
    LIBRARY #
    COMPONENT crypto_tools_Runtime
    NAMELINK_COMPONENT crypto_tools_Development
    ARCHIVE #
    COMPONENT crypto_tools_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    crypto_tools_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(crypto_tools_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${crypto_tools_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT crypto_tools_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${crypto_tools_INSTALL_CMAKEDIR}"
    COMPONENT crypto_tools_Development
)

install(
    EXPORT crypto_toolsTargets
    NAMESPACE crypto_tools::
    DESTINATION "${crypto_tools_INSTALL_CMAKEDIR}"
    COMPONENT crypto_tools_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()

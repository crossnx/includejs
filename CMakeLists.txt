cmake_minimum_required(VERSION 3.24)
project(includejs VERSION 0.0.1 LANGUAGES C CXX
  DESCRIPTION "Build your own JavaScript runtime"
  HOMEPAGE_URL "https://www.includejs.org")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(vendor/noa/cmake/noa.cmake)
include(cmake/CompilerOptions.cmake)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# Options
option(INCLUDEJS_ENGINE "Build the IncludeJS engine library" ON)
option(INCLUDEJS_TESTS "Build the IncludeJS tests" OFF)
option(INCLUDEJS_DOCS "Build the IncludeJS docs" OFF)
option(INCLUDEJS_INSTALL "Install the IncludeJS library" ON)
option(INCLUDEJS_ADDRESS_SANITIZER "Build IncludeJS with an address sanitizer" OFF)
option(INCLUDEJS_UNDEFINED_SANITIZER "Build IncludeJS with an undefined behavior sanitizer" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  COMPATIBILITY SameMajorVersion)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT sourcemeta_includejs_dev)

if(INCLUDEJS_ENGINE)
  find_package(JavaScriptCore REQUIRED)
  add_subdirectory(src/engine)
endif()

if(INCLUDEJS_ADDRESS_SANITIZER)
  noa_sanitizer(TYPE address)
elseif(INCLUDEJS_UNDEFINED_SANITIZER)
  noa_sanitizer(TYPE undefined)
endif()

if(INCLUDEJS_DOCS)
  noa_target_doxygen(CONFIG "${PROJECT_SOURCE_DIR}/doxygen/Doxyfile.in"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/website")
endif()

if(PROJECT_IS_TOP_LEVEL)
  noa_target_clang_format(SOURCES
    src/*.h src/*.cc
    test/*.h test/*.cc)
  noa_target_clang_tidy(SOURCES
    src/*.h src/*.cc)
endif()

# Testing
if(INCLUDEJS_TESTS)
  find_package(GoogleTest REQUIRED)
  enable_testing()

  if(INCLUDEJS_ENGINE)
    add_subdirectory(test/engine)
  endif()

  # TODO: Add packaging tests
endif()

if(NOT GoogleTest_FOUND)
  include(GoogleTest)
  set(BUILD_GMOCK ON CACHE BOOL "enable googlemock")
  set(INSTALL_GTEST OFF CACHE BOOL "disable installation")
  add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/googletest")
  set(GoogleTest_FOUND ON)
endif()

# We want to disable the unsigned shift base sanitizer for gtest
# because it triggers on the gtest code.
if(INCLUDEJS_UNDEFINED_SANITIZER)
  target_compile_options(gtest PRIVATE "-fno-sanitize=unsigned-shift-base")
endif()

cmake_minimum_required(VERSION 3.18)
project(includejs_hello VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
  # We link to the v8_monolith.lib static library from just-js that is built in
  # release mode with "RuntimeLibrary" set to "MT_StaticRelease", so we are
  # adding the /MT compiler option for both the Debug and Release configs on
  # Windows.
  # Refs: https://stackoverflow.com/a/60844772
  add_compile_options(/MT)
endif()
find_package(IncludeJS REQUIRED)
add_executable(includejs_hello hello.cc)
target_link_libraries(includejs_hello PRIVATE includejs::engine)

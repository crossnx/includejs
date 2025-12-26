if(NOT V8_FOUND)
  if(APPLE)
    add_library(v8 INTERFACE IMPORTED)
    # Since the "includejs" library is compiled with the `-isysroot` flag of
    # Clang, it is unable to find the V8 library and headers which have been
    # downloaded from Homebrew.
    # Refs: https://github.com/Homebrew/homebrew-core/issues/45061#issuecomment-541420664
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
      set(HOMEBREW_PREFIX "/opt/homebrew/opt/v8")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set(HOMEBREW_PREFIX "/usr/local/opt/v8")
    endif()
    target_include_directories(v8
      INTERFACE "${HOMEBREW_PREFIX}/include")
    target_link_directories(v8
      INTERFACE "${HOMEBREW_PREFIX}/lib")
    target_link_libraries(v8 INTERFACE "-lv8")
    target_link_libraries(v8 INTERFACE "-lv8_libplatform")
    target_compile_definitions(v8
      INTERFACE V8_COMPRESS_POINTERS)
    target_compile_definitions(v8
      INTERFACE V8_ENABLE_SANDBOX)
    add_library(V8::V8 ALIAS v8)
    set(V8_FOUND ON)
  endif()
endif()

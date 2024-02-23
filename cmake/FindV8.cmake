if(NOT V8_FOUND)
  if(APPLE)
    add_library(v8 INTERFACE IMPORTED)
    # Since the "includejs" library is compiled with the `-isysroot` flag of
    # Clang, it is unable to find the V8 library and headers which have been
    # downloaded from Homebrew.
    # Refs: https://github.com/Homebrew/homebrew-core/issues/45061#issuecomment-541420664
    set(V8_VERSION "12.1.285.24")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
      set(HOMEBREW_CELLAR "/opt/homebrew/Cellar")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set(HOMEBREW_CELLAR "/usr/local/Cellar")
    endif()
    target_include_directories(v8
      INTERFACE "${HOMEBREW_CELLAR}/v8/${V8_VERSION}/include")
    target_link_directories(v8
      INTERFACE "${HOMEBREW_CELLAR}/v8/${V8_VERSION}/lib")
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

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
  elseif(WIN32)
    include(FetchContent)

    set(V8_VERSION 12.3)

    # This downloads the include.tar.gz file from
    # https://github.com/just-js/v8/releases/tag/12.3
    FetchContent_Declare(
      v8_include
      URL https://github.com/just-js/v8/releases/download/${V8_VERSION}/include.tar.gz
      URL_HASH SHA256=cd6b891c30ecd28ccc254a90f33f92ac2d69f822fbfd54043ed74b57bf12c073
      DOWNLOAD_EXTRACT_TIMESTAMP YES # To silence a CMake warning.
      )

    # This downloads the libv8_monolith-win-x64.zip file from
    # https://github.com/just-js/v8/releases/tag/12.3
    FetchContent_Declare(
      v8_lib
      URL https://github.com/just-js/v8/releases/download/${V8_VERSION}/libv8_monolith-win-x64.zip
      URL_HASH SHA256=f80c7b3eb4580aa6fec5a8ec616fc9ada07acf7833d1b9c23b2586838e531960
      DOWNLOAD_EXTRACT_TIMESTAMP YES # To silence a CMake warning.
      )

    FetchContent_MakeAvailable(v8_include v8_lib)

    add_library(v8 INTERFACE)

    # These must be defined, otherwise V8 would crash because of assertion errors.
    # Refs: https://github.com/just-js/lo/blob/16b5b5daa21f129007177eb4c9aa60b72c74b704/Makefile#L17
    target_compile_definitions(v8 INTERFACE
      V8_COMPRESS_POINTERS
      V8_TYPED_ARRAY_MAX_SIZE_IN_HEAP=0
      # https://chromium-review.googlesource.com/c/v8/v8/+/5032576 introduced this
      # line
      # static_assert(I::IsValidSmi(std::numeric_limits<uint16_t>::min()));
      # which confuses the MSVC compiler and causes
      # https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4458?view=msvc-170.
      # Refs: https://stackoverflow.com/a/1904659
      NOMINMAX)

    # The CMAKE_INSTALL_*DIR variables become undefined during the
    # find_package(V8) call while running the packaging test, so we are using
    # the hard coded string values instead.
    target_include_directories(v8 INTERFACE
      "$<BUILD_INTERFACE:${v8_include_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:include/v8/include>")

    install(DIRECTORY "${v8_include_SOURCE_DIR}/include"
      DESTINATION "include/v8"
      COMPONENT includejs_dev)

    install(FILES "${v8_lib_SOURCE_DIR}/v8_monolith.lib"
      DESTINATION "lib"
      COMPONENT includejs_dev)

    target_link_directories(v8 INTERFACE
      "$<BUILD_INTERFACE:${v8_lib_SOURCE_DIR}>"
      "$<INSTALL_INTERFACE:lib>")

    target_link_libraries(v8 INTERFACE
      v8_monolith.lib
      # V8 depends on the following system libraries, so these must be linked.
      # Refs: https://github.com/just-js/lo/blob/16b5b5daa21f129007177eb4c9aa60b72c74b704/Makefile#L86
      winmm.lib
      dbghelp.lib
      advapi32.lib)

    install(TARGETS v8
      EXPORT v8
      PUBLIC_HEADER DESTINATION "include/v8"
        COMPONENT includejs_dev
      ARCHIVE DESTINATION "lib"
        COMPONENT includejs_dev)
    install(EXPORT v8
      DESTINATION "share/cmake/starship"
      COMPONENT includejs_dev)
    add_library(V8::V8 ALIAS v8)
    set(V8_FOUND ON)
  endif()
endif()

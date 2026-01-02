if(MSVC)
  # We link to the v8_monolith.lib static library from just-js that is built in
  # release mode with "RuntimeLibrary" set to "MT_StaticRelease", so we are
  # adding the /MT compiler option for both the Debug and Release configs on
  # Windows.
  # Refs: https://stackoverflow.com/a/60844772
  add_compile_options(/MT)
endif()

set(INCLUDEJS_ENGINE_SOURCES
  common/engine.cc
  common/engine_context.cc
  common/engine_error.cc)

if(INCLUDEJS_BACKEND STREQUAL "JavaScriptCore")
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_context.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_promise.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_value.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_private_data.cc)
elseif(INCLUDEJS_BACKEND STREQUAL "V8")
  list(APPEND INCLUDEJS_ENGINE_SOURCES v8/engine.cc)
else()
  message(FATAL_ERROR "Unknown IncludeJS backend: ${INCLUDEJS_BACKEND}")
endif()

noa_library(PROJECT includejs NAME engine
  FOLDER "IncludeJS/Engine"
  PRIVATE_HEADERS context.h function.h error.h promise.h value.h private_data.h
  SOURCES ${INCLUDEJS_ENGINE_SOURCES})

if(INCLUDEJS_INSTALL)
  noa_library_install(PROJECT includejs NAME engine)
  if(INCLUDEJS_BACKEND STREQUAL "JavaScriptCore")
    install(FILES
      "${PROJECT_SOURCE_DIR}/cmake/FindJavaScriptCore.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/includejs"
      COMPONENT includejs_dev)
  elseif(INCLUDEJS_BACKEND STREQUAL "V8")
    install(FILES
      "${PROJECT_SOURCE_DIR}/cmake/FindV8.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/includejs"
      COMPONENT includejs_dev)
  endif()
endif()

includejs_add_compile_options(includejs_engine)

if(INCLUDEJS_BACKEND STREQUAL "JavaScriptCore")
  find_package(JavaScriptCore REQUIRED)
  target_link_libraries(includejs_engine
    PRIVATE JavaScriptCore::JavaScriptCore)
  target_compile_definitions(includejs_engine
    PUBLIC INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
elseif(INCLUDEJS_BACKEND STREQUAL "V8")
  find_package(V8 REQUIRED)
  target_link_libraries(includejs_engine
    PRIVATE V8::V8)
  target_compile_definitions(includejs_engine
    PUBLIC INCLUDEJS_ENGINE_V8)
else()
  message(FATAL_ERROR "Unknown IncludeJS backend: ${INCLUDEJS_BACKEND}")
endif()

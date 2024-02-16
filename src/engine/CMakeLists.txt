set(INCLUDEJS_ENGINE_SOURCES
  common/engine.cc
  common/engine_context.cc
  common/engine_error.cc)

if(INCLUDEJS_BACKEND STREQUAL "JavaScriptCore")
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_context.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_promise.cc)
  list(APPEND INCLUDEJS_ENGINE_SOURCES javascript_core/engine_value.cc)
else()
  message(FATAL_ERROR "Unknown IncludeJS backend: ${INCLUDEJS_BACKEND}")
endif()

noa_library(NAMESPACE sourcemeta PROJECT includejs NAME engine
  FOLDER "IncludeJS/Engine"
  PRIVATE_HEADERS context.h function.h error.h promise.h value.h
  SOURCES ${INCLUDEJS_ENGINE_SOURCES})
if(INCLUDEJS_INSTALL)
  noa_library_install(NAMESPACE sourcemeta PROJECT includejs NAME engine)
endif()
sourcemeta_includejs_add_compile_options(sourcemeta_includejs_engine)

if(INCLUDEJS_BACKEND STREQUAL "JavaScriptCore")
  find_package(JavaScriptCore REQUIRED)
  target_link_libraries(sourcemeta_includejs_engine
    PRIVATE JavaScriptCore::JavaScriptCore)
  target_compile_definitions(sourcemeta_includejs_engine
    PUBLIC SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
else()
  message(FATAL_ERROR "Unknown IncludeJS backend: ${INCLUDEJS_BACKEND}")
endif()

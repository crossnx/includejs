noa_library(NAMESPACE sourcemeta PROJECT includejs NAME engine
  FOLDER "IncludeJS/Engine"
  PRIVATE_HEADERS context.h function.h error.h promise.h value.h
  SOURCES
    common/engine.cc
    common/engine_context.cc
    common/engine_error.cc
    javascript_core/engine.cc
    javascript_core/engine_context.cc
    javascript_core/engine_promise.cc
    javascript_core/engine_value.cc)

if(INCLUDEJS_INSTALL)
  noa_library_install(NAMESPACE sourcemeta PROJECT includejs NAME engine)
endif()

sourcemeta_includejs_add_compile_options(sourcemeta_includejs_engine)

find_package(JavaScriptCore REQUIRED)
target_link_libraries(sourcemeta_includejs_engine
  PRIVATE JavaScriptCore::JavaScriptCore)
target_compile_definitions(sourcemeta_includejs_engine
  PUBLIC SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)

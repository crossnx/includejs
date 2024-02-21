#ifndef SOURCEMETA_INCLUDEJS_ENGINE_FUNCTION_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_FUNCTION_H_

#include "engine_export.h"

#include <includejs/engine_context.h>
#include <includejs/engine_value.h>

#if defined(SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
#include <exception> // std::exception
#include <vector>    // std::vector
#endif

#ifdef DOXYGEN
/// @ingroup engine
#define SOURCEMETA_INCLUDEJS_ARGS
#else
#if __cplusplus >= 202002L
#include <span> // std::span
#define SOURCEMETA_INCLUDEJS_ARGS std::span<sourcemeta::includejs::Value>
#else
#define SOURCEMETA_INCLUDEJS_ARGS                                              \
  const std::vector<sourcemeta::includejs::Value> &
#endif
#endif

#if defined(SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
namespace sourcemeta {
namespace includejs {
// This is a opaque function signature that can be force-casted into
// JSObjectCallAsFunctionCallback
/// @ingroup engine
using Function = const void *(*)(const void *, const void *, const void *,
                                 const size_t, const void *[], const void **);
} // namespace includejs
} // namespace sourcemeta
#endif

#if defined(SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
#define __SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION_INTERNAL(function, call_as)     \
  static const void *function(const void *context, const void *, const void *, \
                              const size_t argc, const void *raw_arguments[],  \
                              const void **exception) {                        \
    std::vector<::sourcemeta::includejs::Value> arguments;                     \
    arguments.reserve(argc);                                                   \
    for (std::size_t index = 0; index < argc; index++) {                       \
      arguments.emplace_back(context, raw_arguments[index]);                   \
    }                                                                          \
    try {                                                                      \
      return call_as({context}, arguments).native();                           \
    } catch (const std::exception &error) {                                    \
      const ::sourcemeta::includejs::Context ignition_context{context};        \
      *exception = ignition_context.make_error(error.what()).native();         \
      return ignition_context.make_undefined().native();                       \
    }                                                                          \
  }
#endif

#ifdef DOXYGEN
/// @ingroup engine
#define SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION(function)
/// @ingroup engine
#define SOURCEMETA_INCLUDEJS_EXPOSE_TEMPLATE_FUNCTION(function)
#else
#define SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION(function)                         \
  __SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION_INTERNAL(function, function)
#define SOURCEMETA_INCLUDEJS_EXPOSE_TEMPLATE_FUNCTION(function)                \
  template <typename... Args>                                                  \
  __SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION_INTERNAL(function, function<Args...>)
#endif

#endif

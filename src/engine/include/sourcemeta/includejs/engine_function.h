#ifndef SOURCEMETA_INCLUDEJS_ENGINE_FUNCTION_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_FUNCTION_H_

#include "engine_export.h"

#include <sourcemeta/includejs/engine_context.h>
#include <sourcemeta/includejs/engine_value.h>

#if defined(SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
#include <exception> // std::exception
#include <vector>    // std::vector
// For consumers to use
#include <span> // std::span
#endif

#if defined(SOURCEMETA_INCLUDEJS_ENGINE_JAVASCRIPT_CORE)
namespace sourcemeta::includejs {
// This is a opaque function signature that can be force-casted into
// JSObjectCallAsFunctionCallback
/// @ingroup engine
using Function = const void *(*)(const void *, const void *, const void *,
                                 const size_t, const void *[], const void **);
} // namespace sourcemeta::includejs
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

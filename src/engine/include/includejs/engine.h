#ifndef INCLUDEJS_ENGINE_H_
#define INCLUDEJS_ENGINE_H_

/// @defgroup engine Engine
/// @brief The IncludeJS higher-level abstraction of a JavaScript engine

#include "engine_export.h"

#include <includejs/engine_context.h>
#include <includejs/engine_error.h>
#include <includejs/engine_function.h>
#include <includejs/engine_value.h>

#include <filesystem>       // std::filesystem::path
#include <initializer_list> // std::initializer_list
#include <ios>              // std::ios_base
#include <istream>          // std::ifstream
#include <memory>           // std::unique_ptr
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <vector>           // std::vector

namespace includejs {

/// @ingroup engine
class INCLUDEJS_ENGINE_EXPORT Engine {
public:
  Engine();
  ~Engine();

  auto evaluate(const std::filesystem::path &path) -> Value;
  auto evaluate(const std::string &code, const std::filesystem::path &path)
      -> Value;
  auto evaluate(std::ifstream &stream, const std::filesystem::path &path)
      -> Value;

  // TODO(RaisinTen): Add support for bind_function() to the V8 backend.
#if !defined(INCLUDEJS_ENGINE_V8)
  auto bind_function(std::initializer_list<std::string> location,
                     Function function) -> void;
#endif
  auto bind_global(std::initializer_list<std::string> location, Value value)
      -> void;
  auto bind_global(std::initializer_list<std::string> location, bool value)
      -> void;
  auto bind_global(std::initializer_list<std::string> location, int value)
      -> void;
  auto bind_global(std::initializer_list<std::string> location,
                   std::initializer_list<Value> values) -> void;

  auto context() -> Context &;

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
  auto on_error(const Value &exception) -> void;
};

} // namespace includejs

#endif

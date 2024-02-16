#ifndef SOURCEMETA_INCLUDEJS_ENGINE_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_H_

/// @defgroup engine Engine
/// @brief The IncludeJS higher-level abstraction of a JavaScript engine

#include "engine_export.h"

#include <sourcemeta/includejs/engine_context.h>
#include <sourcemeta/includejs/engine_error.h>
#include <sourcemeta/includejs/engine_function.h>
#include <sourcemeta/includejs/engine_value.h>

#include <filesystem>       // std::filesystem::path
#include <initializer_list> // std::initializer_list
#include <ios>              // std::ios_base
#include <istream>          // std::ifstream
#include <memory>           // std::unique_ptr
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <vector>           // std::vector

namespace sourcemeta {
namespace includejs {

/// @ingroup engine
class SOURCEMETA_INCLUDEJS_ENGINE_EXPORT Engine {
public:
  Engine();
  ~Engine();

  auto evaluate(const std::filesystem::path &path) -> Value;
  auto evaluate(const std::string &code, const std::filesystem::path &path)
      -> Value;
  auto evaluate(std::ifstream &stream, const std::filesystem::path &path)
      -> Value;

  auto bind_function(std::initializer_list<std::string> location,
                     Function function) -> void;
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
} // namespace sourcemeta

#endif

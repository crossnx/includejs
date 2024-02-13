#ifndef SOURCEMETA_INCLUDEJS_ENGINE_VALUE_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_VALUE_H_

#include "engine_export.h"

#include <map>      // std::map
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <string>   // std::string

namespace sourcemeta::includejs {

// Inspired by https://github.com/sourcemeta/jsontoolkit
/// @ingroup engine
class SOURCEMETA_INCLUDEJS_ENGINE_EXPORT Value {
public:
  // Consumers are not meant to create this class directly
#ifndef DOXYGEN
  Value(const void *context, const void *value);
  ~Value();
#endif

  Value(Value &&other) noexcept;
  Value(const Value &other) = delete;

  auto is_number() const -> bool;
  auto is_string() const -> bool;
  auto is_error() const -> bool;
  auto is_object() const -> bool;
  auto is_boolean() const -> bool;
  auto to_number() const -> double;
  auto to_string() const -> std::string;
  auto to_boolean() const -> bool;
  auto at(const std::string &property) const -> std::optional<Value>;
  auto set(const std::string &property, Value value) -> void;
  auto to_map() const -> std::map<std::string, Value>;

  // For internal use only
#ifndef DOXYGEN
  auto native() const -> const void *;
#endif

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
};

} // namespace sourcemeta::includejs

#endif

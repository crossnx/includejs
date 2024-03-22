#ifndef INCLUDEJS_ENGINE_CONTEXT_H_
#define INCLUDEJS_ENGINE_CONTEXT_H_

#include "engine_export.h"

#include <includejs/engine_promise.h>
#include <includejs/engine_value.h>

#include <memory> // std::unique_ptr
#include <string> // std::string

namespace includejs {

/// @ingroup engine
class INCLUDEJS_ENGINE_EXPORT Context {
public:
  // Consumers are not meant to create this class directly
#ifndef DOXYGEN
  Context(const void *context);
  ~Context();
#endif
  auto make_undefined() const -> Value;
  auto make_error(const std::string &message) const -> Value;
  auto make_object() const -> Value;
  auto make_promise() const -> Promise;
  auto make_array() const -> Value;
  auto from(const std::string &value) const -> Value;
  auto from(const char *) const -> Value;
  auto from(int value) const -> Value;
  auto from(double value) const -> Value;
  auto from(bool value) const -> Value;
  auto from(std::nullptr_t value) const -> Value;
  auto from_json(const std::string &json_string) const -> Value;
  auto global() const -> Value;

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
};

} // namespace includejs

#endif

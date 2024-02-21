#ifndef SOURCEMETA_INCLUDEJS_ENGINE_CONTEXT_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_CONTEXT_H_

#include "engine_export.h"

#include <includejs/engine_promise.h>
#include <includejs/engine_value.h>

#include <memory> // std::unique_ptr
#include <string> // std::string

namespace sourcemeta {
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
  auto make_null() const -> Value;
  auto make_error(const std::string &message) const -> Value;
  auto make_object() const -> Value;
  auto make_promise() const -> Promise;
  auto from(const std::string &value) const -> Value;
  auto from(const char *) const -> Value;
  auto from(int value) const -> Value;
  auto from(double value) const -> Value;
  auto from(bool value) const -> Value;
  auto from(std::nullptr_t value) const -> Value;
  auto global() const -> Value;

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
};

} // namespace includejs
} // namespace sourcemeta

#endif

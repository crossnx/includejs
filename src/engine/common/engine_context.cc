#include <includejs/engine_context.h>

#if !defined(SOURCEMETA_INCLUDEJS_ENGINE_V8)

namespace includejs {

auto Context::from(int value) const -> Value {
  // JavaScript only has a "real" number type.
  return from(static_cast<double>(value));
}

auto Context::from(const std::string &value) const -> Value {
  return from(value.c_str());
}

} // namespace includejs

#endif

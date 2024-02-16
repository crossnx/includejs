#ifndef SOURCEMETA_INCLUDEJS_ENGINE_PROMISE_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_PROMISE_H_

#include "engine_export.h"

#include <sourcemeta/includejs/engine_value.h>

#include <memory> // std::unique_ptr

namespace sourcemeta {
namespace includejs {

/// @ingroup engine
class SOURCEMETA_INCLUDEJS_ENGINE_EXPORT Promise {
public:
  // Consumers are not meant to create this class directly
#ifndef DOXYGEN
  Promise(const void *context);
  ~Promise();
#endif
  auto resolve(Value result) -> void;
  auto reject(Value error) -> void;
  auto value() -> Value;

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
  bool completed{false};
};

} // namespace includejs
} // namespace sourcemeta

#endif

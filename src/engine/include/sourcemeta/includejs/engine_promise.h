#ifndef SOURCEMETA_INCLUDEJS_ENGINE_PROMISE_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_PROMISE_H_

#include "engine_export.h"

#include <sourcemeta/includejs/engine_value.h>

#include <memory> // std::unique_ptr

namespace sourcemeta::includejs {

class SOURCEMETA_INCLUDEJS_ENGINE_EXPORT Promise {
public:
  Promise(const void *context);
  ~Promise();
  auto resolve(Value result) -> void;
  auto reject(Value error) -> void;
  auto value() -> Value;

private:
  struct Internal;
  std::unique_ptr<Internal> internal;
  bool completed{false};
};

} // namespace sourcemeta::includejs

#endif

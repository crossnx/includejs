#ifndef INCLUDEJS_ENGINE_PRIVATE_DATA_H
#define INCLUDEJS_ENGINE_PRIVATE_DATA_H

#include <includejs/engine_value.h>

#include <map>    // std::map
#include <memory> // std::unique_ptr

namespace includejs {

// Inspired by https://github.com/sourcemeta/jsontoolkit
/// @ingroup engine
struct INCLUDEJS_ENGINE_EXPORT PrivateObjectData {
  std::unique_ptr<std::map<void *, Value::Function>> functions =
      std::make_unique<std::map<void *, Value::Function>>();

  ~PrivateObjectData();

  auto data() -> void *;
  auto set_data(void *new_data, std::function<void(void *)> new_deleter)
      -> void;

private:
  void *data_ = nullptr;
  std::function<void(void *)> deleter_;

  auto clear() -> void;
};

} // namespace includejs

#endif // INCLUDEJS_ENGINE_PRIVATE_DATA_H

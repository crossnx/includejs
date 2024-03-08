#include <includejs/engine_private_data.h>

namespace includejs {

PrivateObjectData::~PrivateObjectData() { clear(); }

auto PrivateObjectData::data() -> void * { return data_; }

auto PrivateObjectData::set_data(void *new_data,
                                 std::function<void(void *)> new_deleter)
    -> void {
  clear();
  data_ = new_data;
  deleter_ = new_deleter;
}

auto PrivateObjectData::clear() -> void {
  if (data_ != nullptr && deleter_ != nullptr) {
    deleter_(data_);
    data_ = nullptr;
  }
}

} // namespace includejs

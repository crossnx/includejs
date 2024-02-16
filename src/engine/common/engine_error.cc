#include <sourcemeta/includejs/engine_error.h>

#include <iterator> // std::cbegin, std::cend
#include <utility>  // std::move

namespace sourcemeta {
namespace includejs {

Error::Error(std::string &&new_message, FrameContainer &&new_stacktrace)
    : message{std::move(new_message)}, stacktrace{std::move(new_stacktrace)} {}

auto Error::what() const noexcept -> const char * {
  return this->message.c_str();
}

auto Error::begin() const -> FrameContainer::const_iterator {
  return std::cbegin(this->stacktrace);
}

auto Error::end() const -> FrameContainer::const_iterator {
  return std::cend(this->stacktrace);
}

} // namespace includejs
} // namespace sourcemeta

#ifndef SOURCEMETA_INCLUDEJS_ENGINE_ERROR_H_
#define SOURCEMETA_INCLUDEJS_ENGINE_ERROR_H_

#include "engine_export.h"

#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <optional>   // std::optional
#include <string>     // std::string
#include <vector>     // std::vector

namespace includejs {
/// @ingroup engine
struct INCLUDEJS_ENGINE_EXPORT Frame {
  const std::optional<std::string> scope;
  const std::optional<std::filesystem::path> path;
  const unsigned long long line;
  const unsigned long long column;
};

/// @ingroup engine
class INCLUDEJS_ENGINE_EXPORT Error : public std::exception {
private:
  using FrameContainer = std::vector<Frame>;

public:
  Error(std::string &&message, FrameContainer &&stacktrace);
  auto what() const noexcept -> const char * override;
  auto begin() const -> FrameContainer::const_iterator;
  auto end() const -> FrameContainer::const_iterator;

private:
  const std::string message;
  const FrameContainer stacktrace;
};
} // namespace includejs

#endif

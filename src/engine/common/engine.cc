#include <includejs/engine.h>

#include <cassert>   // assert
#include <fstream>   // std::ifstream
#include <optional>  // std::optional
#include <sstream>   // std::istringstream
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move
#include <vector>    // std::vector

#if !defined(INCLUDEJS_ENGINE_V8)

namespace includejs {

// For convenience
auto Engine::evaluate(std::ifstream &stream, const std::filesystem::path &path)
    -> Value {
  stream.exceptions(std::ios_base::badbit);
  std::string line;
  std::ostringstream result;
  while (std::getline(stream, line))
    result << line << "\n";
  return evaluate(result.str(), path);
}

auto Engine::evaluate(const std::filesystem::path &path) -> Value {
  std::ifstream stream{path};
  return evaluate(stream, path);
}

auto Engine::on_error(const Value &exception) -> void {
  /*
   * (1) Check that the exception is an instance of the "Error" class
   */
  if (!exception.is_error()) {
    throw std::runtime_error(
        "Caught JavaScript exception that is not an instance of Error");
  }

  /*
   * (2) Get the error message and raw stacktrace
   */
  const std::optional<Value> message{exception.at("message")};
  assert(message.has_value());
  assert(message->is_string());
  const std::optional<Value> stack{exception.at("stack")};
  const std::string stack_string{stack.has_value() ? stack->to_string() : ""};

  /*
   * (3) Parse the stack trace
   */
  std::istringstream stream{stack_string};
  std::string entry;
  std::vector<Frame> stacktrace;
  while (std::getline(stream, entry)) {
    const std::size_t scope_separator{entry.find("@")};
    assert(scope_separator != std::string::npos);
    const std::size_t column_separator{entry.find_last_of(":")};
    const std::size_t line_separator{
        entry.find_last_of(":", column_separator - 1)};
    const auto line{
        line_separator == std::string::npos
            ? 0
            : std::stoull(entry.substr(line_separator + 1,
                                       column_separator - line_separator - 1))};
    const auto column{column_separator == std::string::npos
                          ? 0
                          : std::stoull(entry.substr(column_separator + 1))};

    const std::string raw_scope{entry.substr(0, scope_separator)};
    const std::string raw_path{entry.substr(
        scope_separator + 1, line_separator - scope_separator - 1)};

    stacktrace.push_back(
        {raw_scope == "global code" ? std::optional<std::string>{} : raw_scope,
         raw_path == "[native code]" ? std::optional<std::filesystem::path>{}
                                     : raw_path,
         line, column});
  }

  /*
   * (4) Throw as a C++ exception
   */
  throw Error(message->to_string(), std::move(stacktrace));
}

} // namespace includejs

#endif

#include <includejs/engine.h>

#include <cassert>  // EXIT_SUCCESS
#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout

auto main() -> int {
  includejs::Engine engine;
  // TODO(RaisinTen): Remove this once V8 supports this.
#if !defined(INCLUDEJS_ENGINE_V8)
  includejs::Value result{engine.evaluate("1 + 3", "index.js")};
  assert(result.is_number());
  std::cout << result.to_number() << "\n";
#endif
  return EXIT_SUCCESS;
}

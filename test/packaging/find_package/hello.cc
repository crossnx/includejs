#include <sourcemeta/includejs/engine.h>

#include <cassert>  // EXIT_SUCCESS
#include <cstdlib>  // EXIT_SUCCESS
#include <iostream> // std::cout

auto main() -> int {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("1 + 3", "index.js")};
  assert(result.is_number());
  std::cout << result.to_number() << "\n";
  return EXIT_SUCCESS;
}

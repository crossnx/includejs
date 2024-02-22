#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, is_function) {
  sourcemeta::includejs::Engine engine;

  auto result = engine.evaluate("(function a() {})", "index.js");

  EXPECT_TRUE(result.is_function());
}

TEST(IncludeJS_Engine, to_function) {
  sourcemeta::includejs::Engine engine;

  auto result = engine.evaluate("(function a() { return 42; })", "index.js");

  EXPECT_TRUE(result.is_function());

  auto value = result.to_function()({});
  EXPECT_TRUE(value.is_number());
  EXPECT_EQ(value.to_number(), 42);
}

#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, create_null) {
  sourcemeta::includejs::Engine engine;

  auto null = engine.context().make_null();
  EXPECT_TRUE(null.is_null());
}

TEST(IncludeJS_Engine, evaluate_null) {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("null", "index.js")};
  EXPECT_TRUE(result.is_null());
}

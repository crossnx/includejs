#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, create_undefined) {
  sourcemeta::includejs::Engine engine;

  auto object = engine.context().make_undefined();
  EXPECT_TRUE(object.is_undefined());
}

TEST(IncludeJS_Engine, evaluate_undefined) {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("undefined", "index.js")};
  EXPECT_TRUE(result.is_undefined());
}

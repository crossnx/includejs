#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, create_null) {
  includejs::Engine engine;

  auto null = engine.context().from(nullptr);
  EXPECT_TRUE(null.is_null());
}

TEST(IncludeJS_Engine, evaluate_null) {
  includejs::Engine engine;
  includejs::Value result{engine.evaluate("null", "index.js")};
  EXPECT_TRUE(result.is_null());
}

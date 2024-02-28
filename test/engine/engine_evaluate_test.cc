#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, evaluate_result_integer) {
  includejs::Engine engine;
  includejs::Value result{engine.evaluate("1 + 3", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 4);
}

TEST(IncludeJS_Engine, evaluate_result_string) {
  includejs::Engine engine;
  includejs::Value result{engine.evaluate("\"foo\"", "index.js")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "foo");
}

TEST(IncludeJS_Engine, invalid_function_call) {
  includejs::Engine engine;
  EXPECT_THROW(engine.evaluate("xyz()", "index.js"), includejs::Error);
}

TEST(IncludeJS_Engine, evaluate_syntax_error) {
  includejs::Engine engine;
  EXPECT_THROW(engine.evaluate("1 +", "index.js"), includejs::Error);
}

#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, evaluate_result_integer) {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("1 + 3", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 4);
}

TEST(IncludeJS_Engine, evaluate_result_string) {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("\"foo\"", "index.js")};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "foo");
}

TEST(IncludeJS_Engine, invalid_function_call) {
  sourcemeta::includejs::Engine engine;
  EXPECT_THROW(engine.evaluate("xyz()", "index.js"),
               sourcemeta::includejs::Error);
}

TEST(IncludeJS_Engine, evaluate_syntax_error) {
  sourcemeta::includejs::Engine engine;
  EXPECT_THROW(engine.evaluate("1 +", "index.js"),
               sourcemeta::includejs::Error);
}

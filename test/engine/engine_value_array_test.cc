#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, create_array) {
  sourcemeta::includejs::Engine engine;

  auto arr = engine.context().make_array();

  EXPECT_TRUE(arr.is_array());
}

TEST(IncludeJS_Engine, transform_array_to_vector) {
  sourcemeta::includejs::Engine engine;

  auto arr = engine.context().make_array();
  arr.push(engine.context().from(42));

  auto vec = arr.to_vector();
  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(vec.at(0).to_number(), 42);
}

TEST(IncludeJS_Engine, evaluate_array) {
  sourcemeta::includejs::Engine engine;
  sourcemeta::includejs::Value result{engine.evaluate("([])", "index.js")};
  EXPECT_TRUE(result.is_array());
}

TEST(IncludeJS_Engine, push_and_get_array_element) {
  sourcemeta::includejs::Engine engine;

  auto arr = engine.context().make_array();
  arr.push(engine.context().from(42));
  arr.push(engine.context().from("baz"));

  sourcemeta::includejs::Value result = arr.at(0).value();
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 42);

  sourcemeta::includejs::Value result2 = arr.at(1).value();
  EXPECT_TRUE(result2.is_string());
  EXPECT_EQ(result2.to_string(), "baz");
}

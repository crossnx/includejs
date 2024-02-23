#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, null_type) {
  includejs::Engine engine;

  auto null = engine.context().from(nullptr);
  EXPECT_TRUE(null.type() == includejs::JSValueType::Null);
}

TEST(IncludeJS_Engine, undefined_type) {
  includejs::Engine engine;

  auto undefined = engine.context().make_undefined();
  EXPECT_TRUE(undefined.type() == includejs::JSValueType::Undefined);
}

TEST(IncludeJS_Engine, error_type) {
  includejs::Engine engine;

  auto error = engine.context().make_error("error");
  EXPECT_TRUE(error.type() == includejs::JSValueType::Error);
}

TEST(IncludeJS_Engine, object_type) {
  includejs::Engine engine;

  auto object = engine.context().make_object();
  EXPECT_TRUE(object.type() == includejs::JSValueType::Object);
}

TEST(IncludeJS_Engine, array_type) {
  includejs::Engine engine;

  auto array = engine.context().make_array();
  EXPECT_TRUE(array.type() == includejs::JSValueType::Array);
}

TEST(IncludeJS_Engine, number_type) {
  includejs::Engine engine;

  auto number = engine.context().from(42);
  EXPECT_TRUE(number.type() == includejs::JSValueType::Number);
}

TEST(IncludeJS_Engine, string_type) {
  includejs::Engine engine;

  auto string = engine.context().from("foo");
  EXPECT_TRUE(string.type() == includejs::JSValueType::String);
}

TEST(IncludeJS_Engine, boolean_type) {
  includejs::Engine engine;

  auto boolean = engine.context().from(true);
  EXPECT_TRUE(boolean.type() == includejs::JSValueType::Boolean);
}

TEST(IncludeJS_Engine, function_type) {
  includejs::Engine engine;

  auto result = engine.evaluate("(function foo() {})", "index.js");
  EXPECT_TRUE(result.type() == includejs::JSValueType::Function);
}

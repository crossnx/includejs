#include <gtest/gtest.h>

#include <sourcemeta/includejs/engine.h>

#include <span>

static auto is_string(const sourcemeta::includejs::Context &context,
                      std::span<sourcemeta::includejs::Value> arguments)
    -> sourcemeta::includejs::Value {
  return context.from(!arguments.empty() && arguments.front().is_string());
}

static auto get_details(const sourcemeta::includejs::Context &context,
                        std::span<sourcemeta::includejs::Value>)
    -> sourcemeta::includejs::Value {
  sourcemeta::includejs::Value result{context.make_object()};
  result.set("version", context.from("1.0.0"));
  return result;
}

static auto promise_test(const sourcemeta::includejs::Context &context,
                         std::span<sourcemeta::includejs::Value>)
    -> sourcemeta::includejs::Value {
  sourcemeta::includejs::Promise promise{context.make_promise()};
  promise.resolve(context.from(true));
  return promise.value();
}

SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION(is_string);
SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION(get_details);
SOURCEMETA_INCLUDEJS_EXPOSE_FUNCTION(promise_test);

TEST(IncludeJS_Engine, bind_global_root_integer) {
  sourcemeta::includejs::Engine engine;
  engine.bind_global({"my_global"}, 2);
  sourcemeta::includejs::Value result{engine.evaluate("my_global", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 2);
}

TEST(IncludeJS_Engine, bind_global_nested_integer) {
  sourcemeta::includejs::Engine engine;
  engine.bind_global({"foo", "bar", "baz"}, 8);
  sourcemeta::includejs::Value result{
      engine.evaluate("foo.bar.baz", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 8);
}

TEST(IncludeJS_Engine, bind_global_nested_boolean_true) {
  sourcemeta::includejs::Engine engine;
  engine.bind_global({"foo", "bar", "baz"}, true);
  sourcemeta::includejs::Value result{
      engine.evaluate("foo.bar.baz", "index.js")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_EQ(result.to_boolean(), true);
}

TEST(IncludeJS_Engine, bind_global_nested_boolean_false) {
  sourcemeta::includejs::Engine engine;
  engine.bind_global({"foo", "bar", "baz"}, false);
  sourcemeta::includejs::Value result{
      engine.evaluate("foo.bar.baz", "index.js")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_EQ(result.to_boolean(), false);
}

TEST(IncludeJS_Engine, bind_global_array) {
  sourcemeta::includejs::Engine engine;
  engine.bind_global({"my", "array"}, {engine.context().from("foo"),
                                       engine.context().from("bar"),
                                       engine.context().from("baz")});

  // Length
  sourcemeta::includejs::Value length{
      engine.evaluate("my.array.length", "index.js")};
  EXPECT_TRUE(length.is_number());
  EXPECT_EQ(length.to_number(), 3);

  // Value at index 0
  sourcemeta::includejs::Value result1{
      engine.evaluate("my.array[0]", "index.js")};
  EXPECT_TRUE(result1.is_string());
  EXPECT_EQ(result1.to_string(), "foo");

  // Value at index 1
  sourcemeta::includejs::Value result2{
      engine.evaluate("my.array[1]", "index.js")};
  EXPECT_TRUE(result2.is_string());
  EXPECT_EQ(result2.to_string(), "bar");

  // Value at index 2
  sourcemeta::includejs::Value result3{
      engine.evaluate("my.array[2]", "index.js")};
  EXPECT_TRUE(result3.is_string());
  EXPECT_EQ(result3.to_string(), "baz");
}

TEST(IncludeJS_Engine, bind_function_return_boolean_false) {
  sourcemeta::includejs::Engine engine;
  engine.bind_function({"isString"}, &is_string);
  sourcemeta::includejs::Value result{
      engine.evaluate("isString(1)", "index.js")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_EQ(result.to_boolean(), false);
}

TEST(IncludeJS_Engine, bind_function_return_boolean_true) {
  sourcemeta::includejs::Engine engine;
  engine.bind_function({"isString"}, &is_string);
  sourcemeta::includejs::Value result{
      engine.evaluate("isString('foo')", "index.js")};
  EXPECT_TRUE(result.is_boolean());
  EXPECT_EQ(result.to_boolean(), true);
}

TEST(IncludeJS_Engine, bind_function_details_object) {
  sourcemeta::includejs::Engine engine;
  engine.bind_function({"getDetails"}, &get_details);
  sourcemeta::includejs::Value result{
      engine.evaluate("getDetails()", "index.js")};
  EXPECT_TRUE(result.is_object());
  const std::optional<sourcemeta::includejs::Value> version{
      result.at("version")};
  EXPECT_TRUE(version.has_value());
  EXPECT_TRUE(version->is_string());
  EXPECT_EQ(version->to_string(), "1.0.0");
}

TEST(IncludeJS_Engine, bind_function_promise) {
  sourcemeta::includejs::Engine engine;
  engine.bind_function({"myPromise"}, &promise_test);
  sourcemeta::includejs::Value result{
      engine.evaluate("myPromise()", "index.js")};
  EXPECT_TRUE(result.is_object());
}

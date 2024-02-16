#include <gtest/gtest.h>

#include <sourcemeta/includejs/engine.h>

// TODO(tonygo): Add tests for each kind of type
TEST(IncludeJS_Engine, set_object_property) {
  sourcemeta::includejs::Engine engine;

  auto object = engine.context().make_object();
  object.set("foo", engine.context().from(42));
  engine.bind_global({"obj"}, std::move(object));

  sourcemeta::includejs::Value result{engine.evaluate("obj.foo", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 42);
}

TEST(IncludeJS_Engine, set_object_function) {
  sourcemeta::includejs::Engine engine;

  auto object = engine.context().make_object();
  object.set(
      "foo",
      [](std::span<sourcemeta::includejs::Value> arguments)
          -> sourcemeta::includejs::Value { return std::move(arguments[0]); });
  engine.bind_global({"obj"}, std::move(object));

  sourcemeta::includejs::Value result{
      engine.evaluate("obj.foo(42)", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 42);

  sourcemeta::includejs::Value result2{
      engine.evaluate("obj.foo('bar')", "index.js")};
  EXPECT_TRUE(result2.is_string());
  EXPECT_EQ(result2.to_string(), "bar");
}

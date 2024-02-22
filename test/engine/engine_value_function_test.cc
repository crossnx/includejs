#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, is_function) {
  includejs::Engine engine;

  auto obj = engine.context().make_object();
  obj.set("foo",
          [](std::vector<includejs::Value> arguments) -> includejs::Value {
            return std::move(arguments[0]);
          });
  obj.set("bar", engine.context().from(42));

  EXPECT_TRUE(obj.at("foo").has_value());
  EXPECT_TRUE(obj.at("foo").value().is_function());

  EXPECT_TRUE(obj.at("bar").has_value());
  EXPECT_FALSE(obj.at("bar").value().is_function());
}

#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, to_json_string_object) {
  includejs::Engine engine;
  auto obj = engine.context().make_object();
  obj.set("foo", engine.context().from("bar"));

  auto json_object = obj.to_json_string();
  EXPECT_EQ(json_object, "{\"foo\":\"bar\"}");
}

TEST(IncludeJS_Engine, to_json_skip_function_in_object) {
  includejs::Engine engine;
  auto obj = engine.context().make_object();
  obj.set("bar", [&engine](std::vector<includejs::Value>) {
    return engine.context().from("baz");
  });

  auto json = obj.to_json_string();
  EXPECT_EQ(json, "{}");
}

TEST(IncludeJS_Engine, to_json_string_array) {
  includejs::Engine engine;
  auto arr = engine.context().make_array();
  arr.push(engine.context().from("foo"));
  arr.push(engine.context().from("bar"));

  auto json_array = arr.to_json_string();
  EXPECT_EQ(json_array, "[\"foo\",\"bar\"]");
}

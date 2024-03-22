#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <includejs/engine.h>

TEST(IncludeJS_Engine, from_json_object) {
  includejs::Engine engine;

  auto json = engine.context().from_json("{ \"key\": null }");

  EXPECT_TRUE(json.is_object());
  auto json_object = json.to_map();
  EXPECT_EQ(json_object.size(), 1);
  EXPECT_TRUE(json_object.at("key").is_null());
}

TEST(IncludeJS_Engine, from_json_array) {
  includejs::Engine engine;

  auto json = engine.context().from_json("[2]");

  EXPECT_TRUE(json.is_array());
  auto json_array = json.to_vector();
  EXPECT_EQ(json_array.size(), 1);
  EXPECT_TRUE(json_array.at(0).is_number());
}

TEST(IncludeJS_Engine, from_json_failure) {
  includejs::Engine engine;

  EXPECT_THAT([&] { engine.context().from_json("<div>"); },
              testing::ThrowsMessage<std::invalid_argument>(
                  testing::HasSubstr("Invalid JSON string")));
}

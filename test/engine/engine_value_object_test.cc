#include <gtest/gtest.h>

#include <includejs/engine.h>

#include <future> // std::promise, std::future

TEST(IncludeJS_Engine, create_object) {
  includejs::Engine engine;

  auto object = engine.context().make_object();

  EXPECT_TRUE(object.is_object());
}

TEST(IncludeJS_Engine, transform_object_to_map) {
  includejs::Engine engine;

  auto object = engine.context().make_object();
  object.set("foo", engine.context().from(42));

  auto map = object.to_map();
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.at("foo").to_number(), 42);
}

TEST(IncludeJS_Engine, evaluate_object) {
  includejs::Engine engine;
  includejs::Value result{engine.evaluate("({})", "index.js")};
  EXPECT_TRUE(result.is_object());
}

TEST(IncludeJS_Engine, set_and_get_object_property) {
  includejs::Engine engine;

  auto object = engine.context().make_object();
  object.set("foo", engine.context().from(42));
  object.set("bar", engine.context().from("baz"));

  includejs::Value result = object.at("foo").value();
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 42);

  includejs::Value result2 = object.at("bar").value();
  EXPECT_TRUE(result2.is_string());
  EXPECT_EQ(result2.to_string(), "baz");
}

TEST(IncludeJS_Engine, set_object_function) {
  includejs::Engine engine;

  auto object = engine.context().make_object();
  object.set("foo",
             [](std::vector<includejs::Value> arguments) -> includejs::Value {
               return std::move(arguments[0]);
             });
  engine.bind_global({"obj"}, std::move(object));

  includejs::Value result{engine.evaluate("obj.foo(42)", "index.js")};
  EXPECT_TRUE(result.is_number());
  EXPECT_EQ(result.to_number(), 42);

  includejs::Value result2{engine.evaluate("obj.foo('bar')", "index.js")};
  EXPECT_TRUE(result2.is_string());
  EXPECT_EQ(result2.to_string(), "bar");
}

TEST(IncludeJS_Engine, at_throw_for_functions) {
  includejs::Engine engine;
  includejs::Context &context = engine.context();

  auto obj = context.make_object();
  obj.set("woo", [](std::vector<includejs::Value> args) -> includejs::Value {
    return std::move(args[0]);
  });
  EXPECT_THROW(obj.at("woo"), std::runtime_error);
}

TEST(IncludeJS_Engine, set_and_get_private_data) {
  auto str = new std::string("one");

  includejs::Engine ignition;
  includejs::Value object{ignition.context().make_object()};
  object.private_data(
      str, [](void *data) { delete static_cast<std::string *>(data); });

  EXPECT_EQ(object.private_data(), str);
}

TEST(IncludeJS_Engine, double_set_private_data) {
  auto str1 = new std::string("one");
  auto str2 = new std::string("two");
  std::promise<void> promise;
  std::future<void> future = promise.get_future();

  includejs::Engine ignition;
  includejs::Value object{ignition.context().make_object()};
  object.private_data(str1, [&str1, &promise](void *data) {
    EXPECT_EQ(static_cast<std::string *>(data), str1);
    delete static_cast<std::string *>(data);
    promise.set_value();
  });
  // We add this one, in case the object is cleared properly
  object.private_data(str2, [&str2](void *data) {
    EXPECT_EQ(static_cast<std::string *>(data), str2);
    delete static_cast<std::string *>(data);
  });

  future.wait();
  EXPECT_EQ(object.private_data(), str2);
}

#include <gtest/gtest.h>

#include <includejs/engine.h>

#include <filesystem>
#include <string>

TEST(IncludeJS_Engine, unknown_global_function) {
  includejs::Engine engine;
  try {
    engine.evaluate("this_does_not_exist()", "index.js");
    FAIL();
  } catch (const includejs::Error &error) {
    EXPECT_EQ(std::string{error.what()},
              "Can't find variable: this_does_not_exist");
    const std::vector<includejs::Frame> stacktrace{error.begin(), error.end()};
    EXPECT_EQ(stacktrace.size(), 1);
    EXPECT_FALSE(stacktrace.at(0).scope.has_value());
    EXPECT_TRUE(stacktrace.at(0).path.has_value());
    EXPECT_EQ(stacktrace.at(0).path.value(),
              std::filesystem::weakly_canonical("index.js"));
    EXPECT_EQ(stacktrace.at(0).line, 1);
    EXPECT_EQ(stacktrace.at(0).column, 20);
  }
}

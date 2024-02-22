#include <includejs/engine.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <algorithm> // std::transform
#include <cassert>   // assert
#include <exception> // std::exception
#include <iterator>  // std::begin, std::end, std::prev, std::back_inserter
#include <optional>  // std::optional
#include <utility>   // std::move
#include <vector>    // std::vector

namespace includejs {

struct Engine::Internal {
  JSContextGroupRef group;
  JSGlobalContextRef context;
  JSObjectRef global;
};

Engine::Engine() : internal{std::make_unique<Engine::Internal>()} {
  this->internal->group = JSContextGroupCreate();

  // Create context
  this->internal->context =
      JSGlobalContextCreateInGroup(this->internal->group, nullptr);
  JSStringRef name = JSStringCreateWithUTF8CString("includejs");
  JSGlobalContextSetName(this->internal->context, name);
  JSStringRelease(name);

  // Get a pointer to the global object (for binding purposes)
  this->internal->global = JSContextGetGlobalObject(this->internal->context);
}

Engine::~Engine() {
  this->internal->global = nullptr;
  JSGlobalContextRelease(this->internal->context);
  JSContextGroupRelease(this->internal->group);
}

auto Engine::context() -> Context & {
  static Context engine_context{static_cast<void *>(this->internal->context)};
  return engine_context;
}

auto Engine::evaluate(const std::string &code,
                      const std::filesystem::path &path) -> Value {
  // We do this first as it may throw
  JSStringRef url = JSStringCreateWithUTF8CString(
      std::filesystem::weakly_canonical(path).string().c_str());

  JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
  JSValueRef exception = nullptr;
  JSValueRef result = JSEvaluateScript(this->internal->context, script, nullptr,
                                       url, 1, &exception);
  JSStringRelease(script);
  JSStringRelease(url);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }

  return {this->internal->context, result};
}

static auto set_property(JSContextRef context, JSObjectRef object,
                         JSStringRef name, Function function,
                         JSValueRef *exception) -> void {
  JSObjectRef value = JSObjectMakeFunctionWithCallback(
      context, name,
      reinterpret_cast<JSObjectCallAsFunctionCallback>(function));
  JSObjectSetProperty(context, object, name, value,
                      kJSPropertyAttributeReadOnly, exception);
}

static auto set_property(JSContextRef context, JSObjectRef object,
                         JSStringRef name, JSValueRef value,
                         JSValueRef *exception) -> void {
  JSObjectSetProperty(context, object, name, value,
                      kJSPropertyAttributeReadOnly, exception);
}

template <typename T>
static auto bind_value(JSContextRef context, JSObjectRef global,
                       std::initializer_list<std::string> location, T value,
                       JSValueRef *exception) -> void {
  const auto begin{std::begin(location)};
  const auto end{std::end(location)};
  // The initializer list can't be empty
  assert(begin != end);
  const auto last{std::prev(end)};

  JSObjectRef current = global;
  for (auto iterator{begin}; iterator != end; ++iterator) {
    JSStringRef fragment = JSStringCreateWithUTF8CString(iterator->c_str());
    if (iterator == last) {
      set_property(context, current, fragment, value, exception);
    } else if (JSObjectHasProperty(context, current, fragment)) {
      JSValueRef new_current =
          JSObjectGetProperty(context, current, fragment, exception);
      if (*exception) {
        JSStringRelease(fragment);
        return;
      }

      assert(JSValueIsObject(context, new_current));
      current = JSValueToObject(context, new_current, exception);
    } else {
      JSObjectRef new_current = JSObjectMake(context, nullptr, nullptr);
      JSObjectSetProperty(context, current, fragment, new_current,
                          kJSPropertyAttributeReadOnly, exception);
      current = new_current;
    }

    JSStringRelease(fragment);
    if (*exception) {
      return;
    }
  }
}

auto Engine::bind_function(std::initializer_list<std::string> location,
                           Function function) -> void {
  JSValueRef exception = nullptr;
  bind_value(this->internal->context, this->internal->global, location,
             function, &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }
}

auto Engine::bind_global(std::initializer_list<std::string> location,
                         bool value) -> void {
  JSValueRef exception = nullptr;
  JSValueRef js_value = JSValueMakeBoolean(this->internal->context, value);
  bind_value(this->internal->context, this->internal->global, location,
             js_value, &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }
}

auto Engine::bind_global(std::initializer_list<std::string> location, int value)
    -> void {
  JSValueRef exception = nullptr;
  JSValueRef js_value =
      JSValueMakeNumber(this->internal->context, static_cast<double>(value));
  bind_value(this->internal->context, this->internal->global, location,
             js_value, &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }
}

auto Engine::bind_global(std::initializer_list<std::string> location,
                         Value value) -> void {
  JSValueRef exception = nullptr;
  bind_value(this->internal->context, this->internal->global, location,
             static_cast<JSValueRef>(value.native()), &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }
}

auto Engine::bind_global(std::initializer_list<std::string> location,
                         std::initializer_list<Value> values) -> void {
  // Transform the array of wrapped values into an array of raw values
  std::vector<JSValueRef> raw_values;
  raw_values.reserve(values.size());
  std::transform(
      std::begin(values), std::end(values), std::back_inserter(raw_values),
      [](auto &value) { return static_cast<JSValueRef>(value.native()); });

  // Create JavaScriptCore array object
  JSValueRef exception = nullptr;
  JSObjectRef value =
      JSObjectMakeArray(this->internal->context, raw_values.size(),
                        raw_values.data(), &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }

  bind_value(this->internal->context, this->internal->global, location, value,
             &exception);
  if (exception) {
    on_error(Value{static_cast<JSContextRef>(this->internal->context),
                   static_cast<JSValueRef>(exception)});
  }
}

} // namespace includejs

#include <sourcemeta/includejs/engine_value.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <cassert>   // assert
#include <exception> // std::exception
#include <map>       // std::map
#include <stdexcept> // std::runtime_error
#include <vector>    // std::vector

namespace sourcemeta::includejs {

struct Value::Internal {
  JSContextRef context;
  JSValueRef value;
};

Value::Value(const void *context, const void *value)
    : internal{std::make_unique<Value::Internal>()} {
  this->internal->context = static_cast<JSContextRef>(context);
  this->internal->value = static_cast<JSValueRef>(value);
  assert(this->internal->context != nullptr);
  assert(this->internal->value != nullptr);
}

Value::~Value() {}

Value::Value(Value &&other) noexcept : internal{std::move(other.internal)} {}

auto Value::is_number() const -> bool {
  return JSValueIsNumber(this->internal->context, this->internal->value);
}

auto Value::is_string() const -> bool {
  return JSValueIsString(this->internal->context, this->internal->value);
}

auto Value::is_error() const -> bool {
  JSObjectRef global = JSContextGetGlobalObject(this->internal->context);
  JSStringRef class_name = JSStringCreateWithUTF8CString("Error");
  JSValueRef exception = nullptr;
  JSValueRef class_error = JSObjectGetProperty(this->internal->context, global,
                                               class_name, &exception);
  assert(!exception);
  JSStringRelease(class_name);
  assert(JSValueIsObject(this->internal->context, class_error));
  JSObjectRef class_error_constructor =
      JSValueToObject(this->internal->context, class_error, &exception);
  assert(!exception);
  assert(class_error == class_error_constructor);
  const bool result = JSValueIsInstanceOfConstructor(
      this->internal->context, this->internal->value, class_error_constructor,
      &exception);
  assert(!exception);
  return result;
}

auto Value::is_object() const -> bool {
  return JSValueIsObject(this->internal->context, this->internal->value);
}

auto Value::is_boolean() const -> bool {
  return JSValueIsBoolean(this->internal->context, this->internal->value);
}

auto Value::to_number() const -> double {
  assert(is_number());
  JSValueRef exception = nullptr;
  const auto result = JSValueToNumber(this->internal->context,
                                      this->internal->value, &exception);
  assert(!exception);
  return result;
}

static auto js_string_to_std_string(JSStringRef value) -> std::string {
  // JavaScriptCore doesn't have a function to fetch the UTF-8 byte size
  // of a given string. It can only give us the amount of Unicode code-points
  // in the string, which may be more than the bytes required to store it.
  // As a consequence, we can't do much than allocating the maximum possible
  // buffer to hold the string.
  const size_t max_size = JSStringGetMaximumUTF8CStringSize(value);
  std::vector<char> buffer(max_size);
  // Converts a JavaScript string into a null-terminated UTF-8 string,
  // and copies the result into an external byte buffer.
  JSStringGetUTF8CString(value, buffer.data(), max_size);
  return {buffer.data()};
}

// Converting a value into a string first requires copying
// the value reference into a string reference.
static auto js_value_to_std_string(JSContextRef context, JSValueRef value)
    -> std::string {
  JSValueRef exception = nullptr;
  JSStringRef copy = JSValueToStringCopy(context, value, &exception);
  assert(!exception);
  JSStringRetain(copy);
  try {
    std::string result{js_string_to_std_string(copy)};
    JSStringRelease(copy);
    return result;
  } catch (const std::exception &) {
    JSStringRelease(copy);
    throw;
  }
}

auto Value::to_string() const -> std::string {
  assert(is_string());
  return js_value_to_std_string(this->internal->context, this->internal->value);
}

auto Value::to_boolean() const -> bool {
  assert(is_boolean());
  return JSValueToBoolean(this->internal->context, this->internal->value);
}

auto Value::at(const std::string &property) const -> std::optional<Value> {
  assert(is_object());
  JSValueRef exception = nullptr;
  JSObjectRef object = JSValueToObject(this->internal->context,
                                       this->internal->value, &exception);
  assert(!exception);
  assert(object == this->internal->value);

  JSStringRef property_string = JSStringCreateWithUTF8CString(property.c_str());
  if (!JSObjectHasProperty(this->internal->context, object, property_string)) {
    return std::nullopt;
  }

  JSValueRef result = JSObjectGetProperty(this->internal->context, object,
                                          property_string, &exception);
  assert(!exception);
  JSStringRelease(property_string);
  return std::make_optional<Value>(this->internal->context, result);
}

auto Value::set(const std::string &property, Value value) -> void {
  assert(is_object());
  JSValueRef exception = nullptr;
  JSObjectRef object = JSValueToObject(this->internal->context,
                                       this->internal->value, &exception);
  assert(!exception);
  assert(object == this->internal->value);
  JSStringRef name = JSStringCreateWithUTF8CString(property.c_str());
  JSObjectSetProperty(this->internal->context, object, name,
                      static_cast<JSValueRef>(value.native()),
                      kJSPropertyAttributeReadOnly, &exception);
  JSStringRelease(name);
  assert(!exception);
}

auto Value::to_map() const -> std::map<std::string, Value> {
  assert(is_object());
  JSValueRef exception = nullptr;
  JSObjectRef object = JSValueToObject(this->internal->context,
                                       this->internal->value, &exception);
  assert(!exception);
  assert(object == this->internal->value);

  std::map<std::string, Value> map;
  JSPropertyNameArrayRef properties =
      JSObjectCopyPropertyNames(this->internal->context, object);
  const std::size_t count = JSPropertyNameArrayGetCount(properties);
  for (std::size_t index = 0; index < count; index++) {
    JSStringRef property = JSPropertyNameArrayGetNameAtIndex(properties, index);
    JSValueRef value = JSObjectGetProperty(this->internal->context, object,
                                           property, &exception);
    assert(!exception);
    try {
      map.emplace(js_string_to_std_string(property),
                  Value{static_cast<JSContextRef>(this->internal->context),
                        static_cast<JSValueRef>(value)});
    } catch (const std::exception &) {
      JSPropertyNameArrayRelease(properties);
      throw;
    }
  }

  JSPropertyNameArrayRelease(properties);
  return map;
}

auto Value::native() const -> const void * {
  return static_cast<const void *>(this->internal->value);
}

} // namespace sourcemeta::includejs

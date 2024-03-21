#include <includejs/engine_context.h>
#include <includejs/engine_private_data.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <cassert> // assert

namespace includejs {

struct Context::Internal {
  JSContextRef context;
  JSObjectRef global;
};

Context::Context(const void *context)
    : internal{std::make_unique<Context::Internal>()} {
  this->internal->context = static_cast<JSContextRef>(context);
  this->internal->global = JSContextGetGlobalObject(this->internal->context);
  assert(this->internal->context != nullptr);
  assert(this->internal->global != nullptr);
}

Context::~Context() {}

auto Context::make_undefined() const -> Value {
  return {this->internal->context,
          JSValueMakeUndefined(this->internal->context)};
}

auto Context::make_error(const std::string &message) const -> Value {
  JSStringRef message_value = JSStringCreateWithUTF8CString(message.c_str());
  JSValueRef error_arguments[]{
      JSValueMakeString(this->internal->context, message_value)};
  JSStringRelease(message_value);
  JSValueRef exception = nullptr;
  JSObjectRef error = JSObjectMakeError(this->internal->context, 1,
                                        error_arguments, &exception);
  assert(!exception);
  return {this->internal->context, error};
}

auto Context::make_object() const -> Value {
  // We habe to create a class definition to be able to
  // attach private data to the object.
  JSClassDefinition class_def = kJSClassDefinitionEmpty;
  class_def.finalize = [](JSObjectRef object) {
    auto *private_data =
        static_cast<PrivateObjectData *>(JSObjectGetPrivate(object));
    if (private_data != nullptr) {
      delete private_data;
    }
  };
  JSClassRef empty_class = JSClassCreate(&class_def);

  auto res = JSObjectMake(this->internal->context, empty_class, nullptr);
  JSClassRelease(empty_class);

  return {this->internal->context, res};
}

auto Context::make_array() const -> Value {
  return {this->internal->context,
          JSObjectMakeArray(this->internal->context, 0, nullptr, nullptr)};
}

auto Context::make_promise() const -> Promise {
  return {static_cast<const void *>(this->internal->context)};
}

auto Context::from(double value) const -> Value {
  return {this->internal->context,
          JSValueMakeNumber(this->internal->context, value)};
}

auto Context::from(bool value) const -> Value {
  return {this->internal->context,
          JSValueMakeBoolean(this->internal->context, value)};
}

auto Context::from(const char *value) const -> Value {
  JSStringRef string = JSStringCreateWithUTF8CString(value);
  JSValueRef result = JSValueMakeString(this->internal->context, string);
  JSStringRelease(string);
  return {this->internal->context, result};
}

auto Context::from(std::nullptr_t) const -> Value {
  return {this->internal->context, JSValueMakeNull(this->internal->context)};
}

auto Context::from_json(const std::string &json_string) const -> Value {
  JSStringRef json_c_str = JSStringCreateWithUTF8CString(json_string.c_str());
  JSValueRef result =
      JSValueMakeFromJSONString(this->internal->context, json_c_str);
  JSStringRelease(json_c_str);
  if (result == nullptr) {
    throw std::invalid_argument("Invalid JSON string");
  }
  return {this->internal->context, result};
}

auto Context::global() const -> Value {
  return {this->internal->context, this->internal->global};
}

} // namespace includejs

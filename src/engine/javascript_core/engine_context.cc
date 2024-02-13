#include <sourcemeta/includejs/engine_context.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <cassert> // assert

namespace sourcemeta::includejs {

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
  return {this->internal->context,
          JSObjectMake(this->internal->context, nullptr, nullptr)};
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

auto Context::global() const -> Value {
  return {this->internal->context, this->internal->global};
}

} // namespace sourcemeta::includejs

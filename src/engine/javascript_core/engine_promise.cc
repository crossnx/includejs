#include <includejs/engine_promise.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <cassert> // assert

namespace sourcemeta {
namespace includejs {

struct Promise::Internal {
  JSContextRef context;
  JSObjectRef promise;
  JSObjectRef resolve;
  JSObjectRef reject;
};

Promise::Promise(const void *context)
    : internal{std::make_unique<Promise::Internal>()} {
  this->internal->context = static_cast<JSContextRef>(context);
  JSValueRef exception = nullptr;
  this->internal->promise = JSObjectMakeDeferredPromise(
      this->internal->context, &this->internal->resolve,
      &this->internal->reject, &exception);
  assert(!exception);
}

Promise::~Promise() {}

auto Promise::resolve(Value result) -> void {
  assert(!completed);
  JSValueRef exception = nullptr;
  JSValueRef arguments[]{static_cast<JSValueRef>(result.native())};
  JSObjectCallAsFunction(this->internal->context, this->internal->resolve,
                         this->internal->promise, 1, arguments, &exception);
  completed = true;
  assert(!exception);
}

auto Promise::reject(Value error) -> void {
  assert(!completed);
  assert(error.is_error());
  JSValueRef exception = nullptr;
  JSValueRef arguments[]{static_cast<JSValueRef>(error.native())};
  JSObjectCallAsFunction(this->internal->context, this->internal->reject,
                         this->internal->promise, 1, arguments, &exception);
  completed = true;
  assert(!exception);
}

auto Promise::value() -> Value {
  return {this->internal->context, this->internal->promise};
}

} // namespace includejs
} // namespace sourcemeta

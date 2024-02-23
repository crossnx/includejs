#include <includejs/engine_value.h>

extern "C" {
#include <JavaScriptCore/JavaScript.h>
}

#include <cassert>   // assert
#include <exception> // std::exception
#include <map>       // std::map
#include <stdexcept> // std::runtime_error
#include <vector>    // std::vector

namespace includejs {

struct Value::Internal {
  JSContextRef context;
  JSValueRef value;
};

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

  try {
    std::string result{js_string_to_std_string(copy)};
    JSStringRelease(copy);
    return result;
  } catch (const std::exception &) {
    JSStringRelease(copy);
    throw;
  }
}

static auto get_current_object(JSContextRef context, JSValueRef value)
    -> JSObjectRef {
  JSValueRef exception = nullptr;
  JSObjectRef object = JSValueToObject(context, value, &exception);
  assert(!exception);
  assert(object == value);
  return object;
}

static auto get_object_length(JSContextRef context, JSObjectRef object)
    -> std::size_t {
  JSValueRef exception = nullptr;
  JSStringRef length_string = JSStringCreateWithUTF8CString("length");
  const double length = JSValueToNumber(
      context, JSObjectGetProperty(context, object, length_string, &exception),
      &exception);
  assert(!exception);
  JSStringRelease(length_string);
  return static_cast<std::size_t>(length);
}

Value::Value(const void *context, const void *value)
    : internal{std::make_unique<Value::Internal>()} {
  this->internal->context = static_cast<JSContextRef>(context);
  this->internal->value = static_cast<JSValueRef>(value);
  assert(this->internal->context != nullptr);
  assert(this->internal->value != nullptr);
}

Value::~Value() {}

Value::Value(Value &&other) noexcept : internal{std::move(other.internal)} {}

auto Value::type() const -> ValueType {
  if (is_number()) {
    return ValueType::Number;
  } else if (is_string()) {
    return ValueType::String;
  } else if (is_error()) {
    return ValueType::Error;
  } else if (is_function()) {
    return ValueType::Function;
  } else if (is_array()) {
    return ValueType::Array;
  } else if (is_object()) {
    return ValueType::Object;
  } else if (is_boolean()) {
    return ValueType::Boolean;
  } else if (is_undefined()) {
    return ValueType::Undefined;
  } else if (is_null()) {
    return ValueType::Null;
  } else if (is_function()) {
    return ValueType::Function;
  } else {
    return ValueType::Unknown;
  }
}

auto Value::is_number() const -> bool {
  return JSValueIsNumber(this->internal->context, this->internal->value);
}

auto Value::is_string() const -> bool {
  return JSValueIsString(this->internal->context, this->internal->value);
}

auto Value::is_undefined() const -> bool {
  return JSValueIsUndefined(this->internal->context, this->internal->value);
}

auto Value::is_array() const -> bool {
  return JSValueIsArray(this->internal->context, this->internal->value);
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

auto Value::is_null() const -> bool {
  return JSValueIsNull(this->internal->context, this->internal->value);
}

auto Value::is_function() const -> bool {
  if (!is_object()) {
    return false;
  }

  JSObjectRef object =
      get_current_object(this->internal->context, this->internal->value);

  return JSObjectIsFunction(this->internal->context, object);
}

auto Value::to_number() const -> double {
  assert(is_number());
  JSValueRef exception = nullptr;
  const auto result = JSValueToNumber(this->internal->context,
                                      this->internal->value, &exception);
  assert(!exception);
  return result;
}

auto Value::to_string() const -> std::string {
  assert(is_string());
  return js_value_to_std_string(this->internal->context, this->internal->value);
}

auto Value::to_boolean() const -> bool {
  assert(is_boolean());
  return JSValueToBoolean(this->internal->context, this->internal->value);
}

auto Value::to_function() const -> Function {
  assert(is_function());

  return [context = this->internal->context,
          value = this->internal->value](std::vector<Value> args) {
    JSValueRef exception = nullptr;
    JSObjectRef func = JSValueToObject(context, value, &exception);
    assert(!exception);
    assert(func != nullptr);

    std::vector<JSValueRef> js_args;
    js_args.reserve(args.size());
    for (Value &arg : args) {
      js_args.push_back(static_cast<JSValueRef>(arg.native()));
    }

    // Now we can call the function, get the result and return through
    // the Value class.
    JSValueRef result = JSObjectCallAsFunction(
        context, func, nullptr, js_args.size(), js_args.data(), nullptr);
    assert(result != nullptr);
    return Value{context, result};
  };
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
    JSStringRelease(property_string);
    return std::nullopt;
  }

  JSValueRef result = JSObjectGetProperty(this->internal->context, object,
                                          property_string, &exception);
  assert(!exception);
  JSStringRelease(property_string);

  // The function could not be called outside of this object context
  // as the C++ function store is tied to the object.
  // This is why we don't support function properties.
  if (JSObjectIsFunction(
          this->internal->context,
          JSValueToObject(this->internal->context, result, &exception))) {
    throw std::runtime_error{"Function properties are not supported"};
  }
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

auto Value::set(const std::string &name, Function cpp_function) -> void {
  assert(is_object());

  // Convert function name
  JSStringRef function_name_ref = JSStringCreateWithUTF8CString(name.c_str());

  // Create a callback for JavascriptCore
  JSObjectCallAsFunctionCallback callback =
      [](JSContextRef context_ref, JSObjectRef function_ref,
         JSObjectRef this_object, size_t argc, const JSValueRef args[],
         JSValueRef * /* *exception */) -> JSValueRef {
    // First we need to retrieve the C++ function from the parent object.
    auto *function_store =
        static_cast<Value::FunctionStorage *>(JSObjectGetPrivate(this_object));
    assert(function_store != nullptr);
    Function &retrieved_cpp_function = function_store->at(function_ref);

    // Then, we create a vector of Value arguments.
    std::vector<Value> arguments;
    for (std::size_t index = 0; index < argc; index++) {
      arguments.emplace_back(context_ref, args[index]);
    }

    // Finally we call the C++ function passing the arguments
    // and return the result.
    Value result = retrieved_cpp_function(std::move(arguments));
    return static_cast<JSValueRef>(result.native());
  };

  // Convert callback JavascriptCore function
  JSObjectRef function_obj_ref = JSObjectMakeFunctionWithCallback(
      this->internal->context, function_name_ref, callback);
  assert(function_obj_ref != nullptr);

  // Get the parent object, where we will store the c++ function store.
  JSValueRef exception = nullptr;
  JSObjectRef parent_obj = JSValueToObject(this->internal->context,
                                           this->internal->value, &exception);
  assert(!exception);
  assert(parent_obj != nullptr);

  // Retrieve the existing function store from the parent object.
  // If it doesn't exist, create a new one.
  auto existing_function_store =
      static_cast<Value::FunctionStorage *>(JSObjectGetPrivate(parent_obj));
  if (existing_function_store == nullptr) {
    existing_function_store = new Value::FunctionStorage{};
  }
  existing_function_store->emplace(function_obj_ref, std::move(cpp_function));

  // Store the cpp_function in a private field of the parent object
  // so we can retrieve it later.
  bool is_function_stored =
      JSObjectSetPrivate(parent_obj, existing_function_store);
  (void)is_function_stored; // Avoid unused variable warning (in release mode.)
  assert(is_function_stored);

  // Set the function in current object.
  set(name, Value{this->internal->context, function_obj_ref});

  // Release the function name.
  JSStringRelease(function_name_ref);
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

auto Value::to_vector() const -> std::vector<Value> {
  assert(is_array());
  JSObjectRef object =
      get_current_object(this->internal->context, this->internal->value);
  std::size_t length = get_object_length(this->internal->context, object);

  JSValueRef exception = nullptr;
  std::vector<Value> vector;
  for (unsigned int index = 0; index < static_cast<std::size_t>(length);
       index++) {
    JSValueRef value = JSObjectGetPropertyAtIndex(this->internal->context,
                                                  object, index, &exception);
    assert(!exception);
    vector.emplace_back(static_cast<JSContextRef>(this->internal->context),
                        value);
  }
  return vector;
}

auto Value::at(const unsigned int &position) const -> std::optional<Value> {
  assert(is_array());
  JSObjectRef object =
      get_current_object(this->internal->context, this->internal->value);
  std::size_t length = get_object_length(this->internal->context, object);

  if (position >= static_cast<std::size_t>(length)) {
    return std::nullopt;
  }

  JSValueRef exception = nullptr;
  JSValueRef result = JSObjectGetPropertyAtIndex(this->internal->context,
                                                 object, position, &exception);
  assert(!exception);
  return std::make_optional<Value>(this->internal->context, result);
}

auto Value::push(Value value) -> void {
  assert(is_array());
  JSObjectRef object =
      get_current_object(this->internal->context, this->internal->value);
  std::size_t length = get_object_length(this->internal->context, object);

  JSValueRef exception = nullptr;
  JSObjectSetPropertyAtIndex(
      this->internal->context, object, static_cast<unsigned int>(length),
      static_cast<JSValueRef>(value.native()), &exception);
  assert(!exception);
}

auto Value::native() const -> const void * {
  return static_cast<const void *>(this->internal->value);
}

} // namespace includejs

#include <includejs/engine.h>

#include <iostream>

// This ignores the compiler warnings that get produced by the MSVC compiler
// while including the v8.h header file.
#if defined(_MSC_VER)
#pragma warning(push)
// warning C4100: '...': unreferenced formal parameter
#pragma warning(disable : 4100)
// warning C4127: conditional expression is constant
#pragma warning(disable : 4127)
// warning C4458: declaration of 'identifier' hides class member
#pragma warning(disable : 4458)
#endif
#include <v8.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <libplatform/libplatform.h>

namespace includejs {

struct Engine::Internal {};

Engine::Engine() : internal{std::make_unique<Engine::Internal>()} {
  // TODO(RaisinTen): This code is here to make it easy to verify if the V8
  // integration is working.

  // This example code has been taken from:
  // https://chromium.googlesource.com/v8/v8/+/branch-heads/11.9/samples/hello-world.cc

  // Initialize V8.
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate *isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
      // Create a string containing the JavaScript source code.
      v8::Local<v8::String> source =
          v8::String::NewFromUtf8Literal(isolate, "'Hello' + ', World!'");
      // Compile the source code.
      v8::Local<v8::Script> script =
          v8::Script::Compile(context, source).ToLocalChecked();
      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(isolate, result);
      printf("%s\n", *utf8);
    }
    {
      // Use the JavaScript API to generate a WebAssembly module.
      //
      // |bytes| contains the binary format for the following module:
      //
      //     (func (export "add") (param i32 i32) (result i32)
      //       get_local 0
      //       get_local 1
      //       i32.add)
      //
      const char csource[] = R"(
        let bytes = new Uint8Array([
          0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x01,
          0x60, 0x02, 0x7f, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07,
          0x07, 0x01, 0x03, 0x61, 0x64, 0x64, 0x00, 0x00, 0x0a, 0x09, 0x01,
          0x07, 0x00, 0x20, 0x00, 0x20, 0x01, 0x6a, 0x0b
        ]);
        let module = new WebAssembly.Module(bytes);
        let instance = new WebAssembly.Instance(module);
        instance.exports.add(3, 4);
      )";
      // Create a string containing the JavaScript source code.
      v8::Local<v8::String> source =
          v8::String::NewFromUtf8Literal(isolate, csource);
      // Compile the source code.
      v8::Local<v8::Script> script =
          v8::Script::Compile(context, source).ToLocalChecked();
      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
      // Convert the result to a uint32 and print it.
      uint32_t number = result->Uint32Value(context).ToChecked();
      printf("3 + 4 = %u\n", number);
    }
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete create_params.array_buffer_allocator;
}

Engine::~Engine() {}

} // namespace includejs

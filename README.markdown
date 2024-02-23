<p align="center"><img src="./assets/banner.png" alt="includejs logo"/></p>
<h1 align="center">includejs</h1>
<h3 align="center">Build your own JavasScript runtime</h3>
<p align="center">
  <a href="https://github.com/crossnx/includejs/actions/workflows/ci.yml"><img src="https://github.com/crossnx/includejs/actions/workflows/ci.yml/badge.svg" alt="github action build"></a>
</p>

<p align="center">
    <img referrerpolicy="no-referrer-when-downgrade" src="https://static.scarf.sh/a.png?x-pxid=8107db3b-fcee-4af8-81f0-14e29ec04430" />
    <a href="www.includejs.org/">📖 Documentation</a>
</p>

`includejs` offers a unified API for building your own JavaScript runtime, bridging multiple engines and platforms with ease.

## Features

- Multiple runtime support: `JavascriptCore`, `v8` (more in the furture)
- Cross platform support
- Direclty consumable from `CMake`

> 🤙 Stay tuned as we continue to expand our feature set.

### JavasScript

| Feature        | JavaScriptCore    | V8               |
|----------------|-------------------|------------------|
| Boolean        | ✅ Supported      | ❌ Not Supported |
| Number         | ✅ Supported      | ❌ Not Supported |
| String         | ✅ Supported      | ❌ Not Supported |
| Object         | ✅ Supported      | ❌ Not Supported |
| Array          | ✅ Supported      | ❌ Not Supported |
| Promise        | ✅ Supported      | ❌ Not Supported |
| Function       | ✅ Supported      | ❌ Not Supported |
| Error          | ✅ Supported      | ❌ Not Supported |
| Class          | ❌ Not Supported  | ❌ Not Supported |
| Set            | ❌ Not Supported  | ❌ Not Supported |
| Map            | ❌ Not Supported  | ❌ Not Supported |
| BigInt         | ❌ Not Supported  | ❌ Not Supported |
| Symbol         | ❌ Not Supported  | ❌ Not Supported |
| ArrayBuffer    | ❌ Not Supported  | ❌ Not Supported |
| TypedArray     | ❌ Not Supported  | ❌ Not Supported |

### WHATWG APIs

- [ ] Timers
- [ ] Console

### Platform support

| Platform  | JavaScriptCore    | V8               |
|-----------|-------------------|------------------|
| macOS     | ✅ Supported      | ✅ Supported     |
| Linux     | ✅ Supported      | ✅ Supported     |
| Windows   | ❌ Not Supported  | ❌ Not Supported |

## Dependencies

- C++
- CMake
- JavascriptCore
- Brew (macOS)

## Usage

You can consume `includejs` just with CMake:

```cmake
cmake_minimum_required(VERSION 3.18)

project(your_runtime VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  includejs
  GIT_REPOSITORY https://github.com/crossnx/includejs
  GIT_TAG        main
  DOWNLOAD_EXTRACT_TIMESTAMP NO)
FetchContent_MakeAvailable(includejs)

add_executable(your_runtime main.cc)
target_link_libraries(your_runtime PRIVATE includejs::engine)
```


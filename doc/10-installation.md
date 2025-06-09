# Installation Guide

Get xit-test up and running in your C++ project.

## Prerequisites

- **CMake 3.21+** 
- **C++20 compatible compiler** (GCC 11+, Clang 13+) (MSVC not verified)
- **vcpkg** package manager

> **New to vcpkg?** Install it following the [official vcpkg guide](https://vcpkg.io/en/getting-started.html)

## Quick Start

For a complete working example, see the [minimal repository template](https://github.com/njaldea/cpp_consume).

### 1. Add nil-vcpkg-ports Registry

Create or update `vcpkg-configuration.json` in your project root:

```json
{
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg-configuration.schema.json",
    "default-registry": {
        "kind": "git",
        "repository": "https://github.com/Microsoft/vcpkg",
        "baseline": "3508985146f1b1d248c67ead13f8f54be5b4f5da"
    },
    "registries": [
        {
            "kind": "git",
            "repository": "https://github.com/njaldea/nil-vcpkg-ports",
            "baseline": "07510033fa0ece1d0b25b8eca60c3ecc99f9ef80",
            "packages": ["nil-*"]
        }
    ]
}
```

### 2. Add Dependencies

Create or update `vcpkg.json` in your project root:

```json
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
  "name": "my-project",
  "version": "1.0.0",
  "dependencies": [
    { "name": "nil-xit-test", "features": ["gtest"] }
  ]
}
```

### 3. Configure CMake with vcpkg

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```

### 4. Use xit-test in Your CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.21)
project(my-project)

# Find xit-test with gtest component
find_package(nil-xit-test CONFIG REQUIRED COMPONENTS gtest)

# Your library/code under test
add_library(my_lib INTERFACE)
target_sources(my_lib INTERFACE Circle.hpp)

# Create a visual test
add_xit_test(
    my_visual_test
    SOURCES
        test_circle.cpp
        # Add more test files as needed
    GROUPS
        # Path to your UI components
        "ui=${CMAKE_CURRENT_SOURCE_DIR}/test_ui"
)

# Link your code and the xit-test framework
target_link_libraries(my_visual_test PRIVATE my_lib)
target_link_libraries(my_visual_test PRIVATE nil::xit-gtest)
```

### 5. Build and Run

```bash
# Build the project
cmake --build build

# Run tests normally (headless mode)
ctest --test-dir build

# Or run specific test directly
ninja my_visual_test

# Launch GUI mode
ninja my_visual_test_gui
```

## What `add_xit_test` Provides

The `add_xit_test` function automatically:

- ✅ **Creates an executable** for headless test execution
- ✅ **Downloads web assets** needed for the GUI automatically  
- ✅ **Integrates with CTest** for automated testing
- ✅ **Generates GUI target** (`<test_name>_gui`) with all arguments pre-configured
- ✅ **Handles asset dependencies** and group paths automatically

**Two ways to run your tests:**
- **Headless**: `./build/my_visual_test` or `ctest`
- **GUI**: `cmake --build build --target my_visual_test_gui`

## Verification

To verify your installation works:

1. **Build succeeds** without errors: `cmake --build build`
2. **Test runs** in headless mode: `./build/my_visual_test`
3. **GUI launches** easily: `cmake --build build --target my_visual_test_gui`

## Running GUI Tests

You have multiple convenient options to launch the visual interface:

```bash
# Using CMake (recommended)
cmake --build build --target my_visual_test_gui

# Using Ninja directly (if using Ninja generator)
ninja -C build my_visual_test_gui

# Using Make (if using Unix Makefiles generator)  
make -C build my_visual_test_gui
```

## Next Steps

- 📖 **[Migration Guide](./02-migration.md)** - Convert your first test
- 🎯 **[Core Concepts](./03-concepts.md)** - Understanding frames and groups
- 🔧 **[CLI Commands](./04-cli.md)** - Advanced usage options

## Dependencies

xit-test automatically installs these dependencies through vcpkg:

### Core Framework Dependencies
- **nil-xit** - Core visualization engine
- **nil-service** - Web server functionality  
- **nil-clix** - Command line interface
- **nil-xalt** - Utility library

### Third-Party Libraries
- **flatbuffers** - Serialization for data exchange
- **boost-program-options** - Command line argument parsing
- **boost-asio** - Asynchronous I/O operations
- **boost-beast** - HTTP/WebSocket server implementation

### Test Framework
- **GoogleTest** - Required when using the `gtest` feature

> **Note**: All dependencies are handled automatically by vcpkg. You don't need to install them manually.
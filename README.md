# nil-xit-test

> Interactive visual testing for C++

![version](https://img.shields.io/badge/status-early%20development-blue)

xit-test adds interactive visualization to your existing C++ tests. See your data, adjust parameters in real-time, and share visual test results with your team.

## Why xit-test?

Traditional tests tell you **if** code works. xit-test shows you **how** and **why**:

- **📊 Visualize data** - See complex structures and algorithm behaviors
- **⚡ Fast iteration** - Change parameters without recompiling  
- **👥 Better communication** - Share visual results with teammates
- **📚 Living documentation** - Tests that explain themselves

xit-test works with your existing test framework (currently supports Google Test).

## Quick Start

**1. Define visual frames**
```cpp
// Input frame with UI controls
XIT_FRAME_TEST_INPUT_V("circle", "$group/CircleInput.svelte", Circle())
    .value("radius", &Circle::radius);

// Output frame for results  
XIT_FRAME_OUTPUT_V("result", "$group/ResultOutput.svelte", Circle);
```

**2. Write your test**
```cpp
using MyTest = nil::xit::gtest::Test<
    nil::xit::gtest::Input<"circle">,
    nil::xit::gtest::Output<"result">
>;

XIT_TEST_F(MyTest, visualize, "$group/test")
{
    const auto& [input] = xit_inputs;
    auto& [output] = xit_outputs;
    
    output = processCircle(input);  // Your logic
}
```

**3. Run with GUI**
```bash
./test gui -g "group=./ui" -a "./node_modules/@nil-/xit/assets"
```

## Key Features

- **Live parameter adjustment** through web UI
- **Interactive result visualization** 
- **Test input persistence** for regression testing
- **Works with existing test frameworks**

## Documentation

**Getting Started**
- [Migration Guide](./doc/02-migration.md) - Convert your first test
- [Core Concepts](./doc/03-concepts.md) - Key ideas and terminology

**Reference** 
- [Complete Documentation](./doc/01-gtest.md) - All documentation links
- [MACROS Reference](./doc/05-MACROS.md) - API reference
- [Best Practices](./doc/09-conclusion.md) - When and how to use xit-test

**Framework Support**
- Google Test (GTest) - Available now
- More frameworks coming soon
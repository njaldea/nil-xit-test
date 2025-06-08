# nil-xit-test

> Visualize C++ tests to understand complex behaviors, adjust parameters in real-time, and share insights with your team.

![version](https://img.shields.io/badge/status-early%20development-blue)

nil-xit-test is a C++ library that extends existing testing frameworks with interactive visualization capabilities.

## Why nil-xit-test?

Traditional C++ tests tell you if code works, but not how or why. nil-xit-test bridges this gap:

1. **📊 Make the invisible visible** - See complex data structures and algorithm behaviors in interactive visualizations
2. **⚡ Reduce iteration time** - Adjust test parameters without recompilation
3. **👥 Enhance collaboration** - Share visual test results with teammates for better discussions
4. **📚 Preserve knowledge** - Tests become visual documentation of how and why code works

**nil-xit-test enhances your existing framework** rather than replacing it. While Google Test (GTest) is the first supported framework, the architecture is designed to support multiple testing frameworks.

## How It Works

nil-xit-test transforms your tests in three simple steps:

**1. Register frames for visualization**
```cpp
// Define frames outside your test function
NIL_XIT_FRAME_INPUT(Circle, MyTest)
    .value("radius", &Circle::radius)
    .value("center", &Circle::center);

NIL_XIT_FRAME_OUTPUT(std::string, MyTest);
```

**2. Write your test using these frames**
```cpp
// Test function accesses registered frames
XIT_TEST(Circle, Visualize)
{
    // Access input and output frames
    const auto& [ circle ] = xit_inputs;
    auto& [ result ] = xit_outputs;
    
    // Your test logic stays the same
    result = "Circle area: " + std::to_string(circle.area());
}
```

**3. View and interact with your test**
- Adjust inputs through the UI without recompiling
- See results visually rendered in real-time
- Save inputs for regression testing

## Features

- **🔄 Live Parameter Adjustment** - Modify test inputs through a visual interface
- **📊 Interactive Visualization** - Display test results in custom visual formats  
- **🗂️ Organized Test Groups** - Create hierarchical organization of related tests
- **💾 Input Persistence** - Save and reuse test inputs across test runs
- **🔌 Framework Integration** - Works with your existing test framework

## Getting Started

Choose a framework integration to begin:

- [nil-xit-gtest](./doc/01-gtest.md) - Google Test integration (**current focus**)
- More integrations coming soon

## Architecture

nil-xit-test is designed around these core concepts:

- **Frames** - Visual components for inputs and outputs
- **Groups** - Logical organization of tests and frames
- **Bindings** - Connect C++ data structures to visual components
- **Framework Bridges** - Adapt existing test frameworks to the visualization layer

## Where to Learn More

- [Migration Guide](./doc/02-migration.md) - Convert existing tests to nil-xit-test
- [Core Concepts](./doc/03-concepts.md) - Understanding the key abstractions
- [Full Documentation](./doc/01-gtest.md) - Comprehensive reference
- [Conclusion & Use Cases](./doc/09-conclusion.md) - When and how to best use nil-xit-test
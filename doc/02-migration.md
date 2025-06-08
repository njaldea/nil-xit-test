# Migration Guide: GTest to nil-xit-test

Convert your first Google Test to a visual test in about 15 minutes.

## Step 1: Start with a GTest

```cpp
// Circle.hpp
struct Circle {
    double x, y, radius;
};

// test.cpp
#include <Circle.hpp>
#include <gtest/gtest.h>

TEST(Sample, circle) {
    auto circle = Circle{0.1, 0.1, 1.0};
    // Your test logic here
}
```

## Step 2: Convert to nil-xit-test

Replace `TEST` with `XIT_TEST` and add a group path:

```cpp
#include <Circle.hpp>
#include <nil/xit/gtest.hpp>

XIT_TEST(Sample, circle, "$group/.") {
    auto circle = Circle{0.1, 0.1, 1.0};
    // Your test logic here
}
```

Run the test (assuming project is already built):
```bash
# From .build directory
./bin/step_2 -g "group=../doc/test"
```

The test works the same as before, but now supports visualization.

## Step 3: Add Visual Output

Add frames to visualize your test results. This introduces the GUI:

```cpp
// Main frame (required for GUI to work)
XIT_FRAME_MAIN("$group/Step_3Main.svelte");

// Define what data to visualize
XIT_FRAME_OUTPUT_V("output", "$group/Step_3.svelte", Circle)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

// Equality operator (required for nil-xit-test)
bool operator==(const Circle& l, const Circle& r) {
    return l.x == r.x && l.y == r.y && l.radius == r.radius;
}

// Create test fixture
using MyTest = nil::xit::gtest::Test<
    nil::xit::gtest::Input<>,
    nil::xit::gtest::Output<"output">
>;

XIT_TEST_F(MyTest, circle, "$group/.") {
    auto circle = Circle{0.1, 0.1, 1.0};
    
    // Pass result to output frame
    get<0>(xit_outputs) = circle;
}
```

> **Note:** `XIT_FRAME_MAIN` is required for the GUI to function - it enables the browser to fetch test information and display the interface. You can see an example main frame component at [Step_3Main.svelte](./test/Step_3Main.svelte).

Create `Step_3.svelte` to display your data (see example: [Step_3.svelte](./test/Step_3.svelte)):

```svelte
<script lang="ts">
    import { xit } from "@nil-/xit";
    
    const { values } = xit();
    const x = values.double("x", 0);
    const y = values.double("y", 0);
    const r = values.double("radius", 0);
</script>

<div>x - {$x}</div>
<div>y - {$y}</div>
<div>r - {$r}</div>
```

Run with GUI (assuming frontend dependencies are installed):

```bash
# From .build directory
./bin/step_3 gui -g "group=../doc/test" -a "../doc/node_modules/@nil-/xit/assets"
```

Your test now has a visual interface! You can see the circle's properties displayed in the web UI.

## Step 4: Add Input Parameters

Now let's make test parameters modifiable through the UI:

```cpp
// Main frame (required for GUI)
XIT_FRAME_MAIN("$group/Step_4Main.svelte");

// Output frame for results
XIT_FRAME_OUTPUT_V("output", "$group/Step_4Output.svelte", Circle)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

// Register an input frame
XIT_FRAME_TEST_INPUT_V("input", "$group/Step_4Input.svelte", Circle{0.1, 0.1, 0.1})
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

// Equality operator (required for nil-xit-test)
bool operator==(const Circle& l, const Circle& r) {
    return l.x == r.x && l.y == r.y && l.radius == r.radius;
}

// Update test fixture to include input
using MyTest = nil::xit::gtest::Test<
    nil::xit::gtest::Input<"input">,
    nil::xit::gtest::Output<"output">
>;

XIT_TEST_F(MyTest, circle, "$group/.") {
    const auto& [circle] = xit_inputs;  // Get input from UI
    
    // Your test logic here
    
    get<0>(xit_outputs) = circle;  // Pass to output
}
```

> **Note:** You'll need a main frame component - see [Step_4Main.svelte](./test/Step_4Main.svelte) for an example that provides navigation links.

Create `Step_4Input.svelte` for interactive input (see example: [Step_4Input.svelte](./test/Step_4Input.svelte)):

```svelte
<script lang="ts">
    import { xit } from "@nil-/xit";
    
    const { values } = xit();
    const x = values.double("x", 0);
    const y = values.double("y", 0);
    const r = values.double("radius", 0);
</script>

<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$x}>x</label></div>
<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$y}>y</label></div>
<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$r}>r</label></div>
```

Run the test:

```bash
# From .build directory
./bin/step_4 gui -g "group=../doc/test" -a "../doc/node_modules/@nil-/xit/assets"
```

## Step 5: Add File Persistence

Persist input changes between test runs:

```cpp
#include "Circle.hpp"
#include "nil/xit/gtest/utils/from_file.hpp"

#include <nil/xit/gtest.hpp>

// Main frame (required for GUI)
XIT_FRAME_MAIN("$group/Step_4Main.svelte");

// Output frame for results
XIT_FRAME_OUTPUT_V("output", "$group/Step_4Output.svelte", Circle)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

// Use file utilities to save/load inputs
XIT_FRAME_TEST_INPUT_V(
    "input", 
    "$group/Step_4Input.svelte",
    nil::xit::gtest::from_file_with_update<Circle, "step_5_input">()
)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

// Equality operator (required for nil-xit-test)
bool operator==(const Circle& l, const Circle& r) {
    return l.x == r.x && l.y == r.y && l.radius == r.radius;
}

// Test fixture with input and output
using MyTest = nil::xit::gtest::Test<
    nil::xit::gtest::Input<"input">,
    nil::xit::gtest::Output<"output">
>;

XIT_TEST_F(MyTest, circle, "$group/.") {
    const auto& [circle] = xit_inputs;  // Get input from UI
    
    // Your test logic here
    
    get<0>(xit_outputs) = circle;  // Pass to output
}
```

Run the test:

```bash
# From .build directory
./bin/step_5 gui -g "group=../doc/test" -a "../doc/node_modules/@nil-/xit/assets"
```

Now when you modify parameters in the UI, they're automatically saved to `step_5_input` file in your group directory and will persist between test runs.

## Next Steps

You now have a complete visual testing setup! You can:

- **Experiment with parameters** in real-time through the UI
- **Save interesting test cases** that persist between runs
- **Share test scenarios** by committing the input files
- **Create more complex visualizations** for your specific use cases

See the [complete documentation](./01-gtest.md) for more advanced usage.
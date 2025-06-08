# Migrating GTest to nil-xit-gtest

## Step 1 - Write A GTest test

```cpp
// Circle.hpp
struct Circle
{
    double x;
    double y;
    double radius;
};
```

```cpp
#include <Circle.hpp>

#include <gtest/gtest.h>

TEST(Sample, circle)
{
    auto sut = Circle{
        .x=0.1,
        .y=0.1,
        .radius=1
    };

    // do your test here
}
```

Having a bunch of "magic" or "hardcoded" values is very common in writing tests.

Let's start transforming this test for us to visualize it.

## Step 2 - Convert to `nil-xit-gtest`

The main difference of nil-xit-gtest are the following:
- Test MACRO used
- Always depend on a directory

```cpp
#include <Circle.hpp>

#include <nil/xit/gtest.hpp>

XIT_TEST(Sample, circle, "$group/.")
{
    auto sut = Circle{
        .x=0.1,
        .y=0.1,
        .radius=1
    };

    // do your test here
}
```

> Note: `$group/` refers to a group path in your file system. See [Concepts: Group](./03-concepts.md#group) for more details on how groups work.

### Execution

`nil-xit-gtest` requires an additional command line argument to run.

```
./bin/<binary> -g "group=."
```

The `-g "group=."` option maps the group name to a filesystem path. See [Concepts: Group](./03-concepts.md#group) for more details.

### Differences

| gtest           | nil-xit-gtest               | description                                       |
|-----------------|-----------------------------|---------------------------------------------------|
| `TEST`          | `XIT_TEST`                  | additional 3rd argument is necessary              |
|                 | "$group/."                  | a directory path to be used to generate the test  |
| `Sample.circle` | `Sample.circle[group:.]`   | test ID format with group information ([learn more](./03-concepts.md#test-identification)) |

When ran, there is technically no difference with gtest.

The first problem we'll try to solve is to visualize the test. Let's start adding an output

## Step 3 - Add an Output

In this step, we'll solve our first problem: visualizing test data. By adding an output frame, we can see the Circle's properties visually rather than just in code.

> Throughout the following steps, notice how we use `$group/` paths to reference UI files. See [Concepts: Group](./03-concepts.md#group) for detailed explanation of group paths.

### Step 3.1 - Register an Output Frame

```cpp
XIT_FRAME_OUTPUT_V("output", "$group/Step3.svelte", Circle)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);
    
bool operator==(const Circle& l, const Circle& r)
{
    return l.x == r.x && l.y == r.y && l.radius == r.radius;
}
```

| parameter | value                       | description                     |
|-----------|-----------------------------|---------------------------------|
| frame id  | "output"                    | unique identifier of the frame  |
| file info | "$group/OutputFrame.svelte" | group and path to frame UI file |
| type      | Circle                      | type represented by the output  |

### Step 3.2 - Create a Fixture

```cpp
#include <Circle.hpp>

#include <nil/xit/gtest.hpp>

using Sample = nil::xit::gtest::Test<
    nil::xit::gtest::Input<>,
    nil::xit::gtest::Output<"output">
>;

XIT_TEST(Sample, circle, "$group/.")
{
    auto sut = Circle{
        .x=0.1,
        .y=0.1,
        .radius=1
    };

    // do your test here

    get<0>(xit_outputs) = sut;
}
```

This example test simply passes forward sut to the output.

`xit_outputs` is a tuple-like object, each element representing the outputs defined in the fixture.

### Step 3.3 - Implement the Main UI

This library uses svelte as the frontend framework.

```svelte
<a href="/?frame_id=output&tag=Sample.circle[group:.]">Sample.circle[group:.]</a>
```

In C++, register Main UI

```cpp
XIT_FRAME_MAIN("$group/Step_3Main.svelte");
```

### Step 3.4 - Implement the Output Frame

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

### Step 3.5 - Install pnpm and @nil-/xit

If serving locally, html/js files are necessary.

```bash
# Install pnpm if not already installed
npm install -g pnpm

# Navigate to your project's UI directory
cd doc

# Install dependencies
pnpm install

# Add the @nil-/xit package
pnpm add @nil-/xit
```

The @nil-/xit package provides the frontend components needed to visualize your tests.

### Step 3.6 - Run the test in gui mode

```
./bin/<binary> gui -g "group=." -a "../doc/node_modules/@nil-/xit/assets"
```

Note the `-g "group=."` option which maps the group name to a filesystem path. See [Concepts: Group](./03-concepts.md#group) for details on group mapping.

// image for the gui for an example

With this step completed, you can now see your Circle data in the UI after the test runs. Next, we'll tackle the problem of hardcoded inputs.

## Step 4 - Add an Input

In gtest, inputs are normally hardcoded in the test.

In `nil-xit-gtest`, we can move this input to a Frame, enabling us to modify it through the UI.

### Step 4.1 - Register an Input Frame

```cpp
XIT_FRAME_TEST_INPUT_V("input", "$group/Step_4Input.svelte", Circle{0.1, 0.1, 0.1})
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);
    
bool operator==(const Circle& l, const Circle& r)
{
    return l.x == r.x && l.y == r.y && l.radius == r.radius;
}
```

| parameter | value                       | description                     |
|-----------|-----------------------------|---------------------------------|
| frame id  | "input"                     | unique identifier of the frame  |
| file info | "$group/Step_4Input.svelte" | group and path to frame UI file |
| instance  | Circle{0.1, 0.1, 0.1}       | instance for initial value      |

NOTE: `type` requires equality operator.

### Step 4.2 - Use in the Fixture

```cpp
#include <Circle.hpp>

#include <nil/xit/gtest.hpp>

using Sample = nil::xit::gtest::Test<
    nil::xit::gtest::Input<"input">, // <--
    nil::xit::gtest::Output<"output">
>;

XIT_TEST(Sample, circle, "$group/.")
{
    const auto& [sut] = xit_inputs;

    // do your test here

    get<0>(xit_outputs) = sut;
}
```

`xit_inputs` is a tuple-like object, each element representing the inputs defined in the fixture.

### Step 4.3 - Adjust the Main UI to include input frame

This library uses svelte as the frontend framework.

```svelte
<a href="/?frame=input&tag=Sample.circle[group:.]" target="_blank">Sample.circle[group:.] - input</a>
<br/>
<a href="/?frame=output&tag=Sample.circle[group:.]" target="_blank">Sample.circle[group:.] - output</a>
```

In C++, register Main UI

```cpp
XIT_FRAME_MAIN("$group/Step_4Main.svelte");
```

### Step 4.4 - Implement the Input Frame

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

For every change in the input, the test will rerun.

This can help in tinkering, parameter fine tuning, but there is a third big problem we still need to address.

The Inputs are still hardcoded in the test. Once we terminate the test, and rerun it, the changes are not stored.

`nil-xit-gtest` provides mechanism to store the changes in the filesystem, which we will tackle in Step 4.

## Step 5 Save Input to File

The library provides a couple of utilities to read/write the inputs to a file. For this example, let's use `from_file_with_update` for convenience.

`from_file_with_update` will read the file from a designated directory, and automatically save the changes on "every" changes

```cpp
XIT_FRAME_TEST_INPUT_V(
    "input",
    "$group/Step_4Input.svelte",
    nil::xit::gtest::from_file_with_update<Circle, "step_5_input">()
)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);
```

### Step 5.1 - Understanding Where Files Are Stored

The input files will be stored in a location determined by your group mapping. When you run your test with:

```bash
./bin/<binary> gui -g "group=./path/to/group" -a "../doc/node_modules/@nil-/xit/assets"
```

The input file (`step_5_input`) will be stored in `./path/to/group/step_5_input`.

### Step 5.2 - Reusing Saved Inputs

Once saved, the input values will persist between test runs. This allows you to:

1. Fine-tune parameters through the UI
2. Save complex test cases for regression testing
3. Share test inputs with team members by committing the files to version control

## Conclusion

By following this migration guide, you've transformed a basic GTest with hardcoded values into a visually interactive test that can:

1. Display test outputs in a visual format
2. Modify test inputs through the UI
3. Persist changes for future test runs

These capabilities make it easier to understand, debug, and refine tests, especially those with complex data structures or visual components.

For more detailed information on specific aspects of nil-xit-gtest, refer to the other documentation sections:

- [Core Concepts](./03-concepts.md)
- [CLI Options](./04-cli.md) 
- [MACROS Reference](./05-MACROS.md)
- [Custom Types](./06-custom-types.md)
- [Frame Bindings](./07-frame-bindings.md)
- [File Utilities](./08-file-utilities.md)
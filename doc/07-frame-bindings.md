# Frame Bindings

Bind C++ data to frontend UI components with granular control.

> **Related documentation:**
> - [MACROS](./05-MACROS.md) - Frame registration syntax
> - [Custom Types](./06-custom-types.md) - Working with complex data types
> - [File Utilities](./08-file-utilities.md) - Persisting frame data

## Frame Types Overview

- **Input Frames** (`XIT_FRAME_*_INPUT*`): Allow binding values for data input and UI controls
- **Output Frames** (`XIT_FRAME_OUTPUT*`): Display test results and data visualization
- **Test vs Global**: Test frames are per-test instance, Global frames are shared across tests

```cpp
struct Ranges
{
    double v1;
    double v2;
    double v3;
};
```

## Binding Individual Members

The `.value()` method chains to frame registration macros to bind specific data members:

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);
```

This allows the frontend to access `value-1`, `value-2`, and `value-3` as separate values.

## Binding Whole Object

Bind the entire frame data as a single value:

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value");
```

The frontend can access the complete `Ranges` object under the name `"value"`.

## Binding Unrelated Data (Input Frames Only)

Input frames can bind additional data that's unrelated to the frame's primary data type:

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value", std::uint64_t(1101));
```

This creates a separate `"value"` binding with the integer `1101`, independent of the `Ranges` data.

> **Note**: This feature is only available for input frames. Output frames can only bind data derived from their primary data type.

## Custom Accessors

Use callable objects to customize data access:

```cpp
struct Accessor
{
    auto& operator()(Ranges& data) { return data.v1; }
    const auto& operator()(const Ranges& data) const { return data.v1; }
};

XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value", Accessor());
```

## Frontend Integration

Bound values become available in Svelte components:

```svelte
<script lang="ts">
    import { xit } from "@nil-/xit";

    const { values } = xit();
    
    // Access values by the identifiers used in .value() calls
    const v1 = values.double("value-1", 0);
    const v2 = values.double("value-2", 0);
    const v3 = values.double("value-3", 0);
</script>
```

> **Note:** Detailed frontend integration documentation is available separately.
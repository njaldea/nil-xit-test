# Frame Bindings

Bind C++ data to frontend UI components with granular control.

> **Related documentation:**
> - [MACROS](./05-MACROS.md) - Frame registration syntax
> - [Custom Types](./06-custom-types.md) - Working with complex data types
> - [File Utilities](./08-file-utilities.md) - Persisting frame data

```cpp
struct Ranges
{
    double v1;
    double v2;
    double v3;
};
```

## Binding Individual Members

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);
```

Allows frontend to access `value-1`, `value-2`, and `value-3` separately.

## Binding Whole Object

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value");
```

## Custom Accessors

Use callable objects to customize data access:

```cpp
struct Accessor
{
    auto& operator()(Ranges& data) const { return data.v1; }
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
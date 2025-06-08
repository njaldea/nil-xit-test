# Frame Bindings

MACROS registering a frame allows a more granular data binding with the frontend. This document explains how to bind C++ data to the frontend UI components.

> **Related documentation:**
> - [MACROS](./05-MACROS.md) - For frame registration syntax
> - [Custom Types](./06-custom-types.md) - For working with complex data types
> - [File Utilities](./08-file-utilities.md) - For persisting frame data

```cpp
struct Ranges
{
    double v1;
    double v2;
    double v3;
};
```

## Binding to member attributes

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);
```

The binding above allows the frontend to access value-1, value-2, and value-3 separately.

## Binding to the whole object

```cpp
XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value");
```

## Accessor

Accessor is a callable object that returns the data to be bound.

```cpp
struct Accessor
{
    auto& operator()(Ranges& data) const
    {
        return data.v1;
    }

    const auto& operator()(const Ranges& data) const
    {
        return data.v1;
    }
};

XIT_FRAME_GLOBAL_INPUT_V("frame_id", "$base/File.svelte", Ranges())
    .value("value", Accessor());
```

## Frontend Integration

The values bound through the `.value()` method become available in the frontend Svelte components. See the [Concepts: UI Components](./03-concepts.md#ui-components) section for more details on how the frontend accesses these values.

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

> **Note:** A more detailed documentation about the frontend integration will be available in the separate frontend documentation.
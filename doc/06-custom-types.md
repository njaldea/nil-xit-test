# Custom Types

The library has built-in support for the following types:

- `bool`
- `std::int64_t`
- `double`
- `std::string`
- `std::vector<std::uint8_t>`

> **Related documentation:**
> - [Frame Bindings](./07-frame-bindings.md) - For binding custom types to UI
> - [File Utilities](./08-file-utilities.md) - For serializing/deserializing custom types

To be able to use custom types for Frames, the following are needed to be available:

## operator==

Equality operator is necessary for the library to decide if the test has to be re-ran when there is a change in the input

## buffer_type specialization

```cpp
template <typename T>
struct nil::xit::buffer_type
{
    static nlohmann::json deserialize(
        const void* data,
        std::uint64_t size
    );
    static std::vector<std::uint8_t> serialize(
        const T& value
    );
};
```

The type has to be serializable to be sent to the frontend.

Equivalently, the frontend should be able to know how to deserialize the data.
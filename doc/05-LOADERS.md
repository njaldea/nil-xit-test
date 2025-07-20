# Loaders

Loader = code that gives a frame its start value (and maybe reacts later).

Two styles:
- Direct value (just a literal / object) → only implicit initialize
- Loader object with functions you implement

### Interface Shape
```
T initialize(...tag?) const;      // make starting value
void update(...tag?, const T&) const;    // optional mid-edit reaction
void finalize(...tag?, const T&) const;  // optional end reaction
```
Your code decides what update/finalize do (write file, log, recalc, noop).

### Tags
- Per‑test frames (test input, expect, test utility) pass a tag.
- Global frames do not.
- Tag form: part of test id `SUITE.CASE[$group:path]`.
- Path ending `/*` → one tag per subfolder.

### Built‑In File Helpers (from_file.hpp)
Binary helpers that read/write using `file_codec<T>` (serialize via `buffer_type<T>`). Use only if binary blobs are fine.

| Helper                              | Hooks provided       | Path Mode                                      | When to use |
|:------------------------------------|:---------------------|:-----------------------------------------------|:------------|
| `from_file<T, "name">`              | initialize           | `$name` → group root ; else tag + name         | Read only   |
| `from_file_with_update<T, "name">`  | initialize + update  | same                                           | Save every edit |
| `from_file_with_finalize<T, "name">`| initialize + finalize| same                                           | Save at end |

Path rule:
- File literal starts with `$` → treat as group-rooted fixed path.
- Else → combine tag + file name (one file per test instance).

Support funcs:
- `resolve_from_path(p)` → expand `$group/...`.
- `resolve_from_tag(tag, p)` → build per-test path.
- `file_codec<T>::read / write` → binary IO.

Pick a variant:
- Need only read seed → `from_file`
- Want snapshot after each edit → `from_file_with_update`
- Want snapshot once after acceptance → `from_file_with_finalize`

Need text / JSON diff? Write your own loader (same function names) and use your own codec.

### Custom Loader Mini Example
```cpp
struct JsonCfgLoader {
  Config initialize(const std::string& tag) const { return read_json(path_for(tag)); }
  void update(const std::string& tag, const Config& v) const { write_json(path_for(tag), v); }
  void finalize(const std::string& tag, const Config& v) const { write_json(path_for(tag), v); }
};
```
Leave out update/finalize if not needed.

### Lifecycle (Typical)
```
initialize → (user edits → update?) → test assertions → finalize?
```

### When to Skip a Loader
- Value is trivial or auto-generated each run
- You do not need to store or inspect changes

### Quick Reference
- Direct value = implicit initialize only
- Loader object = add functions you want
- Tag passed only to per-test frames
- File helpers are optional sugar

# Pros & Cons

## Strengths
- Clear split: input / output / expect
- Only expose what you bind
- Clear hook points (init / update / finalize) you can choose to use
- Each test isolated (tag)
- Smooth golden file updates

## Cons
- Must declare upfront
- Loader steps to learn
- More files in repo
- Might add frames that do little
- Light tie to UI paths

## Tips
- Bigger gain with complex or changing data
- Keep paths simple
- Begin with direct values; add loaders later

## Good For
- Data heavy tests
- Visual / structured output
- Golden workflows

## Use Plain gtest For
- Tiny tests
- One-offs

Add frames only if simpler than ad‑hoc code.

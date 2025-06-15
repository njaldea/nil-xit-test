<script lang="ts">
    import ReactiveValueLoader from "@nil-/xit/test/ReactiveValueLoader.svelte";
    import { msgpack_codec } from "./codec.js";
    let { tag } = $props();
</script>

<ReactiveValueLoader frames={[
    { id: "circles_input_frame", tag, codec: msgpack_codec, key: "value", type: "json" },
    { id: "json_input_frame", tag, codec: msgpack_codec, key: "value", type: "json" }
]}>
    {#snippet loaded_data([ circles, slider ])}
        {JSON.stringify(circles)}
        {JSON.stringify(slider)}
    {/snippet}
</ReactiveValueLoader>
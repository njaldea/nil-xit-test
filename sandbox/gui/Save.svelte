<script lang="ts">
    import { type Writable } from "svelte/store";
    import { xit, codec_number } from "@nil-/xit";
    const { tag, signals, load_frame_data } = xit();
    const finalize = signals("finalize");

    let result = $state(null) as Writable<number> | null;
    load_frame_data("tag_info", tag)
        .then(f => {
            f.attach();
            result = f.values("value", 0, codec_number);
        });
</script>


{#if result != null}
    {`Result ${$result}`}
    <button onclick={() => finalize()}>Click to update</button>
{/if}
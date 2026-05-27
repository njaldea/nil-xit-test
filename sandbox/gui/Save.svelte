<script lang="ts">
    import { xit, codec_bool } from "@nil-/xit";
    const { tag, signals, load_frame_data } = xit();
    const finalize = signals("finalize");

    let result = $state(false);
    load_frame_data("tag_info", tag)
        .then(f => {
            f.values("value", result, codec_bool)
                .subscribe(v => {
                    console.log("updated", v);
                    result = v;
                });
        });
</script>


{`Result ${result}`}

<button onclick={() => finalize()}>Click to update</button>
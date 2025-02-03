<script>
    import { xit } from "@nil-/xit";
    import Split from "@nil-/xit/components/layouts/Split.svelte";
    import Scrollable from "@nil-/xit/components/layouts/Scrollable.svelte";
    import { get } from "svelte/store";

    import { msgpack_codec } from "./codec.js";

    const { values, signals, frame, frame_ui } = xit();

    const tags = values.json("tags", /** @type string[] */ ([]), msgpack_codec);

    const finalize = signals.string("finalize");

    let selected = $state(-1);
    let sorted_tags = $state($tags.sort());
    tags.subscribe(v => sorted_tags = v.sort());

    const load_frame = async (/** @type string */ tag, /** @type {"inputs" | "outputs"} */ key) => {
        const { values, unsub } = await frame("frame_info", tag);
        const v = values.json(key, /** @type string[] */ ([]), msgpack_codec);
        unsub();
        return Promise.all(
            get(v)
            .map(v => {
                const [frame_id, mark] = v.split(":");
                if (mark === "T") {
                    return frame_ui(frame_id, tag)
                } else if (mark === "U") {
                    return frame_ui(frame_id)
                }
            })
            .filter(v => v != null)
        );
    };
</script>

<svelte:head>
    <title>nil - xit {0 <= selected && selected < $tags.length ? `- ${$tags[selected]}` : ""}</title>
</svelte:head>

<svelte:window
    onkeydown={
        (event) => {
            if ((event.ctrlKey || event.metaKey) && event.key === 's') {
                if (0 <= selected && selected < $tags.length) {
                    finalize($tags[selected]);
                    event.preventDefault();
                }
            }
        }
    }
/>

{#snippet combo()}
    <div class="combo">
        <select bind:value={selected}>
            {#each sorted_tags as id, i}
            <option value={i}>{id}</option>
            {/each}
        </select>
    </div>
{/snippet}

{#snippet side(/** @type {"inputs" | "outputs"} */ key)}
    {#key selected}
        {#if 0 <= selected && selected < $tags.length}
            {#await load_frame($tags[selected], key) then actions}
                <Scrollable>
                    <div class="items">
                        {#each actions as action}
                            <div style:display="contents" use:action></div>
                        {/each}
                    </div>
                </Scrollable>
            {:catch e}
                <div>Error during loading... {((e) => { console.log(e); return 'e'; })(e)}</div>
            {/await}
        {/if}
    {/key}
{/snippet}

<Scrollable>
    <Split vertical offset={200}>
        {#snippet side_a()}
            {@render side("outputs")}
        {/snippet}
        {#snippet side_b()}
            {@render combo()}
            {@render side("inputs")}
        {/snippet}
    </Split>
</Scrollable>

<style>
    .items {
        height: 100%;
        display: flex;
        flex-direction: column;
    }

    .combo {
        padding-inline: 10px;
        padding-bottom: 4px;
    }

    .combo > select {
        width: 100%;
        padding: 0px;
        margin: 0px;
        box-sizing: border-box;
    }
</style>
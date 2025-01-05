<script>
    import { xit } from "@nil-/xit";
    import { get } from "svelte/store";

    import { msgpack_codec } from "./codec.js";

    const { values, signals, frame, frame_ui } = xit();

    const tags = values.json("tags", /** @type string[] */ ([]), msgpack_codec);

    const finalize = signals.string("finalize");

    let selected = $state(-1);
    let sorted_tags = $state($tags.sort());
    tags.subscribe(v => sorted_tags = v.sort());

    const load_frame = async (/** @type string */ tag) => {
        const { values, unsub } = await frame("frame_info", tag);
        const inputs = values.json("inputs", /** @type string[] */ ([]), msgpack_codec);
        const outputs = values.json("outputs", /** @type string[] */ ([]), msgpack_codec);
        unsub();
        return Promise.all([ 
            Promise.all(get(inputs).map(v => frame_ui(v, tag))), 
            Promise.all(get(outputs).map(v => frame_ui(v, tag)))
        ]);
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
<div class="root">
    <select bind:value={selected}>
        {#each sorted_tags as id, i}
            <option value={i}>{id}</option>
        {/each}
    </select>

    {#key selected}
        {#if 0 <= selected && selected < $tags.length}
            {#await load_frame($tags[selected])}
                <div>Loading...</div>
            {:then [ input_actions, output_actions ]}
                <div class="root-content">
                    {#if output_actions.length > 0}
                        {#await output_actions then a}
                            <div class="outputs" class:full={input_actions.length === 0}>
                                {#each a as action}
                                    <div style:display="contents" use:action></div>
                                {/each}
                            </div>
                        {/await}
                    {/if}
                    {#if input_actions.length > 0}
                        {#await input_actions then a}
                            <div class="inputs">
                                {#each a as action}
                                    <div style:display="contents" use:action></div>
                                {/each}
                            </div>
                        {/await}
                    {/if}
                </div>
            {:catch}
                <div>Error during loading...</div>
            {/await}
        {:else}
            <div>Nothing to load...</div>
        {/if}
    {/key}
</div>

<style>
    .root {
        height: 100%;
        display: flex;
        flex-direction: column;
    }

    .root-content {
        position: relative;
        height: 100%;
    }

    .outputs {
        width: calc(100% - 350px);
    }
    .outputs.full
    {
        width: 100%;
    }

    .inputs {
        position: absolute;
        left: calc(100% - 350px);
        right: 0;
        top: 0;
        bottom: 0;
        display: flex;
        flex-direction: column;
    }
</style>
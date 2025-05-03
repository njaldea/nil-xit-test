<script>
    import TestFrame from "@nil-/xit/test/Frame.svelte";
    import Split from "@nil-/xit/components/layouts/Split.svelte";
    import Scrollable from "@nil-/xit/components/layouts/Scrollable.svelte";

    import { msgpack_codec } from "./codec.js";

    let selected = $state(-1);
</script>

{#snippet combo(/** @type string[] */ tags)}
    <div class="combo">
        <select bind:value={selected}>
            {#each tags as id, i}
                <option value={i}>{id}</option>
            {/each}
        </select>
    </div>
{/snippet}

{#snippet side(/** @type import("@nil-/xit").Action<HTMLElement>[] | null */ actions)}
    <Scrollable>
        {#if actions != null}
            <div class="items">
                {#each actions as action}
                    <div style:display="contents" use:action></div>
                {/each}
            </div>
        {/if}
    </Scrollable>
{/snippet}

<TestFrame decoder={msgpack_codec.decode} {selected}>
    {#snippet load(tags, { inputs, outputs })}
        <Scrollable>
            <Split vertical offset={200}>
                {#snippet side_a()}
                    {@render side(outputs)}
                {/snippet}
                {#snippet side_b()}
                    {@render combo(tags)}
                    {@render side(inputs)}
                {/snippet}
            </Split>
        </Scrollable>
    {/snippet}
</TestFrame>

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
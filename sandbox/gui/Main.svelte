<script lang="ts">
    import TestFrame from "./npmjs/Frame.svelte";
    import Split from "@nil-/xit/components/layouts/Split.svelte";
    import Scrollable from "@nil-/xit/components/layouts/Scrollable.svelte";

    import type { Snippet } from "svelte";

    type Props = {
        children?: Snippet<[string]>,
        input?: Snippet<[{name: string; url: string;}]>,
        expect?: Snippet<[{name: string; url: string;}]>,
    };

    let { children, input, expect }: Props = $props();

    let selected = $state(-1);
</script>

<TestFrame bind:selected>
    {#snippet load({ tags, inputs, outputs, expects })}
        <Scrollable>
            <Split vertical offset={200}>
                {#snippet side_a()}
                    <Scrollable>
                        <div class="items">
                            {#each outputs as { action }}
                                <div style:display="contents" use:action></div>
                            {/each}
                        </div>
                    </Scrollable>
                {/snippet}
                {#snippet side_b()}
                    <div style="display: flex; flex-direction: column;">
                        <div class="combo">
                            <select bind:value={selected}>
                                {#each tags as id, i}
                                    <option value={i}>{id}</option>
                                {/each}
                            </select>
                        </div>
                        {#if 0 <= selected && selected < tags.length }
                            {@render children?.(tags[selected])}
                            {#each inputs as { name, url }}
                                {#if input != null}
                                    {@render input({ name, url: url(tags[selected]) })}
                                {:else}
                                    <a href={url(tags[selected])} target="_blank">{name}</a>
                                {/if}
                            {/each}
                            {#each expects as { name, url }}
                                {#if expect != null}
                                    {@render expect({ name, url: url(tags[selected]) })}
                                {:else}
                                    <a href={url(tags[selected])} target="_blank">{name}</a>
                                {/if}
                            {/each}
                        {/if}
                    </div>
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
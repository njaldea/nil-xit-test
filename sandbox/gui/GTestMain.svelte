<script lang="ts">
    import { derived, get } from "svelte/store";
    import Layout from "@nil-/doc/layout/Layout.svelte";
    import Split from "@nil-/doc/layout/Split.svelte";

    import { xit, codec_string, type Action } from "@nil-/xit";
    import { tick } from "svelte";

    type ActionItem = {
        name: string;
        action: Action<HTMLElement>;
        url: (tag: string) => string;
    };

    let { signals, values, load_frame_data, load_frame_ui } = xit();

    const finalize = signals("finalize", codec_string.encode);

    const last_tag = localStorage.getItem("nil_xit_last_tag");
    let current = $state(last_tag ? JSON.parse(last_tag) : null) as [string, string] | null;

    const tags_str = values("tags", "", codec_string);
    const tags = derived(tags_str, v => v.split(','));
    const regex = /^(.*?)\[(.*?)\]$/;
    const data = derived(tags, v => v.map(u => [`/${u.match(regex)?.slice(1).join('/')}`, u]));

    const frame_info = async (tag: string) => {
        const { values } = await load_frame_data("frame_info", tag);
        const load = async (key: string) => {
            const v = values(key, "", codec_string);
            const remap = (v: string) => {
                const [name, mark, g] = v.split(":");
                if (g !== "V") {
                    return;
                }

                if (mark === "T") {
                    return load_frame_ui(name, tag).then(action => {
                        return { name, action, url: (tag) => `/?frame=${name}&tag=${tag}` } satisfies ActionItem;
                    });
                }

                if (mark === "U") {
                    return load_frame_ui(name).then(action => {
                        return { name, action, url: (tag) => `/?frame=${name}` } satisfies ActionItem;
                    });
                }
            };
            return Promise.all(
                get(v)
                    .split(",")
                    .map(remap)
                    .filter((v) => v != null)
            );
        };

        return await Promise.all([load("inputs"), load("expects"), load("outputs")]);
    };

    type Frames = [inputs: ActionItem[], expects: ActionItem[], outputs: ActionItem[]];
    const empty_frames: Frames = [[], [], []];
    let frames = $state([...empty_frames]);

    let onnavigate = async (e: { detail?: string }) => {
        if (e.detail == null) {
            return;
        }

        current = null;
        current_frame = null;
        frames = [...empty_frames];

        const d = $data.find(v => v[0] === e.detail) as [string, string] | null;
        if (d == null)
        {
            return;
        }

        current = d;
    };

    $effect(() => {
        localStorage.setItem("nil_xit_last_tag", JSON.stringify(current));
        
        if(current != null) {
            const d = current;
            frame_info(current[1]).then(f => {
                if (current != null && current[0] === d[0] && current[1] == d[1]) {
                    frames = f;
                }
            });
        }
    });
    let current_frame = $state(null) as ActionItem | null;

    const on_frame_click = async (e: MouseEvent, info: ActionItem) => {
        if (current == null) {
            return;
        }

        if (e.ctrlKey) {
            window.open(info.url(current[1]), "_blank");
            return;
        }

        current_frame = null;
        await tick();
        current_frame = info;
    };
</script>

<Layout
    data={$data.map(v => v[0])}
    current={current?.[0]}
    theme={"dark"}
    offset={200}
    {onnavigate}
>
    {#snippet title()}
        <span>nil-xit-gtest</span>
    {/snippet}
    <Split vertical offset={200}>
        {#snippet side_a()}
            {#key current_frame}
                {#if current_frame != null}
                    <div style:display="contents" use:current_frame.action></div>
                {/if}
            {/key}
        {/snippet}
        {#snippet side_b()}
            {#if current != null}
                <div class="frame_panel">
                    <div hidden={frames[0].length === 0}>input frames</div>
                    {#each frames[0] as a_i}
                        <button onclick={(e) => on_frame_click(e, a_i)}>{a_i.name}</button>
                    {/each}
                    <div hidden={frames[1].length === 0}>expect frames</div>
                    {#each frames[1] as a_e}
                        <button onclick={(e) => on_frame_click(e, a_e)}>{a_e.name}</button>
                    {/each}
                    <div hidden={frames[2].length === 0}>output frames</div>
                    {#each frames[2] as a_o}
                        <button onclick={(e) => on_frame_click(e, a_o)}>{a_o.name}</button>
                    {/each}
                </div>
            {/if}
        {/snippet}
    </Split>
</Layout>

<svelte:window
    onkeydowncapture={(event) => {
        if ((event.ctrlKey || event.metaKey) && event.key === "s") {
            if (current != null) {
                finalize(current[1]);
                event.preventDefault();
            }
        }
    }}
/>

<style>
    .frame_panel {
        display: flex;
        flex-direction: column;
        padding: 10px;
        gap: 5px;
    }

    button {
        margin-left: 20px;
        min-width: 120px;
        overflow: hidden;
    }
</style>
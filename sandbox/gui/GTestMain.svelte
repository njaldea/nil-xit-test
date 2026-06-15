<script lang="ts">
    import { derived, get } from "svelte/store";
    import BareLayout from "@nil-/doc/layout/BareLayout.svelte";
    import Header from "@nil-/doc/layout/Header.svelte";
    import Nav from "@nil-/doc/navigation/Nav.svelte";
    import Split3 from "@nil-/doc/layout/Split3.svelte";
    import ThemeToggle from "@nil-/doc/layout/ThemeToggle.svelte";
    import Scrollable from "@nil-/doc/layout/Scrollable.svelte";

    import { xit, codec_string, type Action } from "@nil-/xit";
    import { tick, untrack } from "svelte";

    type ActionItem = {
        name: string;
        action: Action<HTMLElement>;
        url: (tag: string) => string;
    };

    let { signals, values, load_frame_data, load_frame_ui } = xit();

    const finalize = signals("finalize", codec_string.encode);

    let current = $state(localStorage.getItem("nil_xit_last_tag"));
    let current_frame = $state(null) as ActionItem | null;

    const tags = derived(values("tags", "", codec_string), v => v.split(','));
    const regex = /^([^.\[]+)\.([^\[\]]+)\[[^:\]]+:([^\]]+)\]$/;
    const parser = (v: string) => v.match(regex)?.slice(1, 4) ?? [];

    type Frames = [views: ActionItem[], inputs: ActionItem[], expects: ActionItem[], outputs: ActionItem[]];
    const empty_frames: Frames = [[], [], [], []];
    let frames = $state([...empty_frames]);

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

        return await Promise.all([load("views"), load("inputs"), load("expects"), load("outputs")]);
    };

    let onnavigate = async ({ detail }: { detail?: string }) => {
        if (detail == null) {
            return;
        }

        if (current !== detail) {
            current = null;
            current_frame = null;
            await tick();

            frames = [...empty_frames];
            current = detail;
        }
    };

    const on_frame_click = async (e: MouseEvent, info: ActionItem) => {
        if (current == null) {
            return;
        }

        if (e.ctrlKey) {
            window.open(info.url(current), "_blank");
            return;
        }

        current_frame = null;
        await tick();

        if (current_frame == null) {
            current_frame = info;
        }
    };

    $effect(() => {
        current;
        untrack(async () => {
            if(current != null) {
                localStorage.setItem("nil_xit_last_tag", current);
                const d = current;
                const f = await frame_info(current);
                if (current != null && current === d) {
                    frames = f;

                    if (frames[3].length > 0) {
                        current_frame = frames[3][0];
                    }
                }
            }
        });
    });

    let theme = $state("dark") as "dark" | "light";
</script>

<BareLayout dark={theme === "dark"}>
    {#snippet header()}
        <Header>
            {#snippet title()} 
                <span>nil-xit-gtest <b hidden={current == null}> - {current}</b> <b hidden={current_frame == null}> - {current_frame?.name}</b></span>
            {/snippet}
            {#snippet title_misc()}
                <ThemeToggle bind:theme />
            {/snippet}
        </Header>
    {/snippet}

    {#snippet body()}
        <Split3 side_a_width={250} side_c_width={250} side_b_min_width={400}>
            {#snippet side_a()}
                <Scrollable>
                    <Nav info={$tags} selected={current ?? ""} {parser} sorter={(l, r) => l.localeCompare(r) as -1 | 0 | 1} renamer={s => s} {onnavigate}>
                    </Nav>
                </Scrollable>
            {/snippet}

            {#snippet side_b()}
                {#key current_frame}
                    {#if current_frame != null}
                        <div style:display="contents" use:current_frame.action></div>
                    {/if}
                {/key}
            {/snippet}

            {#snippet side_c()}
                {#if current != null}
                    <div class="frame_panel">
                        <div hidden={frames[0].length === 0}>view frames</div>
                        {#each frames[0] as a_v}
                            <button onclick={(e) => on_frame_click(e, a_v)}>{a_v.name}</button>
                        {/each}
                        <div hidden={frames[1].length === 0}>input frames</div>
                        {#each frames[1] as a_i}
                            <button onclick={(e) => on_frame_click(e, a_i)}>{a_i.name}</button>
                        {/each}
                        <div hidden={frames[2].length === 0}>expect frames</div>
                        {#each frames[2] as a_e}
                            <button onclick={(e) => on_frame_click(e, a_e)}>{a_e.name}</button>
                        {/each}
                        <div hidden={frames[3].length === 0}>output frames</div>
                        {#each frames[3] as a_o}
                            <button onclick={(e) => on_frame_click(e, a_o)}>{a_o.name}</button>
                        {/each}
                    </div>
                {/if}
            {/snippet}
        </Split3>
    {/snippet}
</BareLayout>

<svelte:window
    onkeydowncapture={(event) => {
        if ((event.ctrlKey || event.metaKey) && event.key === "s") {
            if (current != null) {
                finalize(current);
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
<!-- Copyright (c) 2026, Neil Aldea <njaldea@gmail.com> -->
<!-- SPDX-License-Identifier: BSL-1.0 -->
<!-- See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt. -->

<script lang="ts" module>
    type ActionItem = {
        name: string;
        action: () => Promise<Action<HTMLElement>>;
        url: (tag: string) => string;
    };

    type Frames = {
        status: Frame | null;
        views: ActionItem[];
        inputs: ActionItem[];
        expects: ActionItem[];
        outputs: ActionItem[];
    };
    
    const empty_frames: Frames = {
        status: null,
        views: [],
        inputs: [],
        expects: [],
        outputs: []
    };

    const regex = /^([^.\[]+)\.([^\[\]]+)\[[^:\]]+:([^\]]+)\]$/;
    const parser = (v: string) => v.match(regex)?.slice(1, 4) ?? [];

    const nil_xit_last_tag = "nil_xit_last_tag";
    const nil_xit_last_frame = "nil_xit_last_frame";
    const store_to_local_storage = (data: string | null, tag: string) => {
        if (data != null) {
            localStorage.setItem(tag, data);
        } else {
            localStorage.removeItem(tag);
        }
    };

    const load_from_cache = <Key, Value>(key: Key, cache: Map<Key, Promise<Value>>, loader: () => Promise<Value>): Promise<Value> => {
        if (!cache.has(key)) cache.set(key, loader());
        return cache.get(key) as Promise<Value>;
    };

    const STATUS_MAP: Record<number, { style: string; label: string }> = {
        0: { style: "background:#fff5e8;border-color:#f1c27d;color:#7a4a00;", label: "RUNNING" },
        1: { style: "background:#e8f6ee;border-color:#8ad0a4;color:#0b5f2a;", label: "PASS" },
        2: { style: "background:#fdecef;border-color:#f1a1ad;color:#8a1020;", label: "FAIL" },
        3: { style: "background:#f4ecfb;border-color:#d0afea;color:#5b2b73;", label: "EXCEPTION" },
        4: { style: "background:#f2f2f2;border-color:#c9c9c9;color:#4a4a4a;", label: "UNKNOWN EXCEPTION" }
    };
    const DISABLED = { style: "background:#f2f2f2;border-color:#d0d0d0;color:#999;", label: "DISABLED" };

    const status_view = (active: boolean, value: number) =>
        !active ? DISABLED : (STATUS_MAP[value] ?? { style: "background:#f2f2f2;border-color:#c9c9c9;color:#4a4a4a;", label: `UNKNOWN (${value})` });
    const status_checkbox_style = (active: boolean) =>
        active ? "opacity:1;background:#81c784;border-color:#4caf50;color:#fff;" : "opacity:0.5;";
</script>

<script lang="ts">
    import { derived, get, fromStore } from "svelte/store";
    import BareLayout from "@nil-/doc/layout/BareLayout.svelte";
    import Header from "@nil-/doc/layout/Header.svelte";
    import Nav from "@nil-/doc/navigation/Nav.svelte";
    import Split3 from "@nil-/doc/layout/Split3.svelte";
    import ThemeToggle from "@nil-/doc/layout/ThemeToggle.svelte";
    import Scrollable from "@nil-/doc/layout/Scrollable.svelte";

    import { xit, codec_string, codec_number, type Action, type Frame } from "@nil-/xit";
    import { tick, untrack } from "svelte";


    let { signals, values, load_frame_data, load_frame_ui } = xit();

    const finalize = signals("finalize", codec_string.encode);

    let current = $state(localStorage.getItem("nil_xit_last_tag"));
    let last_frame = $state(localStorage.getItem("nil_xit_last_frame"));
    let current_frame = $state(null) as ActionItem | null;
    let status_active = $state(true);

    const tags = derived(values("tags", "", codec_string), (v) => v.split(","));

    let frames = $state<Frames>({ ...empty_frames });

    const cache_tag = new Map<string, Promise<Frames>>();
    const cache_frame = new Map<ActionItem, Promise<Action<HTMLElement>>>();

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
                    return {
                        name,
                        action: () => load_frame_ui(name, tag),
                        url: (tag) => `/?frame=${name}&tag=${tag}`
                    } satisfies ActionItem;
                }

                if (mark === "U") {
                    return {
                        name,
                        action: () => load_frame_ui(name),
                        url: (tag) => `/?frame=${name}`
                    } satisfies ActionItem;
                }
            };
            return Promise.all(
                get(v)
                    .split(",")
                    .map(remap)
                    .filter((v) => v != null)
            );
        };

        const frames = await Promise.all([
            load_frame_data("tag_info", tag),
            load("views"),
            load("inputs"),
            load("expects"),
            load("outputs")
        ]);
        return {
            status: frames[0],
            views: frames[1],
            inputs: frames[2],
            expects: frames[3],
            outputs: frames[4]
        };
    };

    const await_tag = (tag: string) => load_from_cache(tag, cache_tag, () => frame_info(tag));
    const await_action = (info: ActionItem) => load_from_cache(info, cache_frame, info.action);

    let onnavigate = async ({ detail }: { detail?: string }) => {
        if (detail == null) {
            return;
        }

        if (current !== detail) {
            current = null;
            current_frame = null;
            await tick();
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

        if (current_frame == info) {
            return;
        }

        current_frame = null;
        await tick();
        current_frame = info;
        last_frame = info.name;
    };

    $effect(() => store_to_local_storage(current, nil_xit_last_tag));
    $effect(() => store_to_local_storage(last_frame, nil_xit_last_frame));
    $effect(() => {
        current;
        untrack(async () => {
            if (current != null) {
                const d = current;
                const f = await await_tag(current);
                if (current != null && current === d) {
                    if (frames.status != null) {
                        frames.status.detach();
                    }

                    frames = { ...empty_frames };

                    await tick();

                    frames = f;

                    if (status_active && frames.status != null) {
                        frames.status.attach();
                    }

                    if (last_frame != null) {
                        const all_frames = [frames.views, frames.inputs, frames.expects, frames.outputs].flat();
                        const frame = all_frames.find(f => f.name === last_frame);
                        if (frame != null) {
                            current_frame = frame;
                            return;
                        }
                    }

                    if (frames.outputs.length > 0) {
                        current_frame = frames.outputs[0];
                        last_frame = current_frame.name;
                        return;
                    }

                    last_frame = null;
                }
            }
        });
    });

    let theme = $state("dark") as "dark" | "light";

    $effect(() => {
        status_active;
        untrack(() => {
            if (frames.status != null) {
                if (status_active) {
                    frames.status.attach();
                } else {
                    frames.status.detach();
                }
            }
        });
    });
</script>

{#snippet side_a()}
    <Scrollable>
        <Nav
            info={$tags}
            selected={current ?? ""}
            {parser}
            sorter={(l, r) => l.localeCompare(r) as -1 | 0 | 1}
            renamer={(s) => s}
            {onnavigate}
        ></Nav>
    </Scrollable>
{/snippet}

{#snippet side_b()}
    {#if current_frame != null}
        {#key current_frame}
            <svelte:boundary onerror={(e) => console.log(e)}>
                {#await await_action(current_frame)}
                    <div style="padding: 10px">
                        loading {current_frame.name}
                    </div>
                {:then action}
                    <div style:display="contents" use:action></div>
                {/await}

                {#snippet failed(error, reset)}
                    {@const err = error as Error}
                    <div style="padding: 10px">
                        <h3>{err.message}</h3>
                        <pre class="stack">{err.stack}</pre>
                    </div>
                {/snippet}
            </svelte:boundary>
        {/key}
    {/if}
{/snippet}

{#snippet frame(f: ActionItem[], s: string)}
    {#if f.length > 0}
        <div>{s} frames</div>
        {#each f as a_v}
            <button onclick={(e) => on_frame_click(e, a_v)}>{a_v.name}</button>
        {/each}
    {/if}
{/snippet}

{#snippet side_c()}
    {#key frames}
        <div class="frame_panel">
            {#if frames.status != null}
                {@const status = fromStore(frames.status.values("value", 1, codec_number))}
                {@const status_ui = status_view(status_active, status.current)}
                <div
                    class="status_toggle"
                    style={status_ui.style}
                    onclick={() => (status_active = !status_active)}
                    role="checkbox"
                    aria-checked={status_active}
                    tabindex="0"
                    onkeydown={(e) => {
                        if (e.key === "Enter" || e.key === " ") {
                            e.preventDefault();
                            status_active = !status_active;
                        }
                    }}
                >
                    <span class="checkbox" style={status_checkbox_style(status_active)}>{status_active ? "✓" : ""}</span>
                    <span class="label"><strong>{status_ui.label}</strong></span>
                </div>
            {/if}
            {@render frame(frames.views, "view")}
            {@render frame(frames.inputs, "input")}
            {@render frame(frames.expects, "expect")}
            {@render frame(frames.outputs, "output")}
        </div>
    {/key}
{/snippet}

<BareLayout {theme}>
    {#snippet header()}
        <Header>
            {#snippet title()}
                <span>nil-xit-gtest
                    {#if current != null}
                        {" | "}<b>{current}</b>
                    {/if}
                    {#if current_frame != null}
                        {" | "}<b>{current_frame?.name}</b>
                    {/if}
                </span>
            {/snippet}
            {#snippet title_misc()}
                <ThemeToggle bind:theme />
            {/snippet}
        </Header>
    {/snippet}

    {#snippet body()}
        <Split3
            side_a_width={250}
            side_c_width={250}
            side_b_min_width={400}
            {side_a}
            {side_b}
            {side_c}
        ></Split3>
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

    .status_toggle {
        display: flex;
        align-items: center;
        gap: 8px;
        padding: 6px 10px;
        border-radius: 6px;
        cursor: pointer;
        user-select: none;
        border: 1px solid #d0d0d0;
        background: #f2f2f2;
        color: #999;
        transition: all 0.2s ease;
    }

    .status_toggle:hover {
        background: #e8e8e8;
        border-color: #b0b0b0;
    }

    .checkbox {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        width: 16px;
        height: 16px;
        border: 2px solid #999;
        border-radius: 3px;
        background: #fff;
        transition: all 0.2s ease;
        font-size: 12px;
        font-weight: bold;
        line-height: 1;
    }

    .status_toggle .checkbox {
        opacity: 0.5;
    }

    .status_toggle .label {
        font-size: 13px;
        letter-spacing: 0.05em;
        font-weight: 500;
    }

    .status_toggle .label strong {
        font-weight: 600;
    }

    button {
        margin-left: 20px;
        min-width: 120px;
        overflow: hidden;
    }
</style>

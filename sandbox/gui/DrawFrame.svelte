<script>
    import { xit, json_string } from "@nil-/xit";

    const { values } = xit();

    /** @typedef {{ position: [number, number]; radius: number; }} Circle */
    const value_x = values.json("value-x", /** @type Circle */ ({ position: [0, 0], radius: 1.0 }), json_string);
    const value_y = values.json("value-y", /** @type Circle */ ({ position: [0, 0], radius: 1.0 }), json_string);
</script>

<div style="width=100%; height=100%;">
    <svg viewBox="-2 -2 4 4" xmlns="http://www.w3.org/2000/svg">
        {#each [
            {...$value_x, color: "blue"},
            {...$value_y, color: "red"}
        ] as { position: [cx, cy], radius: r, color: stroke }}
            <circle {cx} {cy} {r} {stroke} fill="none" stroke-width=0.1 />
        {/each}
    </svg>
</div>
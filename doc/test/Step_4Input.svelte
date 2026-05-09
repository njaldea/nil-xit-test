<script lang="ts">
    import { xit, codec_double } from "@nil-/xit";

    const { values } = xit();

    const x = values("x", 0, codec_double);
    const y = values("y", 0, codec_double);
    const r = values("radius", 0, codec_double);
</script>

<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$x}>x</label></div>
<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$y}>y</label></div>
<div><label><input type="range" min="0" max="10" step="0.1" bind:value={$r}>r</label></div>
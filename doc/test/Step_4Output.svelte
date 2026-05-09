<script lang="ts">
    import { xit, codec_double } from "@nil-/xit";

    const { values } = xit();

    const x = values("x", 0, codec_double);
    const y = values("y", 0, codec_double);
    const r = values("radius", 0, codec_double);
</script>

<div>x - {$x}</div>
<div>y - {$y}</div>
<div>r - {$r}</div>
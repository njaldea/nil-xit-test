<script>
    import { xit } from "@nil-/xit";
    const { values, signals } = xit();
    const v_1 = values.number("value-1", 0);
    const v_2 = values.number("value-2", 0);
    const v_3 = values.number("value-3", 0);

    const save = signals.none("save");
</script>

<input type="range" min={0} max={10} step={1} bind:value={$v_1}/>
<input type="range" min={0} max={10} step={1} bind:value={$v_2}/>
<input type="range" min={0} max={10} step={1} bind:value={$v_3}/>
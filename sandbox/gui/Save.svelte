<script lang="ts">
    import { onDestroy } from "svelte";
    import { xit } from "@nil-/xit";
    const { signals, unsub } = xit();
    const finalize = signals.none("finalize");
    onDestroy(unsub);
</script>

<button onclick={finalize}>Click to update</button>
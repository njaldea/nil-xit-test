<!-- Copyright (c) 2026, Neil Aldea <njaldea@gmail.com> -->
<!-- SPDX-License-Identifier: BSL-1.0 -->
<!-- See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt. -->

<script>
    import { xit, codec_number } from "@nil-/xit";

    const { values } = xit();

    const v_1 = values("value-1", 0, codec_number);
    const v_2 = values("value-2", 0, codec_number);
    const v_3 = values("value-3", 0, codec_number);
</script>

<input type="range" min={0} max={10} step={1} bind:value={$v_1}/>
<input type="range" min={0} max={10} step={1} bind:value={$v_2}/>
<input type="range" min={0} max={10} step={1} bind:value={$v_3}/>

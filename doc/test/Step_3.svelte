<!-- Copyright (c) 2026, Neil Aldea <njaldea@gmail.com> -->
<!-- SPDX-License-Identifier: BSL-1.0 -->
<!-- See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt. -->

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
<script>
    import { xit, json_string } from "@nil-/xit";

    const { values } = xit();

    /** @typedef {{ position: [number, number], radius: number }} Circle */

    const value_x = values.json("value-x", /** @type Circle */ ({}), json_string);
    const value_y = values.json("value-y", /** @type Circle */ ({}), json_string);
</script>

<div>
{#if $value_x != null && $value_y != null}
    <svg viewBox="-5 -5 10 10" xmlns="http://www.w3.org/2000/svg">
        <circle cx={$value_x.position[0]} cy={$value_x.position[1]} r={$value_x.radius} stroke="blue"/>
        <circle cx={$value_y.position[0]} cy={$value_y.position[1]} r={$value_y.radius} stroke="red"/>
    </svg>
{/if}
</div>

<style>
    div {
        width: 100%;
        height: 100%;
        display: grid;
    }
    circle {
        fill: none;
        stroke-width: 0.1;
    }
</style>
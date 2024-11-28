<script>
    import { xit, json_string } from "@nil-/xit";

    const { values } = xit();

    const value_x = values.json("value-x", { position: [0, 0], radius: 1.0 }, json_string);
    const value_y = values.json("value-y", { position: [0, 0], radius: 1.0 }, json_string);
</script>

<div style="width=100%; height=100%;">
    <svg viewBox="-5 -5 10 10" xmlns="http://www.w3.org/2000/svg">
        <circle
            cx={$value_x.position[0]}
            cy={$value_x.position[1]}
            r={$value_x.radius}
            fill="none"
            stroke-width=0.1
            stroke="blue"
        />
        <circle
            cx={$value_y.position[0]}
            cy={$value_y.position[1]}
            r={$value_y.radius}
            fill="none"
            stroke-width=0.1
            stroke="red"
        />
    </svg>
</div>
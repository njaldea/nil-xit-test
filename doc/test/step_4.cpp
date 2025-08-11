// Step 4: add an input frame and bind members
#include "Circle.hpp"
#include <nil/xit/gtest.hpp>

XIT_FRAME_MAIN("$group/Step_4Main.svelte");

XIT_FRAME_OUTPUT_V("output", "$group/Step_4Output.svelte", Circle)
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

XIT_FRAME_TEST_INPUT_V("input", "$group/Step_4Input.svelte", Circle(0.1, 0.1, 0.1))
    .value("x", &Circle::x)
    .value("y", &Circle::y)
    .value("radius", &Circle::radius);

struct Sample
{
    XIT_INPUTS("input");
    XIT_OUTPUTS("output");
};

XIT_TEST_F(Sample, circle, "$group/.")
{
    const auto& [sut] = xit_inputs;

    // do your test here

    get<0>(xit_outputs) = sut;
}

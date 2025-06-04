#include "userland_utils.hpp"

#include <filesystem>

#include <nil/xit/gtest.hpp>

using nil::xit::gtest::from_data;               // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file;               // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file_with_finalize; // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file_with_update;   // NOLINT(misc-unused-using-decls)

// TODO: move to command line arguments or env var?

XIT_FRAME_MAIN("$base/Main.svelte", nlohmann::json);
// also accepts callable types (nlohamnn::json is a class/struct)
// [](const std::vector<std::string>& v) { return nlohmann::json(v); }

XIT_FRAME_TAGGED_INPUT_V(
    "json_input_frame",
    "$base/InputFrame.svelte",
    from_file_with_finalize<nlohmann::json, "input_frame.json">()
)
    .value("value");

XIT_FRAME_UNIQUE_INPUT_V(
    "slider_frame",
    "$base/Slider.svelte",
    from_file_with_update<Ranges, "slider_frame">()
)
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);

XIT_FRAME_OUTPUT_V("plotly_frame", "$base/PlotlyFrame.svelte", nlohmann::json)
    .value("value-x", from_json_ptr("/x"))
    .value("value-y", from_json_ptr("/y"));

using nil::xit::gtest::Input;
using nil::xit::gtest::Output;
using nil::xit::gtest::Test;
using nil::xit::gtest::TestInputs;
using nil::xit::gtest::TestOutputs;

using Plotly = Test<Input<"slider_frame", "json_input_frame">, Output<"plotly_frame">>;

XIT_TEST_F(Plotly, demo, "plotly/*")
{
    const auto& [ranges, input_data] = xit_inputs;
    //           ┃           ┃         ┗━━━ from Input<"slider_frame", "input_frame">
    //           ┃           ┗━━━ type == nlohmann::json
    //           ┗━━━ type == Ranges

    auto& [view] = xit_outputs;
    //     ┃       ┗━━━ from Output<"plotly_frame">
    //     ┗━━━ type == nlohmann::json

    std::cout << "Running: " << input_data["x"][2] << std::endl;

    view = input_data;
    view["y"][0] = std::int64_t(input_data["y"][0]) * ranges.v1;
    view["y"][1] = std::int64_t(input_data["y"][1]) * ranges.v2;
    view["y"][2] = std::int64_t(input_data["y"][2]) * ranges.v3;
}

XIT_FRAME_TAGGED_INPUT_V(
    "circles_input_frame",
    "$base/InputFrame.svelte",
    from_file_with_finalize<Circles, "circles_frame.json">()
)
    .value("value");

XIT_FRAME_OUTPUT_V("draw_frame", "$base/DrawFrame.svelte", Circles)
    .value("value-x", &Circles::x)
    .value("value-y", &Circles::y);

using DrawWithInput = Test<Input<"circles_input_frame">, Output<"draw_frame">>;

XIT_TEST_F(DrawWithInput, demo, "draw")
{
    const auto& [input_data] = xit_inputs;
    //           ┃             ┗━━━ from Input<"input_frame">
    //           ┗━━━ type == Circles

    // do your test here...

    auto& [draw] = xit_outputs;
    //     ┃       ┗━━━ from Output<"draw_frame">
    //     ┗━━━ type == Circles

    // Assign data to output.
    // This example is overly simplified where the input and output are of the same type
    // and just copies them to the output directly.
    draw = input_data;
}

using Draw = TestOutputs<"draw_frame">;

XIT_TEST_F(Draw, sample)
{
    Circles circles;

    // Execute test here

    auto& [draw] = xit_outputs;
    //     ┃       ┗━━━ from Output<"draw_frame">
    //     ┗━━━ type == Circles
    draw = circles;
}

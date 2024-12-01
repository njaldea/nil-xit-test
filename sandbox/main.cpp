#include "userland_utils.hpp"

#include <filesystem>

// TODO: move to command line arguments or env var?
const auto source_path = std::filesystem::path(__FILE__).parent_path();
XIT_PATH_MAIN_UI_DIRECTORY(source_path / "gui");
XIT_PATH_UI_DIRECTORY(source_path / "gui");
XIT_PATH_TEST_DIRECTORY(source_path / "files");
XIT_PATH_SERVER_DIRECTORY(source_path / "node_modules/@nil-/xit");

XIT_FRAME_MAIN("Main.svelte", nlohmann::json);
// also accepts callable types (nlohamnn::json is a class/struct)
// [](const std::vector<std::string>& v) { return nlohmann::json(v); }

XIT_FRAME_TAGGED_INPUT(
    "json_input_frame",
    "InputFrame.svelte",
    from_file_with_finalize("input_frame.json", &to_json, &from_json)
)
    .value("value");

XIT_FRAME_UNIQUE_INPUT(
    "slider_frame",
    "Slider.svelte",
    from_file_with_update("slider_frame", &to_range, &from_range)
)
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);

XIT_FRAME_OUTPUT("plotly_frame", "PlotlyFrame.svelte", nlohmann::json)
    .value("value-x", from_json_ptr("/x"))
    .value("value-y", from_json_ptr("/y"));

using nil::xit::gtest::Input;
using nil::xit::gtest::Output;
using nil::xit::gtest::Test;

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

XIT_FRAME_TAGGED_INPUT("circles_input_frame", "InputFrame.svelte", Circles())
    // from_file_with_finalize("input_frame.json", &to_circles, &from_circles)
    .value("value");

XIT_FRAME_OUTPUT("draw_frame", "DrawFrame.svelte", Circles)
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

using Draw = nil::xit::gtest::Test<nil::xit::gtest::Input<>, nil::xit::gtest::Output<"draw_frame">>;

XIT_TEST_F(Draw, sample)
{
    Circles circles;

    // Execute test here

    auto& [draw] = xit_outputs;
    //     ┃       ┗━━━ from Output<"draw_frame">
    //     ┗━━━ type == Circles
    draw = circles;
}

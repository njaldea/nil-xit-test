#include "userland_utils.hpp"

#include <filesystem>

// TODO: move to command line arguments or env var?
const auto source_path = std::filesystem::path(__FILE__).parent_path();
XIT_PATH_MAIN_UI_DIRECTORY(source_path);
XIT_PATH_UI_DIRECTORY(source_path);
XIT_PATH_TEST_DIRECTORY(source_path);
XIT_PATH_SERVER_DIRECTORY(source_path / "node_modules/@nil-/xit");

XIT_FRAME_MAIN("gui/Main.svelte", nlohmann::json);
// also accepts callable types (nlohamnn::json is a class/struct)
// [](const std::vector<std::string>& v) { return nlohmann::json(v); }

XIT_FRAME_TAGGED_INPUT(
    "input_frame",
    "gui/InputFrame.svelte",
    from_file_with_finalize("files", "input_frame.json", &to_json, &from_json)
)
    .value("value");

// XIT_FRAME_UNIQUE_INPUT("slider_frame", "gui/Slider.svelte", Ranges(3, 2, 1))
// XIT_FRAME_UNIQUE_INPUT(
//     "slider_frame",
//     "gui/Slider.svelte",
//     from_file_rw("files", "slider_frame", &to_range, &from_range)
// )
XIT_FRAME_UNIQUE_INPUT(
    "slider_frame",
    "gui/Slider.svelte",
    from_file_with_update("files", "slider_frame", &to_range, &from_range)
)
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);

XIT_FRAME_OUTPUT("view_frame", "gui/ViewFrame.svelte", nlohmann::json)
    .value("value-x", from_json_ptr("/x"))
    .value("value-y", from_json_ptr("/y"));

using Sample = nil::xit::gtest::Test<
    nil::xit::gtest::Input<"slider_frame", "input_frame">,
    nil::xit::gtest::Output<"view_frame">>;

XIT_TEST(Sample, Demo, "files")
{
    const auto& [ranges, input_data] = xit_inputs;
    //           ┃           ┃         ┗━━━ from Input<"slider_frame", "input_frame">
    //           ┃           ┗━━━ type == nlohmann::json
    //           ┗━━━ type == Ranges

    auto& [view] = xit_outputs;
    //     ┃       ┗━━━ from Output<"view_frame">
    //     ┗━━━ type == nlohmann::json

    view = input_data;
    view["y"][0] = std::int64_t(input_data["y"][0]) * ranges.v1;
    view["y"][1] = std::int64_t(input_data["y"][1]) * ranges.v2;
    view["y"][2] = std::int64_t(input_data["y"][2]) * ranges.v3;
}

using PassThrough = nil::xit::gtest::
    Test<nil::xit::gtest::Input<"input_frame">, nil::xit::gtest::Output<"view_frame">>;

XIT_TEST(PassThrough, Demo, "files")
{
    const auto& [input_data] = xit_inputs;
    //           ┃             ┗━━━ from Input<"input_frame">
    //           ┗━━━ type == nlohmann::json

    auto& [view] = xit_outputs;
    //     ┃       ┗━━━ from Output<"view_frame">
    //     ┗━━━ type == nlohmann::json

    view = input_data;
}

// TODO:
//  -   how about runtime configured tests (scripting languages)

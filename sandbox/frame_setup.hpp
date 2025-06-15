#pragma once

#include "userland_utils.hpp"

#include <nil/xit/gtest.hpp>

#include <nlohmann/json.hpp>

using nil::xit::gtest::from_data;               // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file;               // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file_with_finalize; // NOLINT(misc-unused-using-decls)
using nil::xit::gtest::from_file_with_update;   // NOLINT(misc-unused-using-decls)

XIT_FRAME_MAIN("@nil-/xit/test/Main.svelte", nlohmann::json);

XIT_FRAME_TEST_INPUT_V(
    "json_input_frame",
    "$base/InputFrame.svelte",
    from_file_with_finalize<nlohmann::json, "input_frame.msgpack">()
)
    .value("value");

XIT_FRAME_GLOBAL_INPUT_V(
    "slider_frame",
    "$base/Slider.svelte",
    from_file_with_update<Ranges, "$test/slider_frame.msgpack">()
)
    .value("value-1", &Ranges::v1)
    .value("value-2", &Ranges::v2)
    .value("value-3", &Ranges::v3);

XIT_FRAME_OUTPUT_V("plotly_frame", "$base/PlotlyFrame.svelte", nlohmann::json)
    .value("value-x", from_json_ptr("/x"))
    .value("value-y", from_json_ptr("/y"));

XIT_FRAME_TEST_INPUT_V(
    "circles_input_frame",
    "$base/InputFrame.svelte",
    from_file_with_finalize<Circles, "circles_frame.msgpack">()
)
    .value("value");

XIT_FRAME_TEST_V("FF", "$base/InputFrame.svelte");

XIT_FRAME_OUTPUT_V("draw_frame", "$base/DrawFrame.svelte", Circles)
    .value("value-x", &Circles::x)
    .value("value-y", &Circles::y);

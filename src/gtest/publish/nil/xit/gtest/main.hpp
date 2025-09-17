#pragma once

namespace nil::xit::gtest
{
    /**
     * @brief Entry point for the test runner.
     *
     * @param argc Argument count.
     *   - If `argc > 0`, `argv[0]` is assumed to be the program name and is ignored by the parser.
     *   - Remaining elements `argv[1] .. argv[argc-1]` are parsed as options.
     *   - If `argc == 0`, no arguments are parsed (valid but unusual case).
     *
     * @param argv Argument vector.
     *   - Must contain at least `argc` valid C strings.
     *   - `argv[argc]` is expected to be a `nullptr` sentinel (as required by the C++ standard).
     *
     * @return int Exit code for the process.
     *
     * ## Command-line interface
     *
     * ### Default mode
     * Options:
     *   -h, --help                         Show this help.
     *   -l, --list                         List available tests.
     *   -v, --verbose                      Show additional information such as groups.
     *   -i, --ignore-missing-groups        Ignore missing groups.
     *   -g, --path-group <text>            Add group path.
     *
     * Subcommands:
     *   gui : Run in GUI mode (see below).
     *
     * ### GUI mode (`gui`)
     * Options:
     *   -h, --help                         Show this help.
     *   -l, --list                         List available tests.
     *   -v, --verbose                      Show additional information such as groups.
     *   -c, --clear                        Clear cache on boot.
     *   -i, --ignore-missing-groups        Ignore missing groups.
     *   --host <text> (=127.0.0.1)         Host to use.
     *   -p, --port <value> (=0)            Port to use.
     *   -j` [ --jobs ] [=value(=0)] (=1)   Number of jobs
     *   -g, --path-group <text>            Add group path.
     *   -a, --path-assets <text>           Asset path.
     */
    int main(int argc, const char* const* argv);
}

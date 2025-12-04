package(default_visibility = ["//visibility:public"])

load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

refresh_compile_commands(
    name = "refresh_compile_commands",
    # Specifying targets restricts the tool to ONLY your code.
    # This prevents it from trying to compile Abseil's internal header tests,
    # which solves the crash you are seeing.
    targets = {
        "//...": "--features=-parse_headers", 
    },
)
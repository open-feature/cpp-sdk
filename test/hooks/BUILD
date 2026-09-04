load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_test(
    name = "logging_hook_test",
    srcs = ["logging_hook_test.cpp"],
    deps = [
        "//openfeature:evaluation_context",
        "//openfeature:flag_evaluation_details",
        "//openfeature:flag_metadata",
        "//openfeature:flag_type_value",
        "//openfeature:hook_context",
        "//openfeature:hook_data",
        "//openfeature:hook_hints",
        "//openfeature:reason",
        "//openfeature:value",
        "//openfeature/exceptions:open_feature_exceptions",
        "//openfeature/hooks:log_level",
        "//openfeature/hooks:logging_hook",
        "@googletest//:gtest_main",
    ],
)

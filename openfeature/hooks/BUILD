load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "log_level",
    hdrs = ["log_level.h"],
    include_prefix = "openfeature/hooks",
)

cc_library(
    name = "logging_hook",
    srcs = ["logging_hook.cpp"],
    hdrs = ["logging_hook.h"],
    include_prefix = "openfeature/hooks",
    deps = [
        ":log_level",
        "//openfeature:error_code",
        "//openfeature:evaluation_context",
        "//openfeature/exceptions:open_feature_exceptions",
        "//openfeature:general_flag_evaluation_details",
        "//openfeature:general_hook",
        "//openfeature:general_hook_context",
        "//openfeature:hook_hints",
        "//openfeature:reason",
        "//openfeature:value",
    ],
)

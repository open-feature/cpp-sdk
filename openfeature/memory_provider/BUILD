load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "flag",
    hdrs = ["flag.h"],
    include_prefix = "openfeature",
    deps = [
        "//openfeature:evaluation_context",
        "//openfeature:flag_metadata",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "in_memory_provider",
    srcs = ["in_memory_provider.cpp"],
    hdrs = ["in_memory_provider.h"],
    include_prefix = "openfeature",
    deps = [
        "//openfeature:error_code",
        "//openfeature:evaluation_context",
        ":flag",
        "//openfeature:metadata",
        "//openfeature:provider",
        "//openfeature:provider_status",
        "//openfeature:reason",
        "//openfeature:resolution_details",
        "//openfeature:value",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)
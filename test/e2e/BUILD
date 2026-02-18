load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_gherkin//gherkin:defs.bzl", "gherkin_test")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "context_storing_provider",
    srcs = [
        "context_storing_provider.cpp",
    ],
    hdrs = [
        "context_storing_provider.h",
    ],
    include_prefix = "test/e2e",
    deps = [
        "//openfeature:evaluation_context",
        "//openfeature:provider",
        "//openfeature:resolution_details",
        "//openfeature:metadata",
        "//openfeature:flag_metadata",
        "//openfeature:reason",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "flag",
    hdrs = [
        "flag.h",
    ],
    include_prefix = "test/e2e",
    deps = [
        "//openfeature:client_api",
        "//openfeature:evaluation_context",
        "//openfeature:provider",
        "//openfeature:resolution_details",
        "//openfeature:flag_metadata",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "state",
    hdrs = [
        "state.h",
    ],
    include_prefix = "test/e2e",
    deps = [
        "//openfeature:client_api",
        "//openfeature:evaluation_context",
        "//openfeature:provider",
        "//openfeature:resolution_details",
        ":flag",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

gherkin_test(
    name = "openfeature_gherkin_e2e_tests",
    steps = "//test/e2e/steps:gherkin_step_definitions",
    deps = [
        "//:openfeature_gherkin_spec_features",
    ],
)

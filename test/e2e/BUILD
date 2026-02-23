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
        "//openfeature:flag_metadata",
        "//openfeature:metadata",
        "//openfeature:provider",
        "//openfeature:reason",
        "//openfeature:value",
        "//openfeature:resolution_details",
        "@cucumber-cpp//:cucumber-cpp",
        "@googletest//:gtest", 
    ],
)

cc_library(
    name = "flag_test",
    hdrs = [
        "flag_test.h",
    ],
    include_prefix = "test/e2e",
)

cc_library(
    name = "state",
    hdrs = [
        "state.h",
    ],
    include_prefix = "test/e2e",
    deps = [
        "//openfeature:client",
        "//openfeature:evaluation_context",
        "//openfeature:provider",
        "//openfeature:resolution_details",
        "//openfeature:value",
        ":flag_test",
    ],
)

gherkin_test(
    name = "openfeature_gherkin_e2e_tests",
    steps = "//test/e2e/steps:gherkin_step_definitions",
    deps = [
        "//:openfeature_gherkin_spec_features",
    ],
)

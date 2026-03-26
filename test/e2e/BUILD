load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

exports_files(["cwt-cucumber.BUILD"])

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

cc_test(
    name = "openfeature_gherkin_e2e_tests",
    data = ["@openfeature_spec//:gherkin_features"],
    args =["$(locations @openfeature_spec//:gherkin_features)"],
    deps = [
        "//test/e2e/steps:gherkin_step_definitions",
        "@cwt_cucumber//:cwt-cucumber", 
    ],
)

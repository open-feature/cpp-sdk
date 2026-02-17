load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

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
    testonly = True,
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
    testonly = True,
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
    testonly = True,
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

cc_test(
    name = "openfeature_gherkin_e2e_tests",
    deps = [
        "//test/e2e/steps:gherkin_step_definitions",
        ":context_storing_provider",
        ":flag",
        ":state",
        "@cucumber-cpp//:cucumber-cpp",
        "@googletest//:gtest_main", # Required for the Cucumber-Cpp test runner
    ],
    data = [
         "//:openfeature_gherkin_spec_features", # Reference the filegroup from the root package
    ],
)

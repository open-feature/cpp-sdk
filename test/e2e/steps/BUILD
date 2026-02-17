load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "gherkin_step_definitions",
    srcs = [
        "minimal_steps.cpp"
    ],
    testonly = True,
    deps = [
        "//openfeature:client_api",
        "//openfeature:openfeature_api",
        "//openfeature:evaluation_context",
        "//openfeature:metadata",
        "//openfeature:resolution_details",
        "//test/e2e:context_storing_provider", # Explicitly add granular dependencies
        "//test/e2e:flag",
        "//test/e2e:state",
        "@cucumber-cpp//:cucumber-cpp",
        "@googletest//:gtest",
    ],
    includes = ["external/cucumber-cpp"],
    visibility = ["//test/e2e:__subpackages__"],
)
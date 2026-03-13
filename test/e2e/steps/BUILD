load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gherkin_step_definitions",
    srcs = ["minimal_steps.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//openfeature:evaluation_context",
        "//openfeature:openfeature_api",
        "//openfeature/memory_provider:flag",
        "//openfeature/memory_provider:in_memory_provider",
        "//test/e2e:context_storing_provider",
        "//test/e2e:state",
        "//test:mock_feature_provider",
        "@cwt_cucumber//:cwt-cucumber",
        "@googletest//:gtest",
    ],
    visibility = ["//test/e2e:__subpackages__"],
)
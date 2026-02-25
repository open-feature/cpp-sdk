load("@rules_gherkin//gherkin:defs.bzl", "cc_gherkin_steps")

package(default_visibility = ["//visibility:public"])

cc_gherkin_steps(
    name = "gherkin_step_definitions",
    srcs = ["minimal_steps.cpp"],
    deps = [
        "//openfeature:evaluation_context",
        "//openfeature:openfeature_api",
        "//openfeature/memory_provider:flag",
        "//openfeature/memory_provider:in_memory_provider",
        "//test/e2e:context_storing_provider",
        "//test/e2e:state",
        "//test:mock_feature_provider",
        "@cucumber-cpp//:cucumber-cpp",
        "@googletest//:gtest",
        "@googletest//:gmock",  
    ],
    visibility = ["//test/e2e:__subpackages__"],
)
load("@rules_gherkin//gherkin:defs.bzl", "cc_gherkin_steps")

package(default_visibility = ["//visibility:public"])

cc_gherkin_steps(
    name = "gherkin_step_definitions",
    srcs = ["minimal_steps.cpp"],
    deps = [
        "//openfeature:client_api",
        "//openfeature:openfeature_api",
        "//openfeature:evaluation_context",
        "//openfeature:metadata",
        "//openfeature:resolution_details",
        "//test/e2e:context_storing_provider", 
        "//test/e2e:flag",
        "//test/e2e:state",
        "@cucumber-cpp//:cucumber-cpp",
        "@googletest//:gtest", 
    ],
    visibility = ["//test/e2e:__subpackages__"],
)
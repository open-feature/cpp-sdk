load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_test(
    name = "flag_test",
    srcs = ["flag_test.cpp"],
    deps = [
        "//openfeature/memory_provider:flag",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "in_memory_provider_test",
    srcs = ["in_memory_provider_test.cpp"],
    deps = [
        "//openfeature/memory_provider:in_memory_provider",
        "@googletest//:gtest_main",
    ],
)
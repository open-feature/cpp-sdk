cc_test(
    name = "resolution_details_test",
    srcs = ["resolution_details_test.cpp"],
    deps = [
        "//src:resolution_details",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_feature_provider",
    testonly = True,
    hdrs = ["mocks/mock_feature_provider.h"],
    visibility = ["//test:__pkg__"],
    deps = [
        "//include:openfeature_headers",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "feature_provider_status_manager_test",
    srcs = ["feature_provider_status_manager_test.cpp"],
    deps = [
        ":mock_feature_provider",
        "//src:feature_provider_status_manager",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "noop_provider_test",
    # Path is relative to cpp-sdk/src/test/
    srcs = ["noop_provider_test.cpp"],
    deps = [
        # We reference the library defined in the parent 'src' package
        "//src:noop_provider",
        # Google Test dependency remains the same
        "@googletest//:gtest_main",
    ],
)

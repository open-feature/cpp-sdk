cc_library(
    name = "mock_feature_provider",
    testonly = True,
    hdrs = ["mocks/mock_feature_provider.h"],
    visibility = ["//test:__pkg__"],
    deps = [
        "//openfeature:provider",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "feature_provider_status_manager_test",
    srcs = ["feature_provider_status_manager_test.cpp"],
    deps = [
        ":mock_feature_provider",
        "//openfeature:feature_provider_status_manager",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "resolution_details_test",
    srcs = ["resolution_details_test.cpp"],
    deps = [
        "//openfeature:resolution_details",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "noop_provider_test",
    srcs = ["noop_provider_test.cpp"],
    deps = [
        "//openfeature:noop_provider",
        "@googletest//:gtest_main",
    ],
)

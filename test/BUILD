cc_test(
    name = "resolution_details_test",
    # Path is relative to cpp-sdk/src/test/
    srcs = ["resolution_details_test.cpp"],
    deps = [
        # We reference the library defined in the parent 'src' package
        "//src:resolution_details",
        # Google Test dependency remains the same
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

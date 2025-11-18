cc_library(
    name = "openfeature_cpp_sdk",

    # TODO: Remove recursive glob and replace with individual Bazel packages
    # Source files from the src/ directory
    srcs = glob(
        ["src/**/*.cpp"],
        exclude = ["src/test/**/*.cpp"],
        allow_empty = True,
    ),

    # Header files from the include/ directory
    hdrs = glob(["include/**/*.h"]),

    # This tells Bazel to add the 'include' directory to the
    # include search path for any target that depends on this one.
    # So, users can do #include "my_header.h" instead of #include "include/my_header.h"
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "resolution_details_test",
    srcs = ["src/test/resolution_details_test.cpp"],
    deps = [
        # This dependency links the test code against your main library.
        "//:openfeature_cpp_sdk",
        
        # Add the dependency for the Google Test framework:
        "@googletest//:gtest_main",
    ],
)
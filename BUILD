cc_library(
    name = "openfeature_cpp_sdk",

    # TODO: Remove recursive glob and replace with individual Bazel packages
    # Source files from the src/ directory
    srcs = glob(
        ["src/**/*.cpp"],
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

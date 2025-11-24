package(default_visibility = ["//visibility:public"])

# Headers target
cc_library(
    name = "openfeature_headers",
    hdrs = glob(["include/**/*.h"]),
    # Add 'include' directory to the compiler search path
    # for any target that depends on this.
    includes = ["include"], 
)

cc_library(
    name = "openfeature_cpp_sdk",
    deps = ["//src:openfeature_src_bundle"],
)
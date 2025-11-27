package(default_visibility = ["//visibility:public"])

# Headers target
cc_library(
    name = "openfeature_headers",
    hdrs = glob(["**/*.h"]),
    # Adds 'cpp-sdk/include' to the include path
    includes = ["."], 
    deps = [
        "@abseil-cpp//absl/base:core_headers",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/synchronization",
    ],
)

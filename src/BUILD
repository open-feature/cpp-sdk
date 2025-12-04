package(default_visibility = ["//visibility:public"])

cc_library(
    name = "resolution_details",
    srcs = ["resolution_details.cpp"],
    deps = [
        "//include:openfeature_headers", 
    ],
)

cc_library(
    name = "noop_provider",
    srcs = ["noop_provider.cpp"],
    deps = [
        ":resolution_details",
        "//include:openfeature_headers",
    ],
)

cc_library(
    name = "feature_provider_status_manager",
    srcs = ["feature_provider_status_manager.cpp"],
    deps = [
        "//include:openfeature_headers",
        "@abseil-cpp//absl/log:log",
        "@abseil-cpp//absl/memory",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/synchronization",
    ],
)

# Main SDK library
cc_library(
    name = "openfeature_cpp_sdk",
    deps = [
        ":resolution_details",
        ":noop_provider",
        ":feature_provider_status_manager",
    ],
)


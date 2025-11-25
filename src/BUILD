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
        ":interfaces",
        "//include:openfeature_headers",
    ],
)
# Main SDK library
cc_library(
    name = "openfeature_cpp_sdk",
    deps = [
        ":interfaces",
        ":noop_provider",
    ],
)

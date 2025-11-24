package(default_visibility = ["//visibility:public"])

# Interfaces
cc_library(
    name = "interfaces",
    srcs = ["resolution_details.cpp"],
    deps = [
        "//:openfeature_headers", 
    ],
)

# No-op Provider
cc_library(
    name = "noop_provider",
    srcs = ["noop_provider.cpp"],
    deps = [
        ":interfaces",
        "//:openfeature_headers",
    ],
)

# Bundle
cc_library(
    name = "openfeature_src_bundle",
    deps = [
        ":interfaces",
        ":noop_provider",
    ],
)
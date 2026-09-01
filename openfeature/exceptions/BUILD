load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "open_feature_exceptions",
    hdrs = ["open_feature_exceptions.h"],
    include_prefix = "openfeature/exceptions",
    deps = [
        "//openfeature:error_code"
    ],
)

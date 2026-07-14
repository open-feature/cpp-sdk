load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "base_hook",
    hdrs = ["base_hook.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "client",
    hdrs = ["client.h"],
    include_prefix = "openfeature",
    deps = [
        ":evaluation_context",
        ":features",
        ":metadata",
        ":provider_status",
    ],
)

cc_library(
    name = "client_api",
    srcs = ["client_api.cpp"],
    hdrs = ["client_api.h"],
    include_prefix = "openfeature",
    deps = [
        ":client",
        ":evaluation_context",
        ":features",
        ":flag_metadata",
        ":global_context_manager",
        ":metadata",
        ":provider",
        ":provider_repository",
        ":provider_status",
        ":reason",
        ":resolution_details",
        ":value",
    ],
)

cc_library(
    name = "error_code",
    hdrs = ["error_code.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "evaluation_context",
    hdrs = ["evaluation_context.h"],
    srcs = ["evaluation_context.cpp"],
    include_prefix = "openfeature",
)

cc_library(
    name = "feature_provider_status_manager",
    srcs = ["feature_provider_status_manager.cpp"],
    hdrs = ["feature_provider_status_manager.h"],
    include_prefix = "openfeature",
    deps = [
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        ":evaluation_context",
        ":provider",
        ":provider_status",
    ],
)

cc_library(
    name = "features",
    hdrs = ["features.h"],
    include_prefix = "openfeature",
    deps = [
        ":evaluation_context",
        ":value"
    ],
)

cc_library(
    name = "flag_evaluation_details",
    srcs = ["flag_evaluation_details.cpp"],
    hdrs = ["flag_evaluation_details.h"],
    include_prefix = "openfeature",
    deps = [
        ":error_code",
        ":flag_metadata",
        ":reason",
        ":resolution_details",
        ":value"
    ],
)

cc_library(
    name = "flag_metadata",
    hdrs = ["flag_metadata.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "flag_type_value",
    hdrs = ["flag_type_value.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "global_context_manager",
    srcs = ["global_context_manager.cpp"],
    hdrs = ["global_context_manager.h"],
    include_prefix = "openfeature",
    deps = [
        ":evaluation_context",
    ],
)

cc_library(
    name = "hook_context",
    srcs = ["hook_context.cpp"],
    hdrs = ["hook_context.h"],
    include_prefix = "openfeature",
    deps = [
        ":evaluation_context",
        ":flag_metadata",
        ":flag_type_value",
        ":hook_data",
        ":metadata",
        ":value",
    ],
)

cc_library(
    name = "hook_data",
    srcs = ["hook_data.cpp"],
    hdrs = ["hook_data.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "hook_hints",
    hdrs = ["hook_hints.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "hook",
    srcs = ["hook.cpp"],
    hdrs = ["hook.h"],
    include_prefix = "openfeature",
    deps = [
        ":base_hook",
        ":evaluation_context",
        ":flag_evaluation_details",
        ":hook_context",
        ":hook_hints",
        ":value",
    ],
)

cc_library(
    name = "metadata",
    hdrs = ["metadata.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "openfeature_api",
    srcs = ["openfeature_api.cpp"],
    hdrs = ["openfeature_api.h"],
    include_prefix = "openfeature",
    deps = [
        ":client",
        ":client_api",
        ":evaluation_context",
        ":global_context_manager",
        ":metadata",
        ":openfeature",
        ":provider",
        ":provider_repository",
    ],
)

cc_library(
    name = "noop_provider",
    srcs = ["noop_provider.cpp"],
    hdrs = ["noop_provider.h"],
    include_prefix = "openfeature",
    deps = [
        ":evaluation_context",
        ":metadata",
        ":provider",
        ":resolution_details",
        ":value",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "openfeature",
    hdrs = ["openfeature.h"],
    include_prefix = "openfeature",
    deps = [
        ":client",
        ":evaluation_context",
        ":metadata",
        ":provider",
    ],
)

cc_library(
    name = "provider_repository",
    srcs = ["provider_repository.cpp"],
    hdrs = ["provider_repository.h"],
    deps = [
         "@abseil-cpp//absl/status",
        ":evaluation_context",
        ":feature_provider_status_manager",
        ":provider",
        ":provider_status",
        ":noop_provider",
    ],
)

cc_library(
    name = "provider_status",
    hdrs = ["provider_status.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "provider",
    hdrs = ["provider.h"],
    include_prefix = "openfeature",
    deps = [
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        ":evaluation_context",
        ":metadata",
        ":resolution_details",
        ":value",
    ],
)

cc_library(
    name = "reason",
    hdrs = ["reason.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "resolution_details",
    srcs = ["resolution_details.cpp"],
    hdrs = ["resolution_details.h"],
    include_prefix = "openfeature",
    deps = [
        ":error_code",
        ":flag_metadata",
        ":reason",
        ":value",
    ],
)

cc_library(
    name = "value",
    srcs = ["value.cpp"],
    hdrs = ["value.h"],
    include_prefix = "openfeature",
    deps = [
        ":error_code",
        ":flag_metadata",
        ":reason",
    ],
)

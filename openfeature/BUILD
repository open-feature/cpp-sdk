package(
    default_visibility = ["//visibility:public"],
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
        ":metadata",
        ":openfeature_api",
        ":provider",
        ":provider_status",
        ":reason",
        ":resolution_details",
    ],
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
    name = "error_code",
    hdrs = ["error_code.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "evaluation_context",
    hdrs = ["evaluation_context.h"],
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
    ],
)

cc_library(
    name = "flag_metadata",
    hdrs = ["flag_metadata.h"],
    include_prefix = "openfeature",
)

cc_library(
    name = "metadata",
    hdrs = ["metadata.h"],
    include_prefix = "openfeature",
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
    ],
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
        ":metadata",
        ":openfeature",
        ":provider",
        ":provider_repository",
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
        ":evaluation_context",
        ":metadata",
        ":resolution_details",
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
    ],
)

load("@rules_cc//cc:defs.bzl", "cc_library")

# Generate the version file from the template by extracting the version from CMakeLists.txt
genrule(
    name = "generate_version_file",
    srcs = [
        "CMakeLists.txt",
        "src/version.template",
    ],
    outs = ["src/version.hpp"],
    cmd = """
        VERSION=$$(grep 'project(cwt-cucumber VERSION' $(location CMakeLists.txt) | sed 's/.*VERSION \\([0-9.]*\\).*/\\1/');
        sed "s/@PROJECT_VERSION@/$$VERSION/g" $(location src/version.template) > $@
    """,
)

cc_library(
    name = "cwt-cucumber",
    srcs = glob(
        ["src/**/*.cpp"],
        exclude = ["src/main.cpp"],
    ),
    hdrs = glob([
        "src/**/*.hpp",
    ]) + [
        "src/version.hpp",
    ],
    copts = ["-std=c++20"],
    strip_include_prefix = "src",
    visibility = ["//visibility:public"],
)

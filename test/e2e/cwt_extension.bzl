load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _cwt_ext_impl(ctx):
    version = "2.8"
    http_archive(
        name = "cwt_cucumber",
        sha256 = "30576a39a9ce2c4a915ed8f0e46f3f0ef149febf995989dfb02a05866ff38f57",
#Use the latest stable release(e.g., v2.8)
        urls = ["https://github.com/ThoSe1990/cwt-cucumber/archive/refs/tags/" + version + ".tar.gz"],
        strip_prefix = "cwt-cucumber-" + version,
#Inject a BUILD file to compile it on the fly
        build_file_content = """
load("@rules_cc//cc:defs.bzl", "cc_library")

#Generate the version file from the template
genrule(
    name = "generate_version_file",
    srcs =["src/version.template"],
    outs = ["src/version.hpp"],
    cmd = "sed 's/@PROJECT_VERSION@/{version}/g' $< > $@",
)

cc_library(
    name = "cwt-cucumber",
    srcs = glob(
        ["src/**/*.cpp"],
        exclude =["src/main.cpp"]
    ),
    hdrs = glob([
        "src/**/*.hpp",
    ]) +[
        "src/version.hpp",
    ],
    strip_include_prefix = "src",
    copts =["-std=c++20"],
    visibility = ["//visibility:public"],
)
""".replace("{version}", version),
    )

cwt_ext = module_extension(implementation = _cwt_ext_impl)
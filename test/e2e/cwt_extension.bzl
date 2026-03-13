load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _cwt_ext_impl(ctx):
    http_archive(
        name = "cwt_cucumber",
#Use the latest stable release(e.g., v2.8)
        urls =["https://github.com/ThoSe1990/cwt-cucumber/archive/refs/tags/2.8.tar.gz"],
        strip_prefix = "cwt-cucumber-2.8",
#Inject a BUILD file to compile it on the fly
        build_file_content = """
load("@rules_cc//cc:defs.bzl", "cc_library")

#Generate the version file from the template
genrule(
    name = "generate_version_file",
    srcs =["src/version.template"],
    outs = ["src/version.hpp"],
    cmd = "sed 's/@PROJECT_VERSION@/2.8/g' $< > $@",
)

cc_library(
    name = "cwt-cucumber",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob([
        "src/**/*.hpp",
    ]) +[
        "src/version.hpp",
    ],
    strip_include_prefix = "src",
    copts =["-std=c++20"],
    visibility = ["//visibility:public"],
)
""",
    )

cwt_ext = module_extension(implementation = _cwt_ext_impl)
cc_library(
    name = "tag_invoke",
    srcs = ["tag_invoke.h"],
)

cc_binary(
    name = "tag_invoke_demo",
    srcs = ["tag_invoke_demo.cpp"],
    copts = [
        "-fno-exceptions",
        "--std=c++20",
    ],
    deps = [
        ":tag_invoke",
    ],
)

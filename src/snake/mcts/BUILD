load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
#load("@com_google_protobuf//:protobuf.bzl", "cc_proto_library")
#load("@org_tensorflow//tensorflow:tensorflow.bzl", "tf_copts", "tf_cc_binary", "tf_cc_test")

cc_library(
    name = "mcts",
    hdrs = ["mcts.h"],
    copts =  [
        "-std=c++20",
    ],
    # copts = tf_copts() + [
    #     "-Werror",
    #     "-Wextra",
    #     "-Wall",
    # ],
    deps = [
        "@org_tensorflow//tensorflow/core/platform:logging",
    ],
)

cc_test(
    name = "mcts_test",
    srcs = ["mcts_test.cc"],
    copts =  [
        "-std=c++20",
    ],
    deps = [
        ":mcts",
        "@com_google_googletest//:gtest_main",
    ],
)

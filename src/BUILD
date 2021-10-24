cc_binary(
    name = "resnet_client_cc",
    srcs = [
        "resnet_client.cc",
    ],
    copts = [
        "-std=c++20",
        "-fcoroutines",
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_protobuf//:protobuf_lite",
        "@org_tensorflow//tensorflow/core:framework",
        "@org_tensorflow//tensorflow/core:lib",
        "@tf_serving//tensorflow_serving/apis:prediction_service_cc_proto",
    ],
)

cc_binary(
    name = "coro",
    srcs = ["coro.cpp"],
    copts = [
        "-fcoroutines",
        #"-fno-exceptions",
        "--std=c++20",
    ],
    deps = [
        "//src/cppcoro:generator",
        "@boost//:coroutine2",
        "@com_github_google_benchmark//:benchmark_main",
        "@org_tensorflow//tensorflow/core/platform:logging",
    ],
)

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

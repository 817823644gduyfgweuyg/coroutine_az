
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

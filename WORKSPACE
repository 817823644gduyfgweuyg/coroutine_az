workspace(name = "test")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//:repo.bzl", "tensorflow_serving_http_archive")

tensorflow_serving_http_archive(
    name = "tf_serving",
    git_commit = "04d47f8aa567f429185a4416ef58f7fe11a21a43",
    sha256 = "5fe87c899e2afea297dead1ee0e1709852ed55578b7975cffefe135f82b61c77",
)

# ===== TensorFlow dependency =====
#
# TensorFlow is imported here instead of in tf_serving_workspace() because
# existing automation scripts that bump the TF commit hash expect it here.
#
# To update TensorFlow to a new revision.
# 1. Update the 'git_commit' args below to include the new git hash.
# 2. Get the sha256 hash of the archive with a command such as...
#    curl -L https://github.com/tensorflow/tensorflow/archive/<git hash>.tar.gz | sha256sum
#    and update the 'sha256' arg with the result.
# 3. Request the new archive to be mirrored on mirror.bazel.build for more
#    reliable downloads.
load("@tf_serving//tensorflow_serving:repo.bzl", "tensorflow_http_archive")

tensorflow_http_archive(
    name = "org_tensorflow",
    git_commit = "919f693420e35d00c8d0a42100837ae3718f7927",
    sha256 = "70a865814b9d773024126a6ce6fea68fefe907b7ae6f9ac7e656613de93abf87",
)

# # Import all of TensorFlow Serving's external dependencies.
# # Downstream projects (projects importing TensorFlow Serving) need to
# # duplicate all code below in their WORKSPACE file in order to also initialize
# # those external dependencies.
# load("@tf_serving//tensorflow_serving:workspace.bzl", "tf_serving_workspace")

http_archive(
    name = "rules_pkg",
    sha256 = "352c090cc3d3f9a6b4e676cf42a6047c16824959b438895a76c2989c6d7c246a",
    url = "https://github.com/bazelbuild/rules_pkg/releases/download/0.2.5/rules_pkg-0.2.5.tar.gz",
)

# ===== RapidJSON (rapidjson.org) dependency =====
http_archive(
    name = "com_github_tencent_rapidjson",
    build_file = "@tf_serving//third_party/rapidjson:BUILD",
    sha256 = "8e00c38829d6785a2dfb951bb87c6974fa07dfe488aa5b25deec4b8bc0f6a3ab",
    strip_prefix = "rapidjson-1.1.0",
    url = "https://github.com/Tencent/rapidjson/archive/v1.1.0.zip",
)

# ===== libevent (libevent.org) dependency =====
http_archive(
    name = "com_github_libevent_libevent",
    build_file = "@tf_serving//third_party/libevent:BUILD",
    sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
    strip_prefix = "libevent-release-2.1.8-stable",
    url = "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip",
)

# ===== ICU dependency =====
# Note: This overrides the dependency from TensorFlow with a version
# that contains all data.
http_archive(
    name = "icu",
    build_file = "@tf_serving//third_party/icu:BUILD",
    patch_args = [
        "-p1",
        "-s",
    ],
    patches = ["@tf_serving//third_party/icu:data.patch"],
    sha256 = "dfc62618aa4bd3ca14a3df548cd65fe393155edd213e49c39f3a30ccd618fc27",
    strip_prefix = "icu-release-64-2",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/unicode-org/icu/archive/release-64-2.zip",
        "https://github.com/unicode-org/icu/archive/release-64-2.zip",
    ],
)

# ===== TF.Text dependencies
# NOTE: Before updating this version, you must update the test model
# and double check all custom ops have a test:
# https://github.com/tensorflow/text/blob/master/oss_scripts/model_server/save_models.py
http_archive(
    name = "org_tensorflow_text",
    patch_args = ["-p1"],
    patches = ["@tf_serving//third_party/tf_text:tftext.patch"],
    repo_mapping = {"@com_google_re2": "@com_googlesource_code_re2"},
    sha256 = "d82856dc04c04bbce347f72d0ad7df59bb6b26b7030f96f25f036cfe8a138312",
    strip_prefix = "text-2.5.0",
    url = "https://github.com/tensorflow/text/archive/v2.5.0.zip",
)

http_archive(
    name = "com_google_sentencepiece",
    sha256 = "c05901f30a1d0ed64cbcf40eba08e48894e1b0e985777217b7c9036cac631346",
    strip_prefix = "sentencepiece-1.0.0",
    url = "https://github.com/google/sentencepiece/archive/1.0.0.zip",
)

http_archive(
    name = "com_google_glog",
    sha256 = "1ee310e5d0a19b9d584a855000434bb724aa744745d5b8ab1855c85bff8a8e21",
    strip_prefix = "glog-028d37889a1e80e8a07da1b8945ac706259e5fd8",
    urls = [
        "https://mirror.bazel.build/github.com/google/glog/archive/028d37889a1e80e8a07da1b8945ac706259e5fd8.tar.gz",
        "https://github.com/google/glog/archive/028d37889a1e80e8a07da1b8945ac706259e5fd8.tar.gz",
    ],
)

#tf_serving_workspace()

# # Check bazel version requirement, which is stricter than TensorFlow's.
load(
    "@org_tensorflow//tensorflow:version_check.bzl",
    "check_bazel_version_at_least",
)

check_bazel_version_at_least("3.7.2")

# # Initialize TensorFlow's external dependencies.
load("@org_tensorflow//tensorflow:workspace3.bzl", "workspace")

workspace()

load("@org_tensorflow//tensorflow:workspace2.bzl", "workspace")

workspace()

load("@org_tensorflow//tensorflow:workspace1.bzl", "workspace")

workspace()

load("@org_tensorflow//tensorflow:workspace0.bzl", "workspace")

workspace()

# # Initialize bazel package rules' external dependencies.
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()

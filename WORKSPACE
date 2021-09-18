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
load("@tf_serving//tensorflow_serving:workspace.bzl", "tf_serving_workspace")

tf_serving_workspace()

# # Check bazel version requirement, which is stricter than TensorFlow's.
load(
    "@org_tensorflow//tensorflow:version_check.bzl",
    "check_bazel_version_at_least",
)

check_bazel_version_at_least("3.7.2")


# load("@org_tensorflow//third_party:repo.bzl", "tf_http_archive")

# http_archive(
#     name = "upb",
#     sha256 = "91fa3fc98538f643904b86660298a26f38259e6a51d79eb6924f8be8bdc9975e",
#     strip_prefix = "upb-a1c2caeb25b21644700b9423da573b1ccddc35a7",
#     urls = [
#         "https://storage.googleapis.com/mirror.tensorflow.org/github.com/protocolbuffers/upb/archive/a1c2caeb25b21644700b9423da573b1ccddc35a7.tar.gz",
#         "https://github.com/protocolbuffers/upb/archive/a1c2caeb25b21644700b9423da573b1ccddc35a7.tar.gz",
#     ],
# )


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

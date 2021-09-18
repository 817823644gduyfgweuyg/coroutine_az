FROM ubuntu:20.04
RUN apt-get update && apt-get install -y \
 gcc-10 \
 g++-10 \
 wget \
 git \
 python3 \
 python-is-python3 \
&& rm -rf /var/lib/apt/lists/*
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
RUN update-alternatives --set cc /usr/bin/gcc
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
RUN update-alternatives --set c++ /usr/bin/g++

WORKDIR /usr/local/bin
RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.10.1/bazelisk-linux-amd64 -O bazel
RUN chmod a+x bazel
WORKDIR /root
RUN git clone https://github.com/bazelbuild/bazel-watcher.git
WORKDIR /root/bazel-watcher
RUN bazel build //ibazel
RUN cp /root/bazel-watcher/bazel-bin/ibazel/linux_amd64_stripped/ibazel /usr/local/bin
WORKDIR /
RUN rm -rf /root
RUN mkdir root
ARG uid=1000
ARG user=${user}
ARG gid=1000
ARG group=${user}
RUN groupadd -g ${gid} ${group}
RUN useradd -ms /bin/bash ${user} -u ${uid} -g ${gid}


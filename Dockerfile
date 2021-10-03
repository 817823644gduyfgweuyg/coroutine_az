FROM ubuntu:20.04
RUN apt-get update && apt-get install -y \
 gcc-10 \
 g++-10 \
 gdb \
 wget \
 git \
 python3 \
 python3-distutils \
 python3-pip \
 python-is-python3 \
 zip \
 unzip \
 apt-transport-https \
 curl \
 gnupg \
 clang-format \
 sudo \
&& rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
RUN mv bazel.gpg /etc/apt/trusted.gpg.d/
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt-get update && \
  apt-get install bazel \
  && rm -rf /var/lib/apt/lists/*

RUN pip install pip numpy wheel
RUN pip install keras_preprocessing --no-deps
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
RUN update-alternatives --set cc /usr/bin/gcc
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
RUN update-alternatives --set c++ /usr/bin/g++

WORKDIR /usr/local/bin

# RUN wget https://github.com/bazelbuild/bazelisk/releases/download/v1.10.1/bazelisk-linux-amd64 -O bazel
# RUN chmod a+x bazel
# WORKDIR /root
# RUN wget https://github.com/bazelbuild/bazel/archive/master.zip -O master.zip && unzip master.zip
# WORKDIR /root/bazel-master
# RUN bazel build //scripts:bazel-complete.bash
# RUN cp bazel-bin/scripts/bazel-complete.bash /etc/bash_completion.d/bazel

RUN wget https://github.com/bazelbuild/buildtools/releases/download/4.2.0/buildifier-linux-amd64 -O buildifier
RUN chmod a+x buildifier
RUN wget https://github.com/bazelbuild/bazel-watcher/releases/download/v0.15.10/ibazel_linux_amd64 -O ibazel
RUN chmod a+x ibazel
RUN curl -L https://github.com/grailbio/bazel-compilation-database/archive/0.5.2.tar.gz | tar -xz \
  && ln -f -s /usr/local/bin/bazel-compilation-database-0.5.2/generate.py bazel-compdb
WORKDIR /
RUN echo "ALL ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
ARG uid=1000
ARG user=user
ARG gid=1000
ARG group=${user}
RUN groupadd -g ${gid} ${group}
RUN useradd -ms /bin/bash ${user} -u ${uid} -g ${gid}
WORKDIR /home/${user}
USER ${user}
RUN echo "source /etc/bash_completion.d/bazel" >> .bashrc
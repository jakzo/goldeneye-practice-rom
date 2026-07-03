FROM ubuntu:22.04 AS base

# install distro packages
RUN apt update
RUN apt -y install bash-completion sudo binutils-mips-linux-gnu gcc-mips-linux-gnu g++-mips-linux-gnu wget make git python3 libcapstone-dev pkg-config build-essential

# install qemu (can be removed if you're using ido recomp)
RUN wget https://github.com/n64decomp/qemu-irix/releases/download/v2.11-deb/qemu-irix-2.11.0-2169-g32ab296eef_amd64.deb -P /tmp
RUN dpkg -i /tmp/qemu-irix-2.11.0-2169-g32ab296eef_amd64.deb

# setup sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# setup developer directory
RUN useradd -ms /bin/bash dev
RUN usermod -aG sudo dev
USER dev
WORKDIR /home/dev

CMD ["/bin/bash"]

FROM ubuntu:24.04 AS test

ARG ARES_VERSION=v148

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN apt update && DEBIAN_FRONTEND=noninteractive apt -y install \
    binutils-mips-linux-gnu \
    build-essential \
    ca-certificates \
    cmake \
    curl \
    ffmpeg \
    g++-mips-linux-gnu \
    gcc-mips-linux-gnu \
    git \
    libcanberra-gtk3-module \
    libcapstone-dev \
    libgl-dev \
    libgtk-3-dev \
    make \
    mesa-vulkan-drivers \
    ninja-build \
    pkg-config \
    python3 \
    xauth \
    xvfb

RUN curl --fail --location --retry 3 \
      "https://github.com/ares-emulator/ares/archive/refs/tags/${ARES_VERSION}.tar.gz" \
      | tar --extract --gzip --directory /tmp && \
    cmake \
      -S "/tmp/ares-${ARES_VERSION#v}" \
      -B /tmp/ares-build \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/ares \
      -DARES_CORES=n64 \
      -DARES_BUILD_LOCAL=OFF \
      -DARES_BUNDLE_SHADERS=OFF \
      -DARES_ENABLE_LIBRASHADER=OFF \
      -DENABLE_IPO=OFF \
      -DARES_ENABLE_ALSA=OFF \
      -DARES_ENABLE_AO=OFF \
      -DARES_ENABLE_OPENAL=OFF \
      -DARES_ENABLE_OSS=OFF \
      -DARES_ENABLE_PULSEAUDIO=OFF \
      -DARES_ENABLE_SDL=OFF \
      -DARES_ENABLE_UDEV=OFF && \
    cmake --build /tmp/ares-build --parallel 2 && \
    cmake --install /tmp/ares-build && \
    rm -rf /tmp/ares-build "/tmp/ares-${ARES_VERSION#v}"

ENV ARES="/opt/ares/bin/ares"
ENV ARES_ARGS="--no-file-prompt --setting Audio/Driver=None --setting Input/Driver=None"
ENV TERM="xterm"
ENV XDG_CONFIG_HOME="/tmp/ares-config"
ENV XDG_DATA_HOME="/tmp/ares-data"

RUN useradd -ms /bin/bash dev
USER dev
WORKDIR /home/dev

# Keep the default development image small; local tests select the test target explicitly.
FROM base AS goldeneye

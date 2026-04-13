FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive

# Build tools, cross-compiler, debug tools.
# libgpiod-dev installed for both host (amd64) and cross-compile (arm64).
# deb.debian.org serves all architectures from the same mirror — no multiarch
# source configuration needed.
# spdlog and Catch2 are fetched by CMake via FetchContent — no system packages needed.
RUN dpkg --add-architecture arm64 \
 && apt-get update && apt-get install -y --no-install-recommends \
    bash \
    build-essential \
    cmake \
    ninja-build \
    git \
    ca-certificates \
    curl \
    openssh-client \
    rsync \
    file \
    gdb \
    gdb-multiarch \
    clang-tidy \
    doxygen \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    libgpiod-dev \
    libgpiod-dev:arm64 \
 && rm -rf /var/lib/apt/lists/*

ENV CMAKE_EXPORT_COMPILE_COMMANDS=ON

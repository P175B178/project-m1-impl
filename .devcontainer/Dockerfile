# Debian 13 (Trixie) — matches Raspberry Pi OS
FROM debian:13-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    # ── Compilers and build tools (Trixie: gcc/g++ 14.2, clang 19.1) ──
    build-essential \
    gcc \
    g++ \
    clang \
    cmake \
    ninja-build \
    clang-format \
    clang-tidy \
    # ── Cross-compilation (Raspberry Pi 5 / aarch64) ──
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    # ── Debugging (gdb 16.3) ──
    gdb \
    gdb-multiarch \
    # ── Version control ──
    git \
    # ── Remote access and file transfer ──
    openssh-client \
    sshpass \
    rsync \
    # ── Networking diagnostics ──
    curl \
    iputils-ping \
    netcat-openbsd \
    # ── Misc utilities ──
    file \
    less \
    tree \
    nano \
    jq \
    zip \
    unzip \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

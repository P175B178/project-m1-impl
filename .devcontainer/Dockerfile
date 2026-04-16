# Debian 13 (Trixie) — matches Raspberry Pi OS
FROM debian:13-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    # ── Compilers (Trixie: gcc/g++ 14.2, clang 19.1) ──
    build-essential \
    gcc \
    g++ \
    clang \
    # ── Debugging (gdb 16.3) ──
    gdb \
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

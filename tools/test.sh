#!/usr/bin/env bash
set -euo pipefail

# Deploys and runs the test binary on the Pi over SSH.
# Note: tests are built with ENABLE_HARDWARE=OFF (warden_core only, no GPIO).
#
# Usage:
#   tools/test.sh
#   tools/test.sh build/pi-release/tests/warden_tests

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
PI="${PI_USER}@${PI_HOST}"
LOCAL_TEST_BIN="${1:-build/pi-debug/tests/warden_tests}"

if [[ ! -f "${LOCAL_TEST_BIN}" ]]; then
  echo "Test binary not found: ${LOCAL_TEST_BIN}" >&2
  echo "Build first: cmake --preset pi-debug && cmake --build --preset pi-debug" >&2
  exit 2
fi

REMOTE_DIR="/home/${PI_USER}/apps/warden/tests"
REMOTE_TEST_BIN="${REMOTE_DIR}/warden_tests"

SSH_OPTS=(-p "${PORT}" -o StrictHostKeyChecking=accept-new)
RSYNC_RSH="ssh -p ${PORT} -o StrictHostKeyChecking=accept-new"

echo "==> Deploying ${LOCAL_TEST_BIN} -> ${PI}:${REMOTE_TEST_BIN}"
ssh "${SSH_OPTS[@]}" "${PI}" "mkdir -p '${REMOTE_DIR}'"
rsync -av -e "${RSYNC_RSH}" "${LOCAL_TEST_BIN}" "${PI}:${REMOTE_TEST_BIN}"
ssh "${SSH_OPTS[@]}" "${PI}" "chmod +x '${REMOTE_TEST_BIN}'"

echo "==> Running tests on ${PI}..."
exec ssh "${SSH_OPTS[@]}" "${PI}" "${REMOTE_TEST_BIN}"

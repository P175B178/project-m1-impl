#!/usr/bin/env bash
set -euo pipefail

# Deploys the warden binary and config to the Pi over SSH.
#
# Usage:
#   tools/deploy.sh                        # deploy build/arm-debug/warden
#   tools/deploy.sh build/arm-release/warden

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
PI="${PI_USER}@${PI_HOST}"
REMOTE_DIR="/home/${PI_USER}/apps/warden"
LOCAL_BIN="${1:-build/arm-debug/warden}"

if [[ ! -f "${LOCAL_BIN}" ]]; then
  echo "Binary not found: ${LOCAL_BIN}" >&2
  exit 2
fi

SSH_OPTS=(-p "${PORT}" -o StrictHostKeyChecking=accept-new)
RSYNC_RSH="ssh -p ${PORT} -o StrictHostKeyChecking=accept-new"

echo "==> Deploying ${LOCAL_BIN} -> ${PI}:${REMOTE_DIR}/warden"
ssh "${SSH_OPTS[@]}" "${PI}" "mkdir -p '${REMOTE_DIR}/config'"
rsync -av -e "${RSYNC_RSH}" "${LOCAL_BIN}" "${PI}:${REMOTE_DIR}/warden"
rsync -av -e "${RSYNC_RSH}" config/ "${PI}:${REMOTE_DIR}/config/"
ssh "${SSH_OPTS[@]}" "${PI}" "chmod +x '${REMOTE_DIR}/warden'"

echo "==> Deploy complete."

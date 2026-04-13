#!/usr/bin/env bash
set -euo pipefail

# Starts gdbserver on the Pi for remote debugging from VS Code.
#
# Usage:
#   tools/pi/debug.sh
#
# Keep this terminal open while debugging. VS Code connects via the
# "Remote ARM Debug" launch configuration.

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
DEBUG_PORT="${DEBUG_PORT:-2345}"
PI="${PI_USER}@${PI_HOST}"
REMOTE_BIN="/home/${PI_USER}/apps/warden/warden"
REMOTE_CFG="/home/${PI_USER}/apps/warden/config/config.cfg"

SSH_OPTS=(-p "${PORT}" -o StrictHostKeyChecking=accept-new)

echo "==> Checking if debug port ${DEBUG_PORT} is already in use..."
PORT_IN_USE=$(
  ssh "${SSH_OPTS[@]}" "${PI}" "
    command -v lsof >/dev/null 2>&1 &&
    lsof -nP -iTCP:${DEBUG_PORT} -sTCP:LISTEN >/dev/null 2>&1 &&
    echo yes || echo no
  "
)

if [[ "${PORT_IN_USE}" == "yes" ]]; then
  echo "==> Port ${DEBUG_PORT} in use — killing existing process..."
  ssh "${SSH_OPTS[@]}" "${PI}" \
    "lsof -t -iTCP:${DEBUG_PORT} -sTCP:LISTEN | xargs -r kill -9"
fi

echo "==> Starting gdbserver on port ${DEBUG_PORT}..."
ssh "${SSH_OPTS[@]}" "${PI}" \
  "gdbserver 0.0.0.0:${DEBUG_PORT} '${REMOTE_BIN}' --config '${REMOTE_CFG}'" &
DEBUG_PID=$!

# Wait until gdbserver is accepting connections
for i in $(seq 1 15); do
  if (exec 3<>/dev/tcp/${PI_HOST}/${DEBUG_PORT}) 2>/dev/null; then
    sleep 1
    break
  fi
  if ! kill -0 ${DEBUG_PID} 2>/dev/null; then
    echo "Error: gdbserver exited unexpectedly." >&2
    exit 1
  fi
  sleep 1
done

echo "==> Debug server ready. Leave this terminal open."
wait ${DEBUG_PID}

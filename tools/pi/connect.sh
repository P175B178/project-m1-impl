#!/usr/bin/env bash
set -euo pipefail

# Open an interactive SSH shell on the Pi.
#
# Usage:
#   tools/pi/connect.sh

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
PI="${PI_USER}@${PI_HOST}"
SSH_OPTS=(-p "${PORT}" -o StrictHostKeyChecking=accept-new)

exec ssh "${SSH_OPTS[@]}" "${PI}"

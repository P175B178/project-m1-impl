#!/usr/bin/env bash
set -euo pipefail

# ONE-TIME: Installs SSH key-based authentication on the Pi.
# After this, all other scripts connect without a password.
#
# Usage:
#   tools/pi/01-setup-ssh.sh

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"
: "${PI_PASSWORD:?PI_PASSWORD not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
PI="${PI_USER}@${PI_HOST}"
SSH_OPTS=(-p "${PORT}" -o StrictHostKeyChecking=accept-new)

echo "==> Cleaning stale SSH host key (if any)..."
ssh-keygen -R "[${PI_HOST}]:${PORT}" >/dev/null 2>&1 || true

if [[ ! -f "${HOME}/.ssh/id_ed25519.pub" ]]; then
  echo "==> Generating ed25519 key..."
  ssh-keygen -t ed25519 -N "" -f "${HOME}/.ssh/id_ed25519"
fi

echo "==> Installing SSH key on ${PI}..."
sshpass -p "${PI_PASSWORD}" ssh-copy-id "${SSH_OPTS[@]}" "${PI}"

echo "==> SSH setup complete. Password no longer needed."

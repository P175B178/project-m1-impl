#!/usr/bin/env bash
set -euo pipefail

# ONE-TIME: Installs runtime dependencies on the Pi and enables the DHT22 kernel driver.
# Safe to run multiple times — all steps are idempotent.
#
# Usage:
#   tools/pi/02-bootstrap.sh

: "${PI_USER:?PI_USER not set — copy .env.sample to .env and configure it.}"
: "${PI_HOST:?PI_HOST not set — copy .env.sample to .env and configure it.}"

PORT="${PI_PORT:-22}"
PI="${PI_USER}@${PI_HOST}"

echo "==> Bootstrapping Raspberry Pi: ${PI} (port ${PORT})"

ssh -p "${PORT}" -o StrictHostKeyChecking=accept-new "${PI}" bash -s <<'EOF'
set -euo pipefail

echo "==> Updating apt index"
sudo apt-get update

echo "==> Installing runtime dependencies"
sudo apt-get install -y --no-install-recommends \
  ca-certificates \
  rsync \
  gdbserver \
  libgpiod3 \
  gpiod \
  htop

echo "==> Creating app directory"
mkdir -p "$HOME/apps/warden/config" "$HOME/apps/warden/tests"

echo "==> Configuring LED permissions"
UDEV_RULE='SUBSYSTEM=="leds", RUN+="/bin/chgrp video /sys%p/brightness /sys%p/trigger", RUN+="/bin/chmod g+w /sys%p/brightness /sys%p/trigger"'
UDEV_FILE=/etc/udev/rules.d/99-warden-leds.rules
if grep -qxF "$UDEV_RULE" "$UDEV_FILE" 2>/dev/null; then
  echo "    Already configured — skipping"
else
  echo "$UDEV_RULE" | sudo tee "$UDEV_FILE" > /dev/null
  sudo udevadm control --reload-rules
  sudo udevadm trigger --subsystem-match=leds --action=change
  echo "    Done."
fi

echo "==> Enabling DHT22 kernel driver"
CONFIG=/boot/firmware/config.txt
OVERLAY="dtoverlay=dht11,gpiopin=4"
if grep -qxF "$OVERLAY" "$CONFIG"; then
  echo "    Already present in $CONFIG — skipping"
else
  echo "$OVERLAY" | sudo tee -a "$CONFIG" > /dev/null
  echo "    Added. A reboot is required for the driver to load."
  REBOOT_REQUIRED=true
fi

echo "==> Done"
uname -m

if [[ "${REBOOT_REQUIRED:-false}" == "true" ]]; then
  echo ""
  echo "*** Reboot the Pi for the DHT22 driver to take effect: sudo reboot ***"
fi
EOF

echo "Bootstrap complete."

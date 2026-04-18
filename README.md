# Warden — Environmental Monitor

IoT environmental monitoring system for Raspberry Pi 5.

---

## Project structure

```text
include/hardware/  — public interfaces
src/core/          — core logic
src/sim/           — simulated hardware stubs (host only)
tests/             — unit tests (run on host)
tests/mocks/       — mock implementations for testing
config/config.cfg  — configuration file
```

---

## Dev environment

The project uses a **VS Code dev container**. Nothing needs to be installed on the host except:

- [Docker](https://docs.docker.com/get-docker/)
- [VS Code](https://code.visualstudio.com/) with the **Dev Containers** extension

Open the project in VS Code, then click **Reopen in Container** (or `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**).

> [!TIP]
> Throughout this guide, VS Code tasks are run via `Ctrl+Shift+P` → **Tasks: Run Task**.

---

## Local build and simulation

The default build targets the host machine (no Pi required). The binary runs with a **simulated sensor** that produces oscillating temperature and humidity values.

**VS Code task:** `Local: Build` (`Ctrl+Shift+B`)

```bash
cmake --preset debug && cmake --build --preset debug
```

**Run the simulation:**

**VS Code task:** `Local: Run (sim)`

```bash
./build/debug/warden -c config/config.cfg
```

**Debug the simulation with breakpoints:**

**VS Code launch config:** `Local: Debug` (`F5`) — builds automatically and launches under GDB.

---

## Tests

Tests run on the host — no Pi required.

**VS Code task:** `Local: Run tests`

```bash
ctest --preset debug
```

---

## Deploy to Raspberry Pi

### One-time setup

**1. Configure your Pi's address.**
`.env` is created automatically from `.env.sample` on first container start. Edit it to set your Pi's details:

```bash
# .env
PI_HOST=<Pi IP address>
PI_USER=<your username on the Pi>
PI_PASSWORD=<your Pi password>
```

> [!NOTE]
> `.env` is gitignored and never committed. After editing it, restart the container for the changes to take effect: `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**.

**2. Install SSH key** (once — uses `PI_PASSWORD` from `.env`):

**VS Code task:** `Pi init: 1. SSH key`

```bash
tools/pi/init/01-setup-ssh.sh
```

**3. Bootstrap the Pi** — installs runtime dependencies and configures LED permissions. Safe to run multiple times.

**VS Code task:** `Pi init: 2. Bootstrap`

```bash
tools/pi/init/02-bootstrap.sh
```

Bootstrap configures a udev rule so your user can write to `/sys/class/leds/` without sudo. To do it manually, SSH into the Pi (`Pi: SSH` task or `tools/pi/connect.sh`) and run:

```bash
echo 'SUBSYSTEM=="leds", RUN+="/bin/chgrp video /sys%p/brightness /sys%p/trigger", RUN+="/bin/chmod g+w /sys%p/brightness /sys%p/trigger"' \
  | sudo tee /etc/udev/rules.d/99-warden-leds.rules
sudo udevadm control --reload-rules
sudo udevadm trigger --subsystem-match=leds --action=change
```

Verify on the Pi by writing directly:

```bash
echo 1 > /sys/class/leds/PWR/brightness   # red LED on
echo 0 > /sys/class/leds/PWR/brightness   # red LED off
```

### Build for Pi

**VS Code task:** `Pi: Build`

```bash
cmake --preset pi-debug && cmake --build --preset pi-debug
```

### Deploy and run

**VS Code task:** `Pi: Run` — builds, deploys, and runs in one step

```bash
cmake --preset pi-debug && cmake --build --preset pi-debug
tools/pi/deploy.sh
tools/pi/run.sh  # streams output to this terminal, Ctrl+C to stop
```

To deploy without rebuilding (e.g. after `Pi: Build`):

**VS Code task:** `Pi: Deploy`

```bash
tools/pi/deploy.sh
```

---

## Remote debugging

Requires the debug binary to already be deployed (`Pi: Build` + `Pi: Deploy`).

**VS Code launch config:** `Pi: Remote Debug` — connects to gdbserver on the Pi automatically. Select it in the debug dropdown (`Ctrl+Shift+D`) and press `F5`.

Breakpoints, step-through, and variable inspection all work normally.

---

## Utilities

### Formatting

Clang-format runs automatically:
- on save (`Ctrl+S`) in VS Code
- on every commit via the git pre-commit hook

The pre-commit hook lives in `.githooks/` and is configured automatically when the dev container starts. If you're working outside the container, run once after cloning:

```bash
git config core.hooksPath .githooks
```

To reformat all files manually:

**VS Code task:** `Format all sources`

```bash
cmake --preset debug && cmake --build --preset debug --target format
```

### Verify before committing

Builds, formats, runs static analysis (clang-tidy), and tests:

**VS Code task:** `Workflow: Verify`

```bash
cmake --preset debug && cmake --build --preset debug --target format && cmake --workflow --preset tidy && cmake --build --preset debug && ctest --preset debug
```

### Clean build directories

Useful when builds get into a broken state:

**VS Code task:** `Clean all build directories`

```bash
rm -rf build/
```

### Open a shell on the Pi

**VS Code task:** `Pi: SSH`

```bash
tools/pi/connect.sh
```

# Warden

Monitors temperature and humidity via a DHT22 sensor on a Raspberry Pi.
Signals state (Normal / Warning / Alert) through the Pi's built-in LEDs and a buzzer.

Built with C++23, cross-compiled inside a dev container, deployed over SSH.

---

## Project structure

```text
include/warden/   — public interfaces and domain types (no hardware deps)
src/              — core logic (StateMachine, ConfigLoader)
src/hardware/     — hardware drivers (Pi only, compiled with ENABLE_HARDWARE=ON)
tests/            — unit tests (run on host)
tests/mocks/      — mock implementations for testing without hardware
config/config.cfg — configuration file
tools/            — SSH/deploy/run helper scripts
```

---

## Dev environment

The project uses a **VS Code dev container** — all compilers, cross-toolchains, and tools are inside Docker. Nothing needs to be installed on the host except:

- [Docker](https://docs.docker.com/get-docker/)
- [VS Code](https://code.visualstudio.com/) with the **Dev Containers** extension (`ms-vscode-remote.remote-containers`)

Open the project folder in VS Code, then when prompted click **Reopen in Container** (or `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**).

> [!TIP]
> Throughout this guide, VS Code tasks are run via `Ctrl+Shift+P` → **Tasks: Run Task**.

---

## Build

The default build target is the Pi (aarch64 cross-compiled, hardware enabled).

**VS Code task:** `ARM: Build` (`Ctrl+Shift+B`)

```bash
cmake --preset pi-debug && cmake --build --preset pi-debug
```

---

## Tests

Tests run on the host — no Pi required. Hardware drivers are replaced by mocks.

**VS Code task:** `Local: Run tests`

```bash
cmake --preset debug && cmake --build --preset debug
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
```

> [!NOTE]
> `.env` is gitignored and never committed. After editing it, restart the container for the changes to take effect: `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**.

**2. Install SSH key** (once — will prompt for your Pi password):

**VS Code task:** `Pi: Setup SSH key`

```bash
tools/setup-ssh.sh
```

**3. Bootstrap the Pi** — installs runtime dependencies and enables the DHT22 kernel driver. Safe to run multiple times.

**VS Code task:** `Pi: Bootstrap`

```bash
tools/pi/bootstrap.sh
```

If the DHT22 overlay was not already present, bootstrap will ask you to reboot the Pi. After rebooting, verify the sensor is visible:

```bash
cat /sys/bus/iio/devices/iio:device0/in_temp_input
```

### Deploy and run

**VS Code task:** `ARM: Run` — builds, deploys, and runs in one step

```bash
tools/deploy.sh
tools/run.sh  # streams output to this terminal, Ctrl+C to stop
```

To deploy without rebuilding (e.g. after `Ctrl+Shift+B`):

**VS Code task:** `ARM: Deploy`

```bash
tools/deploy.sh
```

---

## Remote debugging

Requires the debug binary to already be deployed.

**VS Code launch config:** `Remote ARM Debug` (`F5`) — connects to gdbserver on the Pi automatically. Breakpoints, step-through, and variable inspection all work normally.

---

## Shipping a release

Builds an optimised Pi binary and packages it as `.deb` and `.tar.gz`:

**VS Code task:** `Workflow: Ship`

```bash
cmake --workflow --preset ship
```

The `.deb` package is written to `build/pi-release/`. Install it on the Pi:

```bash
sudo apt install ./warden-1.0.0-arm64.deb
```

---

## Utilities

**Before committing** — run the full test suite and static analysis to match what CI checks:

**VS Code task:** `Workflow: Verify`

```bash
cmake --workflow --preset build-and-test && cmake --workflow --preset tidy
```

> [!TIP]
> Formatting is also checked automatically on every commit via a git pre-commit hook.

---

**Format all source files** according to `.clang-format`:

**VS Code task:** `Format all sources` (`Ctrl+S` formats the current file on save)

```bash
cmake --preset pi-debug && cmake --build --preset pi-debug --target format
```

---

**Clean all build directories** — useful when builds get into a broken state:

**VS Code task:** `Clean all build directories`

```bash
rm -rf build
```

---

**SSH shell into the Pi:**

**VS Code task:** `Pi: SSH`

```bash
tools/connect.sh
```

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

Two build targets exist: **host** (x86\_64, hardware disabled, for running tests) and **Pi** (aarch64 cross-compiled, hardware enabled).

**VS Code task:** `Local: Build (Debug)` (or `Ctrl+Shift+B` — runs the default build task directly)

**VS Code task:** `ARM: Build (Debug)` / `ARM: Build (Release)` — cross-compile for Pi

```bash
# Host build
cmake --preset debug && cmake --build --preset debug

# Pi cross-compile
cmake --preset pi-debug && cmake --build --preset pi-debug
```

---

## Tests

Tests run on the host — no Pi required. Hardware drivers are excluded from the host build.

**VS Code task:** `Local: Run tests (Debug)` — builds and runs in one step

```bash
ctest --preset debug  # requires a prior host debug build
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

**VS Code task:** `ARM: Run app (Debug)` — chains build → deploy → run automatically

```bash
tools/deploy.sh
tools/run.sh  # streams output to this terminal, Ctrl+C to stop
```

---

## Remote debugging

Requires the debug binary to already be deployed — run the deploy step first.

**VS Code:** press `F5` — **Remote ARM Debug** is the default launch configuration. VS Code starts `gdbserver` on the Pi automatically and connects. Breakpoints, step-through, and variable inspection all work normally.

---

## Utilities

**Format all source files** (`include/` and `src/`) according to `.clang-format`:

**VS Code task:** `Local: Format all sources`

```bash
cmake --build --preset debug --target format
```

> [!TIP]
> Files are also formatted automatically on save (`Ctrl+S`) inside the dev container.

---

**SSH shell into the Pi:**

**VS Code task:** `ARM: SSH shell`

```bash
tools/connect.sh
```

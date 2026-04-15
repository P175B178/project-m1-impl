# Warden

Monitors temperature and humidity via a DHT22 sensor on a Raspberry Pi.
Signals state (Normal / Warning / Alert) through the Pi's built-in LEDs and a buzzer.

Built with C++23, cross-compiled inside a dev container, deployed over SSH.

---

## Project structure

```text
include/warden/   — public interfaces and domain types (no hardware deps)
src/core/         — core logic (StateMachine, ConfigLoader, WardenApp)
src/hardware/     — hardware drivers (Pi only, compiled with ENABLE_HARDWARE=ON)
src/sim/          — simulated hardware stubs (host only, compiled with ENABLE_HARDWARE=OFF)
tests/            — unit tests (run on host)
tests/mocks/      — mock implementations for testing without hardware
config/config.cfg — configuration file
tools/pi/         — Pi helper scripts (SSH, deploy, run, debug)
tools/pi/init/    — one-time Pi setup scripts (run in order)
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

Tests run on the host — no Pi required. Hardware drivers are replaced by mocks.

**VS Code task:** `Local: Run tests`

```bash
ctest --preset debug
```

The task builds automatically before running tests.

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

**3. Bootstrap the Pi** — installs runtime dependencies and enables the DHT22 kernel driver. Safe to run multiple times.

**VS Code task:** `Pi init: 2. Bootstrap`

```bash
tools/pi/init/02-bootstrap.sh
```

If the DHT22 overlay was not already present, bootstrap will ask you to reboot the Pi. After rebooting, SSH into the Pi (`Pi: SSH` task or `tools/pi/connect.sh`) and verify the sensor is visible:

```bash
cat /sys/bus/iio/devices/iio:device0/in_temp_input
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

## Packaging a release

Builds an optimized Pi binary and packages it as `.deb` and `.tar.gz`:

**VS Code task:** `Workflow: Package`

```bash
rm -rf package && cmake --workflow --preset package
```

The packages are written to `package/`. Install the `.deb` on the Pi:

```bash
sudo apt install ./warden_1.0.0_arm64.deb
```

---

## Utilities

### Formatting

Clang-format runs automatically:
- on save (`Ctrl+S`) in VS Code
- on every commit via the git pre-commit hook

To reformat all files manually:

**VS Code task:** `Format all sources`

```bash
cmake --preset debug && cmake --build --preset debug --target format
```

### Verify before committing

Builds, runs tests, and runs static analysis (clang-tidy):

**VS Code task:** `Workflow: Verify`

```bash
cmake --workflow --preset verify && cmake --workflow --preset tidy
```

### Clean build directories

Useful when builds get into a broken state:

**VS Code task:** `Clean all build directories`

```bash
rm -rf build
```

### Open a shell on the Pi

**VS Code task:** `Pi: SSH`

```bash
tools/pi/connect.sh
```

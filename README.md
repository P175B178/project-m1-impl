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

## Cross-compile for Raspberry Pi

Build an ARM64 binary inside the dev container:

**VS Code task:** `Pi: Build`

```bash
cmake --preset pi-debug && cmake --build --preset pi-debug
```

**Release build:**

**VS Code task:** `Pi: Build (Release)`

```bash
cmake --preset pi-release && cmake --build --preset pi-release
```

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

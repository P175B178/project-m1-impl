# Warden — Environmental Monitor

IoT environmental monitoring system for Raspberry Pi 5.

---

## Dev environment

The project uses a **VS Code dev container**. Nothing needs to be installed on the host except:

- [Docker](https://docs.docker.com/get-docker/)
- [VS Code](https://code.visualstudio.com/) with the **Dev Containers** extension

Open the project in VS Code, then click **Reopen in Container** (or `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**).

---

## Build and run

**VS Code task:** `Build` (`Ctrl+Shift+B`)

```bash
cmake --preset debug && cmake --build --preset debug
```

**Run:**

**VS Code task:** `Run`

```bash
./build/debug/main
```

**Debug:** Press `F5` — builds and launches under GDB.

---

## Tests

**VS Code task:** `Run tests`

```bash
ctest --preset debug
```

---

## Utilities

### Formatting

Clang-format runs automatically on save (`Ctrl+S`) in VS Code.

To reformat all files manually:

**VS Code task:** `Format all sources`

```bash
cmake --preset debug && cmake --build --preset debug --target format
```

### Clean build directories

Useful when builds get into a broken state:

**VS Code task:** `Clean all build directories`

```bash
rm -rf build/
```

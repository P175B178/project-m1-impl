# Embedded Project

Project template for module P175B178 — Modern Application of C++ for Hardware Control.

## Getting Started

1. Open this repository in VS Code
2. Reopen in Dev Container (install the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension if you haven't)
3. Start building

## Provided Interfaces

`include/hardware/` contains abstract interfaces that define the hardware contract for this project:

- `Reading.hpp` — temperature + humidity data struct
- `Sensor.hpp` — sensor reading with `std::expected` error handling
- `LedColor.hpp` — color enum (Green, Orange, Red)
- `Led.hpp` — LED color and blink control
- `Buzzer.hpp` — short and long beep sequences

These declarations match the hardware and project requirements. You may extend these files — add namespaces, additional methods, documentation, etc. — but do not remove or modify the existing declarations.

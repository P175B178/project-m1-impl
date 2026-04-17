#include "core/ConfigLoader.hpp"
#include "core/WardenApp.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <cstdlib>
#include <string>

// ── CLI ──────────────────────────────────────────────────────────────────────
struct CliArgs {
  std::string configPath{"/etc/warden/config.cfg"};
};

static CliArgs parseCli(int argc, char **argv) {
  CliArgs args;

  CLI::App app{"Warden — environmental monitor", "warden"};
  app.set_version_flag("-v,--version", std::string{WARDEN_VERSION});
  app.add_option("-c,--config", args.configPath, "Config file")->capture_default_str();

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &err) {
    std::exit(app.exit(err));
  }
  return args;
}

// ── Entry point ──────────────────────────────────────────────────────────────
int main(int argc, char **argv) { // NOLINT(bugprone-exception-escape)
  const auto args = parseCli(argc, argv);

  const auto configResult = warden::ConfigLoader::load(args.configPath);
  if (!configResult) {
    spdlog::error("Failed to load config '{}': {}", args.configPath, configResult.error());
    return EXIT_FAILURE;
  }
  const auto &config = *configResult;

  spdlog::info("Warden {} starting — config: {}", WARDEN_VERSION, args.configPath);
  spdlog::info("Temp threshold: {:.1f} C, humidity threshold: {:.1f}%", config.temperatureThreshold,
               config.humidityThreshold);
  spdlog::info("Read interval: {}s, averaging window: {} samples", config.readInterval.count(), config.averagingWindow);
  spdlog::info("Sensor validation: temp [{:.1f}, {:.1f}] C  hum [{:.1f}, {:.1f}]%", config.minTemperature,
               config.maxTemperature, config.minHumidity, config.maxHumidity);

  std::puts("\n"
            "  ╔══════════════════════════════════════════════════════╗\n"
            "  ║                                                      ║\n"
            "  ║   *** S I M U L A T I O N   M O D E ***              ║\n"
            "  ║                                                      ║\n"
            "  ║   No real hardware. Sensor readings are fake.        ║\n"
            "  ║   Do NOT use this output for anything real.          ║\n"
            "  ║                                                      ║\n"
            "  ╚══════════════════════════════════════════════════════╝\n");

  warden::sim::SimSensor sensor;
  warden::sim::StubLed led;
  warden::sim::StubBuzzer buzzer;

  spdlog::info("Running — press Ctrl-C to stop");

  warden::WardenApp app{sensor, led, buzzer, config};
  app.run();
}

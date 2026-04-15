#include "ConfigLoader.hpp"
#include "WardenApp.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <print>
#include <stop_token>
#include <string>

#ifdef HARDWARE_DISABLED
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"
#else
#include "hardware/buzzer/GpioBuzzer.hpp"
#include "hardware/led/StatusLed.hpp"
#include "hardware/sensor/Dht22Sensor.hpp"
#endif

// ── Signal handling ────────────────────────────────────────────────────────
namespace {
std::stop_source appStop; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
void onSignal(int /*signal*/) { appStop.request_stop(); }
} // namespace

// ── CLI ────────────────────────────────────────────────────────────────────
static void printHelp(const char *argv0) {
  std::println("Usage: {} [options]\n"
               "\n"
               "Options:\n"
               "  -c, --config <path>   Config file (default: /etc/warden/config.cfg)\n"
               "  -v, --version         Print version and exit\n"
               "  -h, --help            Print this help and exit",
               argv0);
}

static void printVersion() { std::println("warden {}", WARDEN_VERSION); }

// ── Entry point ────────────────────────────────────────────────────────────
int main(int argc, char *argv[]) { // NOLINT(bugprone-exception-escape)
  std::string configPath{"/etc/warden/config.cfg"};

  static const std::array<option, 4> longOpts{{
      {.name = "config", .has_arg = required_argument, .flag = nullptr, .val = 'c'},
      {.name = "version", .has_arg = no_argument, .flag = nullptr, .val = 'v'},
      {.name = "help", .has_arg = no_argument, .flag = nullptr, .val = 'h'},
      {.name = nullptr, .has_arg = 0, .flag = nullptr, .val = 0},
  }};

  int opt = 0;
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-pointer-arithmetic)
  while ((opt = getopt_long(argc, argv, "c:vh", longOpts.data(), nullptr)) != -1) {
    switch (opt) {
    case 'c':
      configPath = optarg;
      break;
    case 'v':
      printVersion();
      return EXIT_SUCCESS;
    case 'h':
      printHelp(argv[0]);
      return EXIT_SUCCESS;
    default:
      printHelp(argv[0]);
      return EXIT_FAILURE;
    }
  }
  // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-pointer-arithmetic)

  const auto configResult = warden::ConfigLoader::load(configPath);
  if (!configResult) {
    spdlog::error("Failed to load config '{}': {}", configPath, configResult.error());
    return EXIT_FAILURE;
  }
  const auto &config = *configResult;

  spdlog::info("Warden {} starting — config: {}", WARDEN_VERSION, configPath);
  spdlog::info("Temp threshold: {:.1f}°C, humidity threshold: {:.1f}%", config.temperatureThreshold,
               config.humidityThreshold);
  spdlog::info("Read interval: {}s, averaging window: {} samples", config.readInterval.count(), config.averagingWindow);

#ifdef HARDWARE_DISABLED
  std::puts("\n"
            "  ╔══════════════════════════════════════════════════════╗\n"
            "  ║                                                      ║\n"
            "  ║   *** S I M U L A T I O N   M O D E ***              ║\n"
            "  ║                                                      ║\n"
            "  ║   No real hardware. Sensor readings are fake.        ║\n"
            "  ║   Do NOT use this output for anything real.          ║\n"
            "  ║                                                      ║\n"
            "  ╚══════════════════════════════════════════════════════╝\n");
  spdlog::warn("Hardware disabled — running with simulated sensor");
  warden::sim::SimSensor sensor;
  warden::sim::StubLed led;
  warden::sim::StubBuzzer buzzer;
#else
  warden::hardware::Dht22Sensor sensor{"/sys/bus/iio/devices/iio:device0", config.minTemperature, config.maxTemperature,
                                       config.minHumidity, config.maxHumidity};
  warden::hardware::StatusLed led{};
  warden::hardware::GpioBuzzer buzzer{"/dev/gpiochip4"};
#endif

  std::signal(SIGINT, onSignal);
  std::signal(SIGTERM, onSignal);

  spdlog::info("Running — press Ctrl-C to stop");

  warden::WardenApp app{sensor, led, buzzer, config};
  app.run(appStop.get_token());

  spdlog::info("Shutting down");
  return EXIT_SUCCESS;
}

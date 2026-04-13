#include "warden/ConfigLoader.hpp"
#include "warden/WardenApp.hpp"

#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>

#ifndef HARDWARE_DISABLED
#include "hardware/buzzer/GpioBuzzer.hpp"
#include "hardware/led/StatusLed.hpp"
#include "hardware/sensor/Dht22Sensor.hpp"
#endif

// ── Signal handling ────────────────────────────────────────────────────────
namespace {
std::atomic<bool> running{true};
void onSignal(int) { running = false; }
} // namespace

// ── CLI ────────────────────────────────────────────────────────────────────
static void printHelp(const char *argv0) {
  std::printf("Usage: %s [options]\n"
              "\n"
              "Options:\n"
              "  -c, --config <path>   Config file (default: /etc/warden/config.cfg)\n"
              "  -v, --version         Print version and exit\n"
              "  -h, --help            Print this help and exit\n",
              argv0);
}

static void printVersion() { std::printf("warden %s\n", WARDEN_VERSION); }

// ── Entry point ────────────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
  std::string configPath{"/etc/warden/config.cfg"};

  static const option longOpts[] = {{"config", required_argument, nullptr, 'c'},
                                    {"version", no_argument, nullptr, 'v'},
                                    {"help", no_argument, nullptr, 'h'},
                                    {nullptr, 0, nullptr, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "c:vh", longOpts, nullptr)) != -1) {
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
  spdlog::warn("Built without hardware support — no sensor or LED or buzzer available");
  return EXIT_SUCCESS;
#else
  warden::hardware::Dht22Sensor sensor{"/sys/bus/iio/devices/iio:device0", config.minTemperature, config.maxTemperature,
                                       config.minHumidity, config.maxHumidity};
  warden::hardware::StatusLed led{};
  warden::hardware::GpioBuzzer buzzer{"/dev/gpiochip4"};

  std::signal(SIGINT, onSignal);
  std::signal(SIGTERM, onSignal);

  spdlog::info("Running — press Ctrl-C to stop");

  warden::WardenApp app{sensor, led, buzzer, config};
  app.run(running);

  spdlog::info("Shutting down");
  return EXIT_SUCCESS;
#endif
}

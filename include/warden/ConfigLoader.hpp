#pragma once

#include "Config.hpp"

#include <expected>
#include <string>
#include <string_view>

namespace warden {

/// Loads a TOML config file and returns a populated Config struct.
/// Missing keys keep their default values from Config's member initialisers.
class ConfigLoader {
public:
    [[nodiscard]] static std::expected<Config, std::string> load(std::string_view path);
};

} // namespace warden

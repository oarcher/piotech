#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
// #define XPOWERS_CHIP_AXP2101
// #define CONFIG_XPOWERS_ESP_IDF_NEW_API
// #include "XPowersLib.h"

namespace esphome {
namespace axp2101 {

static const uint8_t AXP2101_CHIP_ID_REG = 0x03;
static constexpr uint8_t AXP2101_ADDR = 0x34;

class AXP2101Component : public Component, public i2c::I2CDevice {
 public:
  AXP2101Component() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const { return setup_priority::DATA; }
  void enable();

  // protected:
  //  XPowersPMU pmu_;
};

}  // namespace axp2101
}  // namespace esphome

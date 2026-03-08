#include "axp2101.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/i2c/i2c_bus_esp_idf.h"

namespace esphome {
namespace axp2101 {

static const char *const TAG = "axp2101";

void AXP2101Component::setup() {
  ESP_LOGCONFIG(TAG, "Probing AXP at 0x%02X…", this->address_);
  auto chip = this->read_byte(AXP2101_CHIP_ID_REG);
  if (chip.has_value()) {
    ESP_LOGI(TAG, "ChipID = 0x%02X", chip.value());
  } else {
    ESP_LOGE(TAG, "No chip found at 0x%02X", this->address_);
    this->mark_failed();
  }
}

void AXP2101Component::enable() {
  ESP_LOGCONFIG(TAG, "Enabling PMU…");

  bool dc3_enabled;
  uint32_t dc3_volt;

  // Using XPowersLib is not implemented,
  // Because it requires to get the IC2 bus handle, and idf_bus->get_bus_handle() is not implemented in esphome
  // Need to add this to IDFI2CBus class in i2c_bus_esp_idf.h
  // #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 2)
  // i2c_master_bus_handle_t get_bus_handle() { return this->bus_; }
  // #endif

  // auto *idf_bus = static_cast<esphome::i2c::IDFI2CBus *>(this->bus_);
  //
  // this->pmu_.begin(idf_bus->get_bus_handle(), this->address_);
  //
  // ESP_LOGI(TAG, "getID:0x%x", this->pmu_.getChipID());
  //
  // dc3_enabled = this->pmu_.isEnableDC3();
  // dc3_volt = this->pmu_.getDC3Voltage();  // en mV
  // ESP_LOGI(TAG, "DC3 Enabled = %s, Voltage = %u mV", dc3_enabled ? "Yes" : "No", dc3_volt);
  //
  // // 4) (Optionnel) Active DC3 si ce n'est pas déjà fait
  // if (!dc3_enabled) {
  //   this->pmu_.setDC3Voltage(3000);
  //   this->pmu_.enableDC3();
  //   ESP_LOGI(TAG, "DC3 activé à 3000 mV");
  // }
  // return;

  const uint8_t POWER_CTL_REG = 0x80;
  ;
  const uint8_t DC3_ENABLE_BIT = 1 << 2;

  auto pwr_reg = this->read_byte(POWER_CTL_REG);
  // dc3_enabled = pwr_reg.has_value() && (pwr_reg.value() & DC3_ENABLE_BIT);

  const uint8_t DC3_VOLTAGE_REG = 0x84;
  // auto volt_reg = this->read_byte(DC3_VOLTAGE_REG);
  // dc3_volt = 0;
  // if (volt_reg.has_value()) {
  //   uint8_t val = volt_reg.value() & 0x7F;
  //   if (val < XPOWERS_AXP2101_DCDC3_VOL_STEPS2_BASE) {
  //     dc3_volt = val * XPOWERS_AXP2101_DCDC3_VOL_STEPS1 + XPOWERS_AXP2101_DCDC3_VOL_MIN;
  //   } else if (val < XPOWERS_AXP2101_DCDC3_VOL_STEPS3_BASE) {
  //     dc3_volt = val * XPOWERS_AXP2101_DCDC3_VOL_STEPS2 - 200;
  //   } else {
  //     dc3_volt = val * XPOWERS_AXP2101_DCDC3_VOL_STEPS3 - 7200;
  //   }
  // }
  // ESP_LOGI(TAG, "DC3 Enabled = %s, Voltage = %u mV", dc3_enabled ? "Yes" : "No", dc3_volt);

  const int DC3_VOL_MIN = 500;
  const uint8_t DC3_VOL_STEPS = 10u;
  dc3_enabled = false;
  if (!dc3_enabled) {
    uint8_t volt_val_to_write = (3000 - DC3_VOL_MIN) / DC3_VOL_STEPS;
    this->write_byte(DC3_VOLTAGE_REG, volt_val_to_write);

    if (pwr_reg.has_value()) {
      this->write_byte(POWER_CTL_REG, pwr_reg.value() | DC3_ENABLE_BIT);
    }
    ESP_LOGI(TAG, "PMU DC3 set to 3000 mV");
  }
  //
  // delay(100);
  //
  // return;
  //
  // const uint8_t POWER_CTL_REG = 0x80;     // Adresse hypothétique du registre de contrôle
  // const uint8_t DC3_ENABLE_BIT = 1 << 2;  // Bit hypothétique pour activer DC3
  //
  // auto pwr_reg = this->read_byte(POWER_CTL_REG);
  // dc3_enabled = pwr_reg.has_value() && (pwr_reg.value() & DC3_ENABLE_BIT);
  //
  // const uint8_t DC3_VOLTAGE_REG = 0x84;  // Adresse hypothétique du registre de tension DC3
  // auto volt_reg = this->read_byte(DC3_VOLTAGE_REG);
  // dc3_volt = 0;
  // if (volt_reg.has_value()) {
  //  // La formule de conversion est dans la datasheet
  //  dc3_volt = 700 + volt_reg.value() & 0x7F * 25;  // Formule hypothétique en mV
  //}
  // ESP_LOGI(TAG, "DC3 Enabled = %s, Voltage = %u mV", dc3_enabled ? "Yes" : "No", dc3_volt);
  //
  // if (!dc3_enabled) {
  //  // Calculer la valeur du registre à partir des mV (inverser la formule)
  //  uint8_t volt_val_to_write = (3000 - 700) / 25;  // Formule hypothétique
  //  this->write_byte(DC3_VOLTAGE_REG, volt_val_to_write);
  //
  //  // Activer le bit DC3 (lecture-modification-écriture pour ne pas altérer les autres bits)
  //  if (pwr_reg.has_value()) {
  //    this->write_byte(POWER_CTL_REG, pwr_reg.value() | DC3_ENABLE_BIT);
  //  }
  //  ESP_LOGI(TAG, "DC3 activé à 3000 mV (via I2C direct)");
  //}
}

void AXP2101Component::dump_config() { LOG_I2C_DEVICE(this); }

}  // namespace axp2101
}  // namespace esphome


#include "ota_http_backend.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/ota/ota_backend_arduino_esp32.h"
#include "esphome/components/ota/ota_backend_arduino_esp8266.h"
#include "esphome/components/ota/ota_backend_arduino_rp2040.h"
#include "esphome/components/ota/ota_backend_esp_idf.h"
#include "esphome/components/ota/ota_backend.h"

namespace esphome {
namespace ota_http {

static const char *const BACKEND_TAG = "ota_http.ota_backend";

std::unique_ptr<ota::OTABackend> make_ota_backend() {
#ifdef USE_ESP8266
  ESP_LOGD(BACKEND_TAG, "Using ArduinoESP8266OTABackend");
  return make_unique<ota::ArduinoESP8266OTABackend>();
#endif  // USE_ESP8266
#ifdef USE_ESP32
  ESP_LOGD(BACKEND_TAG, "Using ArduinoESP32OTABackend");
  return make_unique<ota::ArduinoESP32OTABackend>();
#endif  // USE_ESP32

#ifdef USE_ESP_IDF
  ESP_LOGD(BACKEND_TAG, "Using IDFOTABackend");
  return make_unique<ota::IDFOTABackend>();
#endif  // USE_ESP_IDF
#ifdef USE_RP2040
  ESP_LOGD(BACKEND_TAG, "Using ArduinoRP2040OTABackend");
  return make_unique<ota::ArduinoRP2040OTABackend>();
#endif  // USE_RP2040
  ESP_LOGE(BACKEND_TAG, "No OTA backend!");
}

}  // namespace ota_http
}  // namespace esphome

#pragma once

#include "ota_http.h"

#ifdef USE_ARDUINO

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#include <memory>
#include <utility>
#include <string>

#ifdef USE_ESP32
#include <HTTPClient.h>
#endif
#ifdef USE_ESP8266
#include <ESP8266HTTPClient.h>
#ifdef USE_HTTP_REQUEST_ESP8266_HTTPS
#include <WiFiClientSecure.h>
#endif
#endif

namespace esphome {
namespace ota_http {

// void url_firmware_update(std::string url_);
// in HTTPClient.h : #define HTTP_TCP_BUFFER_SIZE (1460)

class OtaHttpArduino : public OtaHttpComponent {
 public:
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  // void set_url(std::string url);
  void set_timeout(uint64_t timeout) { this->timeout_ = timeout; }
  void flash();
};

}  // namespace ota_http
}  // namespace esphome

#endif  // USE_ARDUINO

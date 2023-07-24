#pragma once

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

class OtaHttpComponent : public Component {
 public:
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  void set_url(std::string url);
  void set_timeout(uint64_t timeout) { this->timeout_ = timeout; }
  // void flash(const std::vector<HttpRequestResponseTrigger *> &response_triggers);
  void flash();

 protected:
  std::string url_;
  bool secure_;
  uint64_t timeout_{1000 * 60 * 10};  // must match CONF_TIMEOUT in __init__.py
};

template<typename... Ts> class OtaHttpFlashAction : public Action<Ts...> {
 public:
  OtaHttpFlashAction(OtaHttpComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(std::string, url)
  TEMPLATABLE_VALUE(uint16_t, timeout)

  // void register_response_trigger(HttpRequestResponseTrigger *trigger) { this->response_triggers_.push_back(trigger);
  // }

  void play(Ts... x) override {
    this->parent_->set_url(this->url_.value(x...));
    if (this->timeout_.has_value()) {
      this->parent_->set_timeout(this->timeout_.value(x...));
    }
    // this->parent_->flash(this->response_triggers_);
    this->parent_->flash();
  }

 protected:
  OtaHttpComponent *parent_;
  // std::vector<HttpRequestResponseTrigger *> response_triggers_;
};

}  // namespace ota_http
}  // namespace esphome

#endif  // USE_ARDUINO

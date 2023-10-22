#pragma once

#include "ota_http.h"

#ifdef USE_ESP_IDF

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#include <memory>
#include <utility>
#include <string>

namespace esphome {
namespace ota_http {

class OtaHttpIDF : public OtaHttpComponent {
 public:
  int http_init() override;
  size_t http_read(uint8_t *buf, size_t len) override;
  void http_end() override;
  void cleanup() override;

 protected:
  int x;
};

}  // namespace ota_http
}  // namespace esphome

#endif  // USE_ESP_IDF

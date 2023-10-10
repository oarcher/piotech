
// firmware update from http (ie when OTA port is behind a firewall)
// code adapted from
// esphome/components/ota/ota_backend.cpp
// and
// esphome/components/http_request
#ifdef USE_ARDUINO

#include "ota_http.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/ota/ota_backend.h"
#include "esphome/components/ota/ota_backend_arduino_esp32.h"
#include "esphome/components/ota/ota_backend_arduino_esp8266.h"
#include "esphome/components/ota/ota_backend_arduino_rp2040.h"
#include "esphome/components/ota/ota_backend_esp_idf.h"
#include "esphome/core/application.h"
#include "esphome/components/network/util.h"
#include "esphome/components/md5/md5.h"

namespace esphome {
namespace ota_http {

static const char *const TAG = "ota_http";

void OtaHttpComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OTA_http:");
  ESP_LOGCONFIG(TAG, "  Timeout: %ums", this->timeout_);
}

void OtaHttpComponent::set_url(std::string url) {
  this->url_ = std::move(url);
  this->secure_ = this->url_.rfind("https:", 0) == 0;
}

std::unique_ptr<ota::OTABackend> make_ota_backend() {
#ifdef USE_ESP8266
  ESP_LOGD(TAG, "Using ArduinoESP8266OTABackend");
  return make_unique<ota::ArduinoESP8266OTABackend>();
#endif  // USE_ESP8266
#ifdef USE_ESP32
  ESP_LOGD(TAG, "Using ArduinoESP32OTABackend");
  return make_unique<ota::ArduinoESP32OTABackend>();
#endif  // USE_ESP32
#ifdef USE_ESP_IDF
  ESP_LOGD(TAG, "Using IDFOTABackend");
  return make_unique<ota::IDFOTABackend>();
#endif  // USE_ESP_IDF
#ifdef USE_RP2040
  ESP_LOGD(TAG, "Using ArduinoRP2040OTABackend");
  return make_unique<ota::ArduinoRP2040OTABackend>();
#endif  // USE_RP2040
  ESP_LOGE(TAG, "No OTA backend!");
}

struct Header {
  const char *name;
  const char *value;
};

void OtaHttpComponent::flash() {
  unsigned long update_start_time = millis();
  unsigned long start_time;
  unsigned long duration;
  int http_code;
  const char *headerKeys[] = {"Content-Length", "Content-Type"};
  const size_t headerCount = sizeof(headerKeys) / sizeof(headerKeys[0]);
  const size_t chunk_size = 1024;  // HTTP_TCP_BUFFER_SIZE;
  size_t chunk_start = 0;
  size_t chunk_stop = chunk_size;
  size_t chunk_end;
  uint8_t buf[chunk_size + 1];
  char str_range[40];
  bool update_started = false;
  int error_code = 0;
  unsigned long last_progress = 0;
  size_t body_length;
  size_t bytes_read = 0;
  esphome::md5::MD5Digest md5_receive;
  char *md5_receive_str = new char[33];
  HTTPClient client_{};
  WiFiClient stream;
  std::unique_ptr<ota::OTABackend> backend;

  if (!network::is_connected()) {
    ESP_LOGW(TAG, "Not connected to network");
    return;
  }

#ifdef USE_ESP8266
#ifdef USE_HTTP_REQUEST_ESP8266_HTTPS
  if (this->secure_) {
    ESP_LOGE(TAG, "https connection not handled!");
  }
#endif  // USE_HTTP_REQUEST_ESP8266_HTTPS
#endif  // USE_ESP8266

  ESP_LOGD(TAG, "Trying to connect to %s", this->url_.c_str());

  bool status = false;
#if defined(USE_ESP32)
  status = client_.begin(this->url_.c_str());
#elif defined(USE_ESP8266)
  status = client_.begin(stream, this->url_.c_str());
#endif

  if (!status) {
    ESP_LOGE(TAG, "Unable to make http connection");
    client_.end();
    return;
  } else {
    ESP_LOGV(TAG, "http begin successfull.");
  }

  client_.setReuse(true);
  ESP_LOGVV(TAG, "http client setReuse.");

  // we will compute md5 on the fly
  // TODO: better security if fetched from the http server
  md5_receive.init();
  ESP_LOGV(TAG, "md5sum from received data initialized.");

  // returned needed headers must be collected before the requests
  client_.collectHeaders(headerKeys, headerCount);
  ESP_LOGV(TAG, "http headers collected.");

  // http GET
  start_time = millis();
  http_code = client_.GET();
  duration = millis() - start_time;
  ESP_LOGV(TAG, "http GET finished.");

  if (http_code >= 310) {
    ESP_LOGW(TAG, "HTTP Request failed; URL: %s; Error: %s (%d); Duration: %lu ms", url_.c_str(),
             HTTPClient::errorToString(http_code).c_str(), http_code, duration);
    return;
  }

  body_length = client_.getSize();
  if (client_.getSize() < 0) {
    ESP_LOGE(TAG, "Incorrect file size (%d) reported by http server (http status: %d). Aborting", body_length, http_code);
    return;
  }
  ESP_LOGD(TAG, "firmware is %d bytes length.", body_length);

  // flash memory backend
  backend = make_ota_backend();

  error_code = backend->begin(body_length);
  if (error_code != 0) {
    ESP_LOGW(TAG, "backend->begin error: %d", error_code);
    goto error;  // NOLINT(cppcoreguidelines-avoid-goto)
  }

#ifdef ESP32
  stream = client_.getStream();
  ESP_LOGVV(TAG, "Got esp32 stream");
#endif

  while (bytes_read != body_length) {
    size_t bufsize = std::min(chunk_size, body_length - bytes_read);

    // ESP_LOGVV(TAG, "going to %d bytes at %zu/%zu", bufsize, bytes_read, body_length);

    // ESP_LOGVV(TAG, "waiting for %zu bytes available..", bufsize);
    while (stream.available() < bufsize) {
      // give other tasks a chance to run while waiting for some data:
      // ESP_LOGVV(TAG, "data available: %zu", stream.available());
      yield();
      delay(10);
    }
    // ESP_LOGVV(TAG, "data available: %zu", stream.available());

    stream.readBytes(buf, bufsize);
    bytes_read += bufsize;
    buf[bufsize] = '\0';  // not fed to ota

    md5_receive.add(buf, bufsize);
    // ESP_LOGVV(TAG, "md5 added");

    update_started = true;
    error_code = backend->write(buf, bufsize);
    // ESP_LOGVV(TAG, "wrote to backend");
    if (error_code != 0) {
      // error code explaination available at
      // https://github.com/esphome/esphome/blob/dev/esphome/components/ota/ota_component.h
      ESP_LOGW(TAG, "Error code (%d) writing binary data to flash at offset %d and size %d", error_code, chunk_start,
               body_length);
      goto error;  // NOLINT(cppcoreguidelines-avoid-goto)
    }

    // show progress, and feed watch dog
    unsigned long now = millis();
    if ((now - last_progress > 1000) or (bytes_read == body_length)) {
      last_progress = now;
      ESP_LOGD(TAG, "Progress: %0.1f%%", bytes_read * 100. / body_length);

      // feed watchdog and give other tasks a chance to run
      esphome::App.feed_wdt();
      yield();
    }
  }  // while

  ESP_LOGI(TAG, "Done in %.0f secs", float(millis() - update_start_time) / 1000);

  // send md5 to backend (backend will check that the flashed one has the same)
  md5_receive.calculate();
  md5_receive.get_hex(md5_receive_str);
  ESP_LOGD(TAG, "md5sum recieved: %s (size %d)", md5_receive_str, bytes_read);
  backend->set_update_md5(md5_receive_str);

  client_.end();

  delete[] md5_receive_str;

  // feed watchdog and give other tasks a chance to run
  esphome::App.feed_wdt();
  yield();
  delay(100);

  error_code = backend->end();
  if (error_code != 0) {
    ESP_LOGW(TAG, "Error ending OTA!, error_code: %d", error_code);
    goto error;  // NOLINT(cppcoreguidelines-avoid-goto)
  }

  delay(10);
  ESP_LOGI(TAG, "OTA update finished! Rebooting...");
  delay(100);  // NOLINT
  esphome::App.safe_reboot();
  // new firmware flashed!

error:
  if (update_started) {
    ESP_LOGE(TAG, "Aborted");
    backend->abort();
    return;
  }
}

}  // namespace ota_http
}  // namespace esphome

#endif  // USE_ARDUINO

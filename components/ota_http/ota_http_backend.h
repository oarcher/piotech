#pragma once

#include "esphome/core/defines.h"
#include "esphome/components/ota/ota_backend.h"

namespace esphome {
namespace ota_http {

std::unique_ptr<ota::OTABackend> make_ota_backend();

}  // namespace ota_http
}  // namespace esphome

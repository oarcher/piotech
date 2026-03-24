#pragma once

#include "nulluart.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace nulluart {

class NullUARTOnUpdateTrigger : public Trigger<> {
 public:
  explicit NullUARTOnUpdateTrigger(NullUARTComponent *parent) {
    parent->add_on_update_callback([this]() { this->trigger(); });
  }
};

}  // namespace nulluart
}  // namespace esphome

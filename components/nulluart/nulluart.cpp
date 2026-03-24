#include "nulluart.h"
#include "esphome/core/log.h"
#include <cstring>
#include <cmath>

namespace esphome {
namespace nulluart {

static const char *const TAG = "nulluart";

// UART Interface Implementation
void NullUARTComponent::write_array(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (this->nulluart_buffer_size_ < UART_BUFFER_SIZE) {
      // Buffer has space, write normally
      this->nulluart_buffer_[this->write_ptr_] = data[i];
      this->write_ptr_ = (this->write_ptr_ + 1) % UART_BUFFER_SIZE;
      this->nulluart_buffer_size_++;
    } else {
      // Buffer full, overwrite oldest data (true circular buffer behavior)
      this->nulluart_buffer_[this->write_ptr_] = data[i];
      this->write_ptr_ = (this->write_ptr_ + 1) % UART_BUFFER_SIZE;
      // Move read pointer forward since we're overwriting
      this->read_ptr_ = (this->read_ptr_ + 1) % UART_BUFFER_SIZE;
      // Size stays at max
    }
  }
}

bool NullUARTComponent::read_array(uint8_t *data, size_t len) {
  if (this->available() < len)
    return false;

  for (size_t i = 0; i < len; i++) {
    data[i] = this->nulluart_buffer_[this->read_ptr_];
    this->read_ptr_ = (this->read_ptr_ + 1) % UART_BUFFER_SIZE;
    this->nulluart_buffer_size_--;
  }
  return true;
}

}  // namespace nulluart
}  // namespace esphome

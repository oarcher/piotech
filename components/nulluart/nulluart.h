#pragma once

// ============================================================================
// This component implements a virtual UART (software-only) that can be used
// to bridge data between components without physical UART hardware.
// Current use case: Feed NMEA GPS data from modem AT commands to GPS component
// ============================================================================

#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include <ctime>

namespace esphome
{
  namespace nulluart
  {

    class NullUARTComponent : public uart::UARTComponent, public Component
    {
    public:
      void add_on_update_callback(std::function<void()> &&callback) { this->on_update_callback_.add(std::move(callback)); }

      // UART read/write methods
      void write_array(const uint8_t *data, size_t len) override;
      bool peek_byte(uint8_t *data) override { return false; }
      bool read_array(uint8_t *data, size_t len) override;
      size_t available() override { return this->nulluart_buffer_size_; }
      uart::FlushResult flush() override { return uart::FlushResult::ASSUMED_SUCCESS; }
      void check_logger_conflict() override {}

      float get_setup_priority() const override { return setup_priority::DATA; }

    protected:
      // Circular buffer for UART data
      static constexpr size_t UART_BUFFER_SIZE = 168;
      uint8_t nulluart_buffer_[UART_BUFFER_SIZE] = {0};
      size_t nulluart_buffer_size_{0}; // Current number of bytes in buffer
      size_t read_ptr_{0};
      size_t write_ptr_{0};

      CallbackManager<void()> on_update_callback_;
    };

  } // namespace nulluart
} // namespace esphome

#pragma once

#include <libhal-soft/bit_bang_i2c.hpp>
#include <libhal-util/units.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>

using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::trial_project {

struct hardware_map_t
{
  hal::steady_clock* steady_clock;
  hal::serial*       terminal;
  hal::output_pin*   pin0;   ///< SDA
  hal::output_pin*   pin1;   ///< SCL
  hal::servo*        servo;
  hal::callback<void()> reset;
};

hardware_map_t initialize_platform();
void application(hardware_map_t& p_map);

}  // namespace sjsu::trial_project

#include "../hardware_map.hpp"
#include "../include/mpu6050.hpp"
#include "resource_list.hpp"

#include <libhal-soft/bit_bang_i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/error.hpp>
#include <libhal/units.hpp>

namespace sjsu::trial_project {

void application(hardware_map_t& hardware_map)
{
  using namespace std::literals;

  auto& clock    = *hardware_map.steady_clock;
  auto& terminal = *hardware_map.terminal;
  auto& sda_pin  = *hardware_map.pin0;
  auto& scl_pin  = *hardware_map.pin1;
  auto& servo    = *hardware_map.servo;

  static hal::soft::bit_bang_i2c i2c(
    hal::soft::bit_bang_i2c::pins{
      .sda = &sda_pin,
      .scl = &scl_pin,
    },
    clock);

  auto mpu = mpu6050(clock, i2c);

  constexpr float target_angle = 0.0f;  // change this and recompile

  hal::print<64>(terminal, "Target: %f deg\n", target_angle);
  hal::print(terminal, "Starting stabilization...\n");

  while (true) {
    float roll  = mpu.calculate_roll_angle();
    float error = target_angle - roll;

    hal::print<128>(terminal,
                    "Roll: %f  Target: %f  Error: %f\n",
                    roll,
                    target_angle,
                    error);

    try {
      servo.position(error);
    } catch (hal::argument_out_of_domain&) {
      hal::print(terminal, "Error exceeds servo range!\n");
    }

    hal::delay(clock, 10ms);
  }
}

}  // namespace sjsu::trial_project

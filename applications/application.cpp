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

  // NOTE: hal::soft::bit_bang_i2c is the correct modern libhal type.
  // The old hal::bit_bang_i2c no longer exists.
  static hal::soft::bit_bang_i2c i2c(
    hal::soft::bit_bang_i2c::pins{
      .sda = &sda_pin,
      .scl = &scl_pin,
    },
    clock);

  auto mpu = mpu6050(clock, i2c);

  hal::print(terminal, "Self-leveling started!\n");

  servo.position(0);

  while (true) {
    float angle = mpu.calculate_roll_angle();

    hal::print<128>(terminal,
                    "Accel X: %f  Y: %f  Z: %f  |  Roll Angle: %f deg\n",
                    mpu.accel.accelx,
                    mpu.accel.accely,
                    mpu.accel.accelz,
                    angle);

    try {
      servo.position(angle);
    } catch (hal::argument_out_of_domain&) {
      hal::print(terminal, "Servo angle out of range!\n");
    }

    hal::delay(clock, 10ms);
  }
}

}  // namespace sjsu::trial_project

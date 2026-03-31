// Copyright 2024 Khalil Estell
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "hardware_map.hpp"
#include "resource_list.hpp"

#include <libhal-actuator/rc_servo.hpp>
#include <libhal-expander/pca9685.hpp>
#include <libhal-micromod/micromod.hpp>
#include <libhal-soft/bit_bang_i2c.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::trial_project {

hardware_map_t initialize_platform()
{
  using namespace hal::literals;
  using namespace std::chrono_literals;

  static auto& counter  = hal::micromod::v1::uptime_clock();
  static auto& terminal = hal::micromod::v1::console(hal::buffer<1024>);
  hal::print(terminal, "created terminal and counter\n");

  static auto& sda = hal::micromod::v1::output_g0();
  static auto& scl = hal::micromod::v1::output_g1();
  hal::print(terminal, "created g0 and g1\n");

  static hal::soft::bit_bang_i2c i2c({ .sda = &sda, .scl = &scl }, counter);
  hal::print(terminal, "created i2c\n");

  static hal::expander::pca9685 pca9685(i2c, 0b100'0000);
  static auto pwm0 = pca9685.get_pwm_channel<0>();
  hal::print(terminal, "created pca9685 and got pwm channel\n");

  static hal::actuator::rc_servo servo(pwm0, {
    .frequency        = 50,
    .min_angle        = -90,
    .max_angle        = 90,
    .min_microseconds = 1000,
    .max_microseconds = 2000,
  });
  hal::print(terminal, "created servo\n");

  return hardware_map_t{
    .steady_clock = &counter,
    .terminal     = &terminal,
    .pin0         = &sda,
    .pin1         = &scl,
    .servo        = &servo,
    .reset        = +[]() { hal::micromod::v1::reset(); },
  };
}

}  // namespace sjsu::trial_project

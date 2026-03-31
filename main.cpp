#include "hardware_map.hpp"   // included once

#include <libhal-util/serial.hpp>
#include <libhal/error.hpp>

sjsu::trial_project::hardware_map_t hardware_map{};

int main()
{
  try {
    hardware_map = sjsu::trial_project::initialize_platform();
  } catch (...) {
    hal::halt();
  }

  sjsu::trial_project::application(hardware_map);

  return 0;
}

#pragma once

#include <cstdint>

#include <libhal-soft/bit_bang_i2c.hpp>
#include <libhal-util/i2c.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

namespace sjsu::trial_project {

class mpu6050
{
public:
  // I2C register addresses for the MPU-6050
  enum addresses : hal::byte
  {
    device_address        = 0x68,  ///< Default I2C device address (AD0 = LOW)
    power_address         = 0x6B,  ///< PWR_MGMT_1 register
    accelerometer_config  = 0x1C,  ///< ACCEL_CONFIG register
    accel_xout_first_half = 0x3B,  ///< ACCEL_XOUT_H (burst-read start)
  };

  struct acceleration
  {
    float accelx = 0.0f;
    float accely = 0.0f;
    float accelz = 0.0f;
  };

  mpu6050(hal::steady_clock& p_clock, hal::soft::bit_bang_i2c& p_i2c);

  /// Read all three accelerometer axes and store in `accel`
  void read_accel();

  /// Return the roll angle in degrees computed from `accel`
  float calculate_roll_angle();

  acceleration accel;

private:
  hal::steady_clock&        m_clock;
  hal::soft::bit_bang_i2c&  m_i2c;
};

}  // namespace sjsu::trial_project

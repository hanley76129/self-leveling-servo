#include "../include/mpu6050.hpp"

#include <cmath>

#include <libhal-soft/bit_bang_i2c.hpp>
#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::trial_project {

constexpr double PI = 3.14159265358979323846;

mpu6050::mpu6050(hal::steady_clock& p_clock, hal::soft::bit_bang_i2c& p_i2c)
  : m_clock(p_clock)
  , m_i2c(p_i2c)
{
  // Wake the MPU-6050 by clearing the SLEEP bit in PWR_MGMT_1 (reg 0x6B)
  std::array<hal::byte, 2> power_settings = {
    addresses::power_address, 0x00
  };
  hal::write(m_i2c, addresses::device_address, power_settings);
}

void mpu6050::read_accel()
{
  // Point to the first accelerometer data register (ACCEL_XOUT_H = 0x3B)
  std::array<hal::byte, 1> reg = { addresses::accel_xout_first_half };
  std::array<hal::byte, 6> buffer;

  // Burst-read 6 bytes: X_H, X_L, Y_H, Y_L, Z_H, Z_L
  hal::write_then_read(m_i2c, addresses::device_address, reg, buffer);

  // Combine high and low bytes, then scale to g's (±2g range → 16384 LSB/g)
  accel.accelx = static_cast<int16_t>((buffer[0] << 8) | buffer[1]) / 16384.0f;
  accel.accely = static_cast<int16_t>((buffer[2] << 8) | buffer[3]) / 16384.0f;
  accel.accelz = static_cast<int16_t>((buffer[4] << 8) | buffer[5]) / 16384.0f;
}

float mpu6050::calculate_roll_angle()
{
  read_accel();

  // Roll = atan2(Y, sqrt(X^2 + Z^2)), converted to degrees
  float roll = std::atan(accel.accely /
                         std::sqrt(std::pow(accel.accelx, 2) +
                                   std::pow(accel.accelz, 2)))
               * 180.0f / static_cast<float>(PI);

  return roll;
}

}  // namespace sjsu::trial_project

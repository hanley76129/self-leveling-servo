# Self-Leveling Servo

Firmware for a self-leveling servo system built on STM32 using
[libhal](https://github.com/libhal/libhal). The servo automatically adjusts
its position to maintain a level orientation as the board tilts.

## Overview

The system runs on an STM32 microcontroller and is responsible for:
- Reading accelerometer data from an MPU-6050 over bit-bang I2C
- Computing the roll angle from raw accelerometer data
- Driving a servo to counteract the measured tilt in real time

## How It Works

### Bit-Bang I2C
The target STM32 lacked hardware I2C peripherals, so I2C was implemented
in software using `hal::soft::bit_bang_i2c` with two GPIO output pins.

During development, the MPU-6050 was silently failing to acknowledge I2C
transactions. Capturing the bus with a logic analyzer revealed that SDA was
idling low instead of high — a violation of the open-drain I2C specification.
Fixing the pin initialization restored reliable communication.

### MPU-6050 Driver
A custom driver wakes the MPU-6050 by clearing the SLEEP bit in PWR_MGMT_1,
then burst-reads 6 bytes of accelerometer data starting at register 0x3B.
Raw values are scaled to g-forces using the ±2g range (16384 LSB/g).

### Roll Angle Calculation
Roll angle is computed from the accelerometer axes using:
```
roll = atan(Y / sqrt(X² + Z²)) × (180 / π)
```

### Servo Control
The computed roll angle is passed directly to the servo as a position command,
closing the feedback loop. Out-of-range angles are caught and logged to the
terminal.

## Hardware
- STM32 microcontroller
- MPU-6050 IMU (accelerometer)
- RC servo (1000-2000 µs pulse range)
- PCA9685 PWM expander (used to generate servo PWM over I2C)

## Build
```bash
conan build . -pr stm32f103c8 -pr arm-gcc-14.2 -b missing
```

## Dependencies
Built with [libhal](https://github.com/libhal/libhal) using CMake and Conan.
- `libhal-soft` — bit-bang I2C
- `libhal-actuator` — servo driver
- `libhal-util` — I2C helpers, serial printing

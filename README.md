# CERAM

Cost Effective Rocket Aviation Module.

CERAM is a flight computer board for model rocketry, built around a simple idea: instead of hand soldering a big board full of tiny SMD parts, you plug in cheap breakout boards that cost a couple of dollars each. The board just wires them together.

![CERAM board top view](images/1.png)

## Why

Making a rocket flight computer usually means picking one of two paths. Either you buy a commercial unit and spend a lot of money, or you design your own PCB with a bare microcontroller, an IMU, a barometer and a pile of passives, then try to hand solder QFN packages with a normal iron. That second path is a weekend of frustration, one spool of solder wick, and usually a dead board or two.

## What is on it

CERAM is a carrier board. What goes on top of it defines its actual use.


- A Seeed Studio Xiao ESP32-S3 board serving as the MCU of the entire stack
- An MPU-6050 or other compatible IMU
- A BMP280 or other compatible barometer
- 4 PWM outputs
- A UART Header
- An I2C Header
- A LM317 for voltage regulation
- Two IRLZ44N MOSFETs for parachute deployment/reaction wheel driving/motor igniting/pyrotechnic uses

![Board with everything labeled](images/2.png)

## BOM

| Part | Link | Cost |
|---|---|---|
| tbd | tbd| tbd| 

## Software
Included is a board test program (`firmware/BoardTest`) that initializes the IMU, barometer, and servos and streams all sensor readings over serial.

## License

MIT. See LICENSE.

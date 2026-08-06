#include <Wire.h>
#include "MPU6050.h"
#include "StateEstimator.h"
#include "SG90.h"
#include "BMP180.h"
#include "PID.h"
#include "esp32-hal-gpio.h"

MPU6050 imu;
BMP180 baro;

SG90 xservo;
SG90 yservo;

PID xPID(0.4, 0, 0.2);
PID yPID(0.4, 0, 0.2);

MPU6050Data i;
BMP180Data b;

StateEstimator state;

uint32_t lastTime = 0;
uint32_t lastLed = 0;
bool ledState = false;

const uint32_t PERIOD_US = 10000;

void setup() {
	Serial.begin(115200);
	Wire.begin(6, 7);

	imu.init(0x68);
	baro.init(0x77);

	delay(100);
	imu.calibrate(500, 10); // 500 samples @ 10ms
	Serial.printf("IMU drift: {gx=%f, gy=%f, gz=%f}\n", imu.drift.gx, imu.drift.gy, imu.drift.gz);

	xservo.initServo(9); // INNER
	yservo.initServo(2); // OUTER
	pinMode(4, OUTPUT);

	lastTime = micros();
}

void loop() {
	uint32_t now = micros();
	if (now - lastTime >= PERIOD_US) {
		float actual_ms = (now - lastTime) / 1000.0f;
		lastTime += PERIOD_US;

		// IMU: accelerometer + gyro + temperature
		i = imu.getData();

		// Barometer: temperature + pressure + altitude
		b = baro.getData();

		// Fuse gyro rates + baro altitude into an attitude estimate
		state.updateState({i.gx, i.gy, i.gz}, b.altitude, actual_ms);
		Orientation att = state.getOrientation() * RAD_TO_DEG;

		// Level the board: PID tries to drive tilt back to 0
		float xAngle = att.y;
		float yAngle = -att.z; // due to mpu mounting

		int xCommand = (int)xPID.updateLoop(xAngle, 0, 0.01); // state=xAngle, setpoint 0, dt = 10ms
		int yCommand = (int)yPID.updateLoop(yAngle, 0, 0.01); // state=yAngle, setpoint 0, dt = 10ms
		xCommand = constrain(xCommand, -30, 30);
		yCommand = constrain(yCommand, -30, 30);

		xservo.setPosition(90 + xCommand);
		yservo.setPosition(90 + yCommand);

		Serial.printf("t=%lu ms | accel {%f, %f, %f} m/s2 | gyro {%f, %f, %f} deg/s | imu_temp %f C | baro %f C, %f hPa, %f m | att r=%f p=%f y=%f | servo cmd=%d,%d\n",
				now / 1000,
				i.ax, i.ay, i.az,
				i.gx, i.gy, i.gz,
				i.temp,
				b.temperature, b.pressure, b.altitude,
				att.x, att.y, att.z,
				xCommand, yCommand);
	}

	// Heartbeat on pin 4 (LED) every 500ms
	if (now - lastLed >= 500000) {
		lastLed = now;
		ledState = !ledState;
		digitalWrite(4, ledState);
	}
}

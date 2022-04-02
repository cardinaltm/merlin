/**
 *******************************************************************************
 * @file   flight.c
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "flight.h"
#include "usart.h"
#include "tim.h"
#include "sensors.h"

/*
 const float pidPGainRoll = 1.3;
 const float pidIGainRoll = 0.04;
 const float pidDGainRoll = 18.0;
 const int pidMaxRoll = 400;

 const float pidPGainPitch = pidPGainRoll;
 const float pidIGainPitch = pidIGainRoll;
 const float pidDGainPitch = pidDGainRoll;
 const int pidMaxPitch = pidMaxRoll;

 const float pidPGainYaw = 4.0;
 const float pidIGainYaw = 0.02;
 const float pidDGainYaw = 0.0;
 const int pidMaxYaw = 400;
 */

// Setup PID
const float pidPGainRoll = 0.55;
const float pidIGainRoll = 0.85;
const float pidDGainRoll = 0.39;
const int pidMaxRoll = 400;

const float pidPGainPitch = 0.60;
const float pidIGainPitch = 0.90;
const float pidDGainPitch = 0.42;
const int pidMaxPitch = 400;

const float pidPGainYaw = 0.59;
const float pidIGainYaw = 0.90;
const float pidDGainYaw = 0.0;
const int pidMaxYaw = 400;

int8_t autoLevel = 1;

// Original Inputs
long accelTotalVector;
double inputGyroPitch, inputGyroRoll, inputGyroYaw;
int inputReceiverInputChannel1 = 1500, inputReceiverInputChannel2 = 1500, inputReceiverInputChannel3 = 1400, inputReceiverInputChannel4 = 1500;

int esc1, esc2, esc3, esc4;
int throttle;
//int battery_voltage;
int start;
float rollLevelAdjust, pitchLevelAdjust;
float pidErrorTemp;
float pidIMemRoll, pidRollSetPoint, gyroRollInput, pidOutputRoll, pidLastRollDError;
float pidIMemPitch, pidPitchSetPoint, gyroPitchInput, pidOutputPitch, pidLastPitchDError;
float pidIMemYaw, pidYawSetPoint, gyroYawInput, pidOutputYaw, pidLastYawDError;
float angleRollAcc, anglePitchAcc, anglePitch, angleRoll;

void Flight_Init(void)
{
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // ESC_FRONT_RIGHT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // ESC_BACK_RIGHT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // ESC_BACK_LEFT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // ESC_FRONT_LEFT

// Starts the PWM signal generation
	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
	{
		/* PWM Generation Error */
		printf("Motors not found\r\n");
		Error_Handler();
	}
	printf("Motors Found\r\n");

	HAL_Delay(100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1000);

	printf("Motors Initialize\r\n");
	HAL_Delay(1000);

	start = 0; // Set start back to zero.

	// Let's take multiple gyro data samples so we can determine the average gyro offset (calibration).
	// TODO: calibrate gyroscope, make 2000 records, delay 3ms, turn on led

	// Now that we have 2000 measures, we need to divide by 2000 to get the average gyro offset.
	// TODO: divide and get average

// Wait until the receiver is active and the throtle is set to the lower position.
//	while (inputReceiverInputChannel3 < 990 || inputReceiverInputChannel3 > 1020 || inputReceiverInputChannel4 < 1400)
//	{
//		inputReceiverInputChannel3 = convert_receiver_channel(3);                 // Convert the actual receiver signals for throttle to the standard 1000 - 2000us
//		inputReceiverInputChannel4 = convert_receiver_channel(4);                 // Convert the actual receiver signals for yaw to the standard 1000 - 2000us
//		start++;                                                               // While waiting increment start whith every loop.
//		// We don't want the esc's to be beeping annoyingly. So let's give them a 1000us puls while waiting for the receiver inputs.
//		// Set digital poort 4, 5, 6 and 7 low.
//		HAL_Delay(3);                                                               // Wait 3 milliseconds before the next loop.
//	}
	start = 0;                                                                // Set start back to 0.

	// Load the battery voltage to the battery_voltage variable.
	// 65 is the voltage compensation for the diode.
	// 12.6V equals ~5V @ Analog 0.
	// 12.6V equals 1023 analogRead(0).
	// 1260 / 1023 = 1.2317.
	// The variable battery_voltage holds 1050 if the battery voltage is 10.5V.
//	battery_voltage = (analogRead(0) + 65) * 1.2317;
}

void Flight_Control(void)
{
	// 65.5 = 1 deg/sec (check the datasheet of the MPU-6050 for more information).
	// Gyro pid input is deg/sec.
	gyroRollInput = (gyroRollInput * 0.7) + ((inputGyroRoll / 65.5) * 0.3);
	gyroPitchInput = (gyroPitchInput * 0.7) + ((inputGyroPitch / 65.5) * 0.3);
	gyroYawInput = (gyroYawInput * 0.7) + ((inputGyroYaw / 65.5) * 0.3);

	// Gyro angle calculations
	// 0.0000611 = 1 / (250Hz / 65.5)
	anglePitch += inputGyroPitch * 0.0000611;  // Calculate the traveled pitch angle and add this to the anglePitch variable.
	angleRoll += inputGyroRoll * 0.0000611;  // Calculate the traveled roll angle and add this to the angleRoll variable.

	// 0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
	anglePitch -= angleRoll * sin(inputGyroYaw * 0.000001066); // If the IMU has yawed transfer the roll angle to the pitch angel.
	angleRoll += anglePitch * sin(inputGyroYaw * 0.000001066); // If the IMU has yawed transfer the pitch angle to the roll angel.

	// Accelerometer angle calculations
	accelTotalVector = sqrt((sensorAcc.X * sensorAcc.X) + (sensorAcc.Y * sensorAcc.Y) + (sensorAcc.Z * sensorAcc.Z)); // Calculate the total accelerometer vector.

	// Prevent the asin function to produce a NaN
	if (abs(sensorAcc.Y) < accelTotalVector)
	{
		// Calculate the pitch angle.
		anglePitchAcc = asin((float) sensorAcc.Y / accelTotalVector) * 57.296;
	}

	// Prevent the asin function to produce a NaN
	if (abs(sensorAcc.X) < accelTotalVector)
	{
		// Calculate the roll angle.
		angleRollAcc = asin((float) sensorAcc.X / accelTotalVector) * -57.296;
	}

	// Place the MPU-6050 spirit level and note the values in the following two lines for calibration.
	anglePitchAcc -= 0.0;  // Accelerometer calibration value for pitch.
	angleRollAcc -= 0.0; // Accelerometer calibration value for roll.

	anglePitch = anglePitch * 0.9996 + anglePitchAcc * 0.0004; // Correct the drift of the gyro pitch angle with the accelerometer pitch angle.
	angleRoll = angleRoll * 0.9996 + angleRollAcc * 0.0004; // Correct the drift of the gyro roll angle with the accelerometer roll angle.

	pitchLevelAdjust = anglePitch * 15; // Calculate the pitch angle correction
	rollLevelAdjust = angleRoll * 15; // Calculate the roll angle correction

	// If the quadcopter is not in auto-level mode
	if (autoLevel == 0)
	{
		pitchLevelAdjust = 0; // Set the pitch angle correction to zero.
		rollLevelAdjust = 0; // Set the roll angle correcion to zero.
	}

	// For starting the motors: throttle low and yaw left (step 1).
	if (inputReceiverInputChannel3 < 1050 && inputReceiverInputChannel4 < 1050)
	{
		start = 1;
	}

	// When yaw stick is back in the center position start the motors (step 2).
	if (start == 1 && inputReceiverInputChannel3 < 1050 && inputReceiverInputChannel4 > 1450)
	{
		start = 2;

		anglePitch = anglePitchAcc; // Set the gyro pitch angle equal to the accelerometer pitch angle when the quadcopter is started.
		angleRoll = angleRollAcc; // Set the gyro roll angle equal to the accelerometer roll angle when the quadcopter is started.

		// Reset the PID controllers for a bump less start.
		pidIMemRoll = 0;
		pidLastRollDError = 0;
		pidIMemPitch = 0;
		pidLastPitchDError = 0;
		pidIMemYaw = 0;
		pidLastYawDError = 0;
	}

	// Stopping the motors: throttle low and yaw right.
	if (start == 2 && inputReceiverInputChannel3 < 1050 && inputReceiverInputChannel4 > 1950)
		start = 0;

	// The PID set point in degrees per second is determined by the roll receiver input.
	// In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
	pidRollSetPoint = 0;
	// We need a little dead band of 16us for better results.
	if (inputReceiverInputChannel1 > 1508)
	{
		pidRollSetPoint = inputReceiverInputChannel1 - 1508;
	}
	else if (inputReceiverInputChannel1 < 1492)
	{
		pidRollSetPoint = inputReceiverInputChannel1 - 1492;
	}

	pidRollSetPoint -= rollLevelAdjust; // Subtract the angle correction from the standardized receiver roll input value.
	pidRollSetPoint /= 3.0; // Divide the setpoint for the PID roll controller by 3 to get angles in degrees.

	// The PID set point in degrees per second is determined by the pitch receiver input.
	// In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
	pidPitchSetPoint = 0;
	// We need a little dead band of 16us for better results.
	if (inputReceiverInputChannel2 > 1508)
	{
		pidPitchSetPoint = inputReceiverInputChannel2 - 1508;
	}
	else if (inputReceiverInputChannel2 < 1492)
	{
		pidPitchSetPoint = inputReceiverInputChannel2 - 1492;
	}

	pidPitchSetPoint -= pitchLevelAdjust; // Subtract the angle correction from the standardized receiver pitch input value.
	pidPitchSetPoint /= 3.0; // Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.

	// The PID set point in degrees per second is determined by the yaw receiver input.
	// In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
	pidYawSetPoint = 0;
	// We need a little dead band of 16us for better results.
	if (inputReceiverInputChannel3 > 1050)
	{ // Do not yaw when turning off the motors.
		if (inputReceiverInputChannel4 > 1508)
		{
			pidYawSetPoint = (inputReceiverInputChannel4 - 1508) / 3.0;
		}
		else if (inputReceiverInputChannel4 < 1492)
		{
			pidYawSetPoint = (inputReceiverInputChannel4 - 1492) / 3.0;
		}
	}

	// PID inputs are known. So we can calculate the PID output.
	calculatePID();

	// The battery voltage is needed for compensation.
	// A complementary filter is used to reduce noise.
	// 0.09853 = 0.08 * 1.2317.
//	battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;

	// Turn on the led if battery voltage is to low.
//	if (battery_voltage < 1000 && battery_voltage > 600)
//		digitalWrite(12, HIGH);

	throttle = inputReceiverInputChannel3; // We need the throttle signal as a base signal.

	// The motors are started.
//	if (start == 2)
//	{
	if (throttle > 1800)
	{
		// We need some room to keep full control at full throttle.
		throttle = 1800;
	}

	esc1 = throttle - pidOutputPitch + pidOutputRoll - pidOutputYaw; // Calculate the pulse for esc 1 (front-right - CCW)
	esc2 = throttle + pidOutputPitch + pidOutputRoll + pidOutputYaw; // Calculate the pulse for esc 2 (rear-right - CW)
	esc3 = throttle + pidOutputPitch - pidOutputRoll - pidOutputYaw; // Calculate the pulse for esc 3 (rear-left - CCW)
	esc4 = throttle - pidOutputPitch - pidOutputRoll + pidOutputYaw; // Calculate the pulse for esc 4 (front-left - CW)

	// Is the battery connected?
//		if (battery_voltage < 1240 && battery_voltage > 800)
//		{
//			// Compensate the esc-1/2/3/4 pulse for voltage drop.
//			esc1 += esc1 * ((1240 - battery_voltage) / (float) 3500);
//			esc2 += esc2 * ((1240 - battery_voltage) / (float) 3500);
//			esc3 += esc3 * ((1240 - battery_voltage) / (float) 3500);
//			esc4 += esc4 * ((1240 - battery_voltage) / (float) 3500);
//		}

	// Keep the motors running.
	if (esc1 < 1100)
		esc1 = 1100;
	if (esc2 < 1100)
		esc2 = 1100;
	if (esc3 < 1100)
		esc3 = 1100;
	if (esc4 < 1100)
		esc4 = 1100;

	// Limit the esc-1/2/3/4 pulse to 2000us.
	if (esc1 > 2000)
		esc1 = 2000;
	if (esc2 > 2000)
		esc2 = 2000;
	if (esc3 > 2000)
		esc3 = 2000;
	if (esc4 > 2000)
		esc4 = 2000;
//	}
//	else
//	{
//		// If start is not 2 keep a 1000us pulse for ess-1/2/3/4.
//		esc1 = 1000;
//		esc2 = 1000;
//		esc3 = 1000;
//		esc4 = 1000;
//	}

	// Calculate the time of the falling edge of the esc-1/2/3/4 pulse.
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, esc1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, esc2);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, esc3);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, esc4);

	// DEBUG
//	printf("MOTORS - nesc1: %u, esc2: %u, esc3: %u, esc4: %u\r\n", esc1, esc2, esc3, esc4);
}

void calculatePID(void)
{
	// Roll calculations
	pidErrorTemp = gyroRollInput - pidRollSetPoint;
	pidIMemRoll += pidIGainRoll * pidErrorTemp;
	if (pidIMemRoll > pidMaxRoll)
		pidIMemRoll = pidMaxRoll;
	else if (pidIMemRoll < pidMaxRoll * -1)
		pidIMemRoll = pidMaxRoll * -1;

	pidOutputRoll = pidPGainRoll * pidErrorTemp + pidIMemRoll + pidDGainRoll * (pidErrorTemp - pidLastRollDError);
	if (pidOutputRoll > pidMaxRoll)
		pidOutputRoll = pidMaxRoll;
	else if (pidOutputRoll < pidMaxRoll * -1)
		pidOutputRoll = pidMaxRoll * -1;

	pidLastRollDError = pidErrorTemp;

	// Pitch calculations
	pidErrorTemp = gyroPitchInput - pidPitchSetPoint;
	pidIMemPitch += pidIGainPitch * pidErrorTemp;
	if (pidIMemPitch > pidMaxPitch)
		pidIMemPitch = pidMaxPitch;
	else if (pidIMemPitch < pidMaxPitch * -1)
		pidIMemPitch = pidMaxPitch * -1;

	pidOutputPitch = pidPGainPitch * pidErrorTemp + pidIMemPitch + pidDGainPitch * (pidErrorTemp - pidLastPitchDError);
	if (pidOutputPitch > pidMaxPitch)
		pidOutputPitch = pidMaxPitch;
	else if (pidOutputPitch < pidMaxPitch * -1)
		pidOutputPitch = pidMaxPitch * -1;

	pidLastPitchDError = pidErrorTemp;

	// Yaw calculations
	pidErrorTemp = gyroYawInput - pidYawSetPoint;
	pidIMemYaw += pidIGainYaw * pidErrorTemp;
	if (pidIMemYaw > pidMaxYaw)
		pidIMemYaw = pidMaxYaw;
	else if (pidIMemYaw < pidMaxYaw * -1)
		pidIMemYaw = pidMaxYaw * -1;

	pidOutputYaw = pidPGainYaw * pidErrorTemp + pidIMemYaw + pidDGainYaw * (pidErrorTemp - pidLastYawDError);
	if (pidOutputYaw > pidMaxYaw)
		pidOutputYaw = pidMaxYaw;
	else if (pidOutputYaw < pidMaxYaw * -1)
		pidOutputYaw = pidMaxYaw * -1;

	pidLastYawDError = pidErrorTemp;
}


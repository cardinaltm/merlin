/**
 *******************************************************************************
 * @file   receiver.c
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 4, 2022
 *******************************************************************************
 **/

#include "receiver.h"
#include "tim.h"

Receiver receiver;

void Receiver_Init(void)
{
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // RECEIVER_THROTTLE
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2); // RECEIVER_YAW
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3); // RECEIVER_PITCH
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4); // RECEIVER_ROLL

	receiver.Throttle = 1000;
	receiver.Pitch = 1500;
	receiver.Roll = 1500;
	receiver.Yaw = 1500;

	printf("Initialize Receivers\r\n");
}

uint8_t throttleFirstCapture, yawFirstCapture, pitchFirstCapture, rollFirstCapture;
uint8_t throttleValue1, throttleValue2;
uint8_t yawValue1, yawValue2;
uint8_t pitchValue1, pitchValue2;
uint8_t rollValue1, rollValue2;

//					throttlePercent = ((receiverThrottleDifference - receiverThrottleMin) / (receiverThrottleMax - receiverThrottleMin)) * 100;
//
//					float motorThrottleWid = ((motorThrottleMax - motorThrottleMin) * (throttlePercent / 100)) + motorThrottleMin;
//					htim1.Instance->CCR1 = motorThrottleWid; // MOTOR_FRONT_RIGHT
//					htim1.Instance->CCR2 = motorThrottleWid; // MOTOR_BACK_RIGHT
//					htim1.Instance->CCR3 = motorThrottleWid; // MOTOR_FRONT_LEFT
//					htim1.Instance->CCR4 = motorThrottleWid; // MOTOR_BACK_LEFT
void ReceiverCaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		switch (htim->Channel)
		{
		// RECEIVER_THROTTLE
		case HAL_TIM_ACTIVE_CHANNEL_1:
			if (throttleFirstCapture == 0) // if the first value is not captured
			{
				throttleValue1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				throttleFirstCapture = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}

			else if (throttleFirstCapture == 1)   // if the first is already captured
			{
				throttleValue2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);

				if (throttleValue2 > throttleValue1)
				{
					receiver.Throttle = throttleValue2 - throttleValue1;
				}

				throttleFirstCapture = 0;
				// set polarity to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			break;
			// RECEIVER_YAW
		case HAL_TIM_ACTIVE_CHANNEL_2:
			if (yawFirstCapture == 0) // if the first value is not captured
			{
				yawValue1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				yawFirstCapture = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}

			else if (yawFirstCapture == 1)   // if the first is already captured
			{
				yawValue2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);

				if (yawValue2 > yawValue1)
				{
					receiver.Yaw = yawValue2 - yawValue1;
				}

				yawFirstCapture = 0;
				// set polarity to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			break;
			// RECEIVER_PITCH
		case HAL_TIM_ACTIVE_CHANNEL_3:
			if (pitchFirstCapture == 0) // if the first value is not captured
			{
				pitchValue1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				pitchFirstCapture = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}

			else if (pitchFirstCapture == 1)   // if the first is already captured
			{
				pitchValue2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);

				if (pitchValue2 > pitchValue1)
				{
					receiver.Pitch = pitchValue2 - pitchValue1;
				}

				pitchFirstCapture = 0;
				// set polarity to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			break;
			// RECEIVER_ROLL
		case HAL_TIM_ACTIVE_CHANNEL_4:
			if (rollFirstCapture == 0) // if the first value is not captured
			{
				rollValue1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				rollFirstCapture = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}

			else if (rollFirstCapture == 1)   // if the first is already captured
			{
				rollValue2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);

				if (rollValue2 > rollValue1)
				{
					receiver.Roll = rollValue2 - rollValue1;
				}

				rollFirstCapture = 0;
				// set polarity to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
			break;
		default:
			break;
		}
	}
}

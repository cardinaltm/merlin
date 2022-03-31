/*
 * flight.c
 *
 *  Created on: Mar 31, 2022
 *      Author: cardinal
 */

#include "flight.h"
#include "usart.h"

void FC_Init()
{
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // MOTOR_FRONT_RIGHT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // MOTOR_BACK_RIGHT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // MOTOR_FRONT_LEFT
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // MOTOR_BACK_LEFT

// Starts the PWM signal generation
	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK || HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}

	printf("------- Initialize Motors -------\r\n");
	HAL_Delay(1000);

	// set motors to 0
	motorFrontRightSpeed = motorBackRightSpeed = motorFrontLeftSpeed = motorBackLeftSpeed = BLDC_MIN_PWM;

	// инициализация нечеткого регулятора
	Fuzzy_Init();
	fsmState = FSM_NULL;
	HAL_Delay(1000);
}

/*
 * @brief основная функция абсу
 */
void FlightControl()
{
	if ((HAL_GetTick() - uiFC_TicksCNT) < 20)
		return;

	uiFC_TicksCNT = HAL_GetTick();

	switch (fsmState)
	{
	case FSM_TAKEOFF:
		setRoll(0);
		stallSpeed = 1500;

		// TODO: барометром или дальнометром поднять флаг взлета
		// if(isTakeOFF) // высота 1м
		fsmState = FSM_HANGING;
		// else stallSpeed += 10;

		motorFrontRightSpeed = motorBackRightSpeed = motorFrontLeftSpeed = motorBackLeftSpeed = stallSpeed;
		break;
	case FSM_HANGING:
		selfStabilizing();
		break;
	default:
		break;
	}

	// применяем исменения к моторам
	motorDriver();

}

/*
 * @brief управление моторами
 */
void motorDriver()
{
	motorFrontRightSpeed = constrain(motorFrontRightSpeed, BLDC_MIN_PWM, BLDC_MAX_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorFrontRightSpeed);

	motorBackRightSpeed = constrain(motorBackRightSpeed, BLDC_MIN_PWM, BLDC_MAX_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorBackRightSpeed);

	motorFrontLeftSpeed = constrain(motorFrontLeftSpeed, BLDC_MIN_PWM, BLDC_MAX_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, motorFrontLeftSpeed);

	motorBackLeftSpeed = constrain(motorBackLeftSpeed, BLDC_MIN_PWM, BLDC_MAX_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, motorBackLeftSpeed);
}

/*
 * @brief процедура стабилицазий бпла
 */
void selfStabilizing()
{
	// пока работаем только с креном
	// TODO: добавить ограничения, проверить уже на модели на критических углах
	int16_t _iRollError = (int16_t) ((_acRoll - RP_Data.aRoll) * 4); // 125/31 * error 31'+ - макцимальная ошибка
	// приводим к интервалу [-125, 125]
	int8_t iRollError = constrain(_iRollError, -125, 125);
	int8_t iDeltaError = constrain(iRollError - iprevRollError, -125, 125);
	// храним текушую ошибку для составления первой разницы
	iprevRollError = iRollError;

	// узнаем нечеткий вывод и переводим его в шим для двигателья
	double dMTh = getFuzzyConclusion(iRollError, iDeltaError) * 8; // 8 = 1000/125 - максимальное исправление в 1000 при...

	// теперь нужно подстроить скорости врашения двигателья
	// желательно без смены высоты
	int16_t iPart = (int16_t) (dMTh * 0.5);

	motorFrontRightSpeed = stallSpeed + iPart;
	motorBackRightSpeed = stallSpeed + iPart;
	motorFrontLeftSpeed = stallSpeed - iPart;
	motorBackLeftSpeed = stallSpeed - iPart;

	// проверка для невылета за максимальный диапазон
	if (motorFrontLeftSpeed > BLDC_MAX_PWM)
	{
		int16_t _d = motorFrontLeftSpeed - BLDC_MAX_PWM;
		motorFrontLeftSpeed = BLDC_MAX_PWM;
		motorFrontRightSpeed -= _d;
	}
	if (motorFrontRightSpeed > BLDC_MAX_PWM)
	{
		int16_t _d = motorFrontRightSpeed - BLDC_MAX_PWM;
		motorFrontRightSpeed = BLDC_MAX_PWM;
		motorFrontLeftSpeed -= _d;
	}

	printf("iMTh: %i iRollError %i motorFrontRightSpeed: %i motorBackRightSpeed: %i motorFrontLeftSpeed: %i motorBackLeftSpeed: %i\r\n", (int16_t) (floor(dMTh)), (int16_t) (floor(iRollError)), (int16_t) (floor(motorFrontRightSpeed)), (int16_t) (floor(motorBackRightSpeed)), (int16_t) (floor(motorFrontLeftSpeed)), (int16_t) (floor(motorBackLeftSpeed)));
}

/*
 * @brief Set the Pitch angle
 *
 * @param aPitch
 */
void setPitch(float aPitch)
{
	_acPitch = aPitch;
}

/*
 * @brief Set the Roll angle
 *
 * @param aRoll
 */
void setRoll(float aRoll)
{
	_acRoll = aRoll;
}

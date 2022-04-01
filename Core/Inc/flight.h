/*
 * flight.h
 *
 *  Created on: Mar 31, 2022
 *      Author: cardinal
 */

#ifndef INC_FLIGHT_H_
#define INC_FLIGHT_H_

#include "fuzzy.h"
#include "imu.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define BLDC_MIN_PWM 1000
#define BLDC_MAX_PWM 2000

// состояния конечного автомата FSM
#define FSM_NULL 0 // самое начальное состояние
#define FSM_TAKEOFF 1 // режим взлета
#define FSM_HANGING 2 // демонстрационный режим зависания

float _acPitch;
float _acRoll;
volatile uint16_t uiFC_TicksCNT;
int8_t iprevRollError;

uint16_t motorFrontRightSpeed;
uint16_t motorBackRightSpeed;
uint16_t motorFrontLeftSpeed;
uint16_t motorBackLeftSpeed;

uint16_t stallSpeed;
extern uint8_t fsmState;

void FC_Init();
void FlightControl();
void motorDriver();
void selfStabilizing();
void setPitch(float aPitch);
void setRoll(float aRoll);

#endif /* INC_FLIGHT_H_ */

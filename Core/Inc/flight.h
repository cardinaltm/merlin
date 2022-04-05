/**
 *******************************************************************************
 * @file   flight.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#ifndef INC_FLIGHT_H_
#define INC_FLIGHT_H_

#include "main.h"

#define MOTOR_THROTTLE_MIN 1000
#define MOTOR_THROTTLE_MAX 2000
#define MOTOR_THROTTLE_INIT 1000
#define MOTOR_THROTTLE_LIMIT 1800

void Flight_Init(void);
void Flight_Control(void);
void calculatePID(void);

#endif /* INC_FLIGHT_H_ */

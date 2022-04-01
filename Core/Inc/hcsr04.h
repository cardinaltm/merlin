/*
 * hcsr04.h
 *
 *  Created on: Apr 1, 2022
 *      Author: cardinal
 */

#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

#include "stm32f4xx_hal.h"

#define TRIG_PIN SONAR_TRIGGER_Pin
#define TRIG_PORT SONAR_TRIGGER_GPIO_Port

extern uint32_t IC_Val1;
extern uint32_t IC_Val2;
extern uint32_t Difference;
extern uint8_t Is_First_Captured;
extern uint8_t sonarEchoDownDistance;

void HCSR04_Read(void);
void HCSR04_Delay(uint16_t time);

#endif /* INC_HCSR04_H_ */

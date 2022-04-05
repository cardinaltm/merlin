/**
 *******************************************************************************
 * @file   receiver.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 4, 2022
 *******************************************************************************
 **/

#ifndef INC_RECEIVER_H_
#define INC_RECEIVER_H_

#include "stm32f4xx_hal.h"

typedef struct Receiver
{
	uint32_t Throttle;
	uint32_t Pitch;
	uint32_t Roll;
	uint32_t Yaw;
} Receiver;

extern struct Receiver receiver;

void Receiver_Init(void);
void ReceiverCaptureCallback(TIM_HandleTypeDef *htim);

#endif /* INC_RECEIVER_H_ */

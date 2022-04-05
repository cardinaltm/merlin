/**
 *******************************************************************************
 * @file   board.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

#include "stm32f4xx_hal.h"
#include "adxl345.h"

//typedef void (*sensorInitFuncPtr)(sensor_align_e align);
//typedef void (*sensorReadFuncPtr)(int16_t *data);
//typedef void (*baroOpFuncPtr)(void);
//typedef void (*baroCalculateFuncPtr)(int32_t *pressure, int32_t *temperature);
//
//typedef struct sensor_t
//{
//	sensorInitFuncPtr init;
//	sensorReadFuncPtr read;
//	sensorReadFuncPtr temperature;
//} sensor_t;
//
//typedef struct baro_t
//{
//	uint16_t ut_delay;
//	uint16_t up_delay;
//	baroOpFuncPtr start_ut;
//	baroOpFuncPtr get_ut;
//	baroOpFuncPtr start_up;
//	baroOpFuncPtr get_up;
//	baroCalculateFuncPtr calculate;
//} baro_t;

#endif /* INC_BOARD_H_ */

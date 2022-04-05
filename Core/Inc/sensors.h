/**
 *******************************************************************************
 * @file   sensors.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 2, 2022
 *******************************************************************************
 **/

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include "main.h"

typedef struct SensorAcc
{
	int16_t X_RAW;
	int16_t Y_RAW;
	int16_t Z_RAW;
	double X;
	double Y;
	double Z;
} SensorAcc;

typedef struct SensorGyro
{
	int16_t X_RAW;
	int16_t Y_RAW;
	int16_t Z_RAW;
	double X; // Yaw
	double Y; // Pitch
	double Z; // Roll
} SensorGyro;

typedef struct SensorMag
{
	int16_t X_RAW;
	int16_t Y_RAW;
	int16_t Z_RAW;
	double X;
	double Y;
	double Z;
	double Pusula;
	double PusulaOKU;
} SensorMag;

typedef struct SensorBaro
{
	float Pressure;
	float Humidity;
	float Temperature;
	float Altitude;
	int8_t IsBME280;
} SensorBaro;

typedef struct SensorTemp
{
	float Temperature;
} SensorTemp;

extern struct SensorAcc sensorAcc;
extern struct SensorGyro sensorGyro;
extern struct SensorMag sensorMag;
extern struct SensorBaro sensorBaro;
extern struct SensorTemp sensorTemp;

void Sensors_Init(void);
void Sensors_Read(void);

#endif /* INC_SENSORS_H_ */

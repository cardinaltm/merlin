/**
 *******************************************************************************
 * @file   sensors.c
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 2, 2022
 *******************************************************************************
 **/

#include <stdio.h>
#include "sensors.h"
#include "mpu6050.h"
#include "adxl345.h"
#include "bmp280.h"
#include "lsm303dlhc.h"
#include "i2c.h"

struct SensorAcc sensorAcc;
struct SensorGyro sensorGyro;
struct SensorMag sensorMag;
struct SensorBaro sensorBaro;
struct SensorTemp sensorTemp;

MPU6050_t mpu6050data;

void Sensors_Init(void)
{
	// MPU6050
//	if (MPU6050_Init(&hi2c1) != 1) // TODO: change to while
//	{
//		printf("Failed found MPU6050\r\n");
//	}
//	else
//	{
//		printf("------- Init MPU6050 -------\r\n");
//	}

	ADXL345_Init();

	AccelMag_Ini();

	// BMP280
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;
	bmp280_init(&bmp280, &bmp280.params);

	while (!bmp280_init(&bmp280, &bmp280.params))
	{
		printf("BMP280 initialization failed, retry 2000ms\r\n");
		HAL_Delay(2000);
	}
	bme280p = bmp280.id == BME280_CHIP_ID;
	printf("BMP280: found %s\r\n", bme280p ? "BME280" : "BMP280");

	// Calibrate Sensors
	printf("Calibrate sensors...\r\n");
	for (int i = 0; i < 2000; i++)
	{
		Sensors_Read();
		HAL_Delay(3);
	}
}

void Sensors_Read(void)
{
	// MPU6050
	ADXL345_Read(&sensorAcc);

	AccelMag_Read(&sensorMag);

	// BMP280
	if (!bmp280_read_float(&bmp280, &sensorBaro.Temperature, &sensorBaro.Pressure, &sensorBaro.Humidity, &sensorBaro.Altitude))
	{
		printf("Temperature/pressure reading failed\n");
	}

}

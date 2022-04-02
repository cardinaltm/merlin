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
#include "adxl345.h"
#include "bmp280.h"
#include "i2c.h"

struct SensorAcc sensorAcc;
struct SensorGyro sensoGyro;
struct SensorMag sensorMag;
struct SensorBaro sensorBaro;
struct SensorTemp sensorTemp;

void Sensors_Init(void)
{
	ADXL345_Init();

	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c2;
	bmp280_init(&bmp280, &bmp280.params);

	while (!bmp280_init(&bmp280, &bmp280.params))
	{
		printf("BMP280 initialization failed, retry 2000ms\r\n");
		HAL_Delay(2000);
	}
	bme280p = bmp280.id == BME280_CHIP_ID;
	printf("BMP280: found %s\r\n", bme280p ? "BME280" : "BMP280");
}

void Sensors_Read(void)
{
	ADXL345_Read(&sensorAcc);

	if (!bmp280_read_float(&bmp280, &sensorBaro.Temperature, &sensorBaro.Pressure, &sensorBaro.Humidity, &sensorBaro.Altitude))
	{
		printf("Temperature/pressure reading failed\n");
	}

}

/**
 *******************************************************************************
 * @file   adxl345.c
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#include "adxl345.h"
#include <stdio.h>
#include "i2c.h"

struct AdxlCommands AdxlReg =
{ 0x00, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };

uint8_t i2c_tx[2];
int16_t x, y, z;
uint8_t raw[6];

void ADXL345_Init(void)
{
	/* Rate */
	i2c_tx[0] = AdxlReg.BW_RATE;
	i2c_tx[1] = ((1 << D3) | (1 << D1));
	HAL_I2C_Master_Transmit(&hi2c1, ADXL_ADR, (uint8_t*) i2c_tx, 2, 1000);

	/* Range 16g */
	i2c_tx[0] = AdxlReg.DATA_FORMAT;
	i2c_tx[1] = ((1 << D0) | (1 << D1));
	HAL_I2C_Master_Transmit(&hi2c1, ADXL_ADR, (uint8_t*) i2c_tx, 2, 1000);

	/* Measure */
	i2c_tx[0] = AdxlReg.POWER_CTL;
	i2c_tx[1] = (1 << D3);
	HAL_I2C_Master_Transmit(&hi2c1, ADXL_ADR, (uint8_t*) i2c_tx, 2, 1000);
}

void ADXL345_Read(SensorAcc *data)
{
	HAL_I2C_Master_Transmit(&hi2c1, ADXL_ADR, (uint8_t*) &AdxlReg.DATAX0, 1, 100);
	HAL_I2C_Master_Receive(&hi2c1, ADXL_ADR, (uint8_t*) raw, 6, 100);

	data->X_RAW = ((int16_t) (raw[1] << 8) | raw[0]);
	data->Y_RAW = ((int16_t) (raw[3] << 8) | raw[2]);
	data->Z_RAW = ((int16_t) (raw[5] << 8) | raw[4]);

	data->X = (SCALE_FACTOR * data->X_RAW) / 1000;
	data->Y = (SCALE_FACTOR * data->Y_RAW) / 1000;
	data->Z = (SCALE_FACTOR * data->Z_RAW) / 1000;
}

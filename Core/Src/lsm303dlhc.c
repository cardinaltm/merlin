/**
 *******************************************************************************
 * @file   lsm303dlhc.c
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 6, 2022
 *******************************************************************************
 **/

#include "lsm303dlhc.h"
#include <stdio.h>
#include "i2c.h"

uint8_t buf2[14] =
{ 0 };
char str1[30] =
{ 0 };

volatile int16_t xbuf_avg[8] =
{ 0 }, ybuf_avg[8] =
{ 0 }, zbuf_avg[8] =
{ 0 };

volatile int8_t avg_cnt;
volatile int64_t tmp64[3];
float pusula_oku, pusula;

void Error(void)
{

}

void MovingAverage(int16_t *dt)
{
	if (avg_cnt < 8)
	{
		xbuf_avg[avg_cnt] = dt[0];
		ybuf_avg[avg_cnt] = dt[1];
		zbuf_avg[avg_cnt] = dt[2];
		if (avg_cnt == 7)
		{
			tmp64[0] = xbuf_avg[7] + xbuf_avg[6] + xbuf_avg[5] + xbuf_avg[4] +\
 xbuf_avg[3] + xbuf_avg[2] + xbuf_avg[1] + xbuf_avg[0];
			tmp64[1] = ybuf_avg[7] + ybuf_avg[6] + ybuf_avg[5] + ybuf_avg[4] +\
 ybuf_avg[3] + ybuf_avg[2] + ybuf_avg[1] + ybuf_avg[0];
			tmp64[2] = zbuf_avg[7] + zbuf_avg[6] + zbuf_avg[5] + zbuf_avg[4] +\
 zbuf_avg[3] + zbuf_avg[2] + zbuf_avg[1] + zbuf_avg[0];
			dt[0] = tmp64[0] / 8;
			dt[1] = tmp64[1] / 8;
			dt[2] = tmp64[2] / 8;
		}
		avg_cnt++;
	}
	else
	{
		//âû÷òåì èç îáùèõ ñóìì ïåðâûå (ñàìûå ñòàðûå) ýëåìåíòû
		tmp64[0] -= xbuf_avg[0];
		tmp64[1] -= ybuf_avg[0];
		tmp64[2] -= zbuf_avg[0];
		//ñäâèíåì áóôåðû íà 1 ýëåìåíò
		memcpy((void*) xbuf_avg, (void*) (xbuf_avg + 1), sizeof(int16_t) * 7);
		memcpy((void*) ybuf_avg, (void*) (ybuf_avg + 1), sizeof(int16_t) * 7);
		memcpy((void*) zbuf_avg, (void*) (zbuf_avg + 1), sizeof(int16_t) * 7);
		//çàìåíèì 7 ýëåìåíòû íà íîâûå
		xbuf_avg[7] = dt[0];
		ybuf_avg[7] = dt[1];
		zbuf_avg[7] = dt[2];
		//ïðèáàâèì íîâûå ýëåìåíòû
		tmp64[0] += dt[0];
		tmp64[1] += dt[1];
		tmp64[2] += dt[2];
		//îáíîâèì ñðåäíèå çíà÷åíèÿ
		dt[0] = tmp64[0] / 8;
		dt[1] = tmp64[1] / 8;
		dt[2] = tmp64[2] / 8;
	}
}
//--------------------------------------------
static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;
	status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 0x10000);
	if (status != HAL_OK)
		Error();
	return value;
}
//--------------------------------------------
static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 0x10000);
	if (status != HAL_OK)
		Error();
}
//--------------------------------------------
uint8_t Accel_IO_Read(uint16_t DeviceAddr, uint8_t RegisterAddr)
{
	return I2Cx_ReadData(DeviceAddr, RegisterAddr);
}
//--------------------------------------------
void Accel_IO_Write(uint16_t DeviceAddr, uint8_t RegisterAddr, uint8_t Value)
{
	I2Cx_WriteData(DeviceAddr, RegisterAddr, Value);
}
//--------------------------------------------
void Accel_GetXYZ(int16_t *pData)
{
	int16_t pnRawData[3];
	uint8_t ctrlx[2] =
	{ 0, 0 };
	uint8_t buffer[6];
	uint8_t i = 0;
	uint8_t sensitivity = LSM303DLHC_FULLSCALE_2G;

	ctrlx[0] = Accel_IO_Read(0x32, LSM303DLHC_CTRL_REG4_A);
	ctrlx[1] = Accel_IO_Read(0x32, LSM303DLHC_CTRL_REG5_A);

	buffer[0] = Accel_IO_Read(0x32, LSM303DLHC_OUT_X_L_A);
	buffer[1] = Accel_IO_Read(0x32, LSM303DLHC_OUT_X_H_A);
	buffer[2] = Accel_IO_Read(0x32, LSM303DLHC_OUT_Y_L_A);
	buffer[3] = Accel_IO_Read(0x32, LSM303DLHC_OUT_Y_H_A);
	buffer[4] = Accel_IO_Read(0x32, LSM303DLHC_OUT_Z_L_A);
	buffer[5] = Accel_IO_Read(0x32, LSM303DLHC_OUT_Z_H_A);

	if (!(ctrlx[0] & LSM303DLHC_BLE_MSB))
	{
		for (i = 0; i < 3; i++)
		{
			pnRawData[i] = ((int16_t) ((uint16_t) buffer[2 * i + 1] << 8) + buffer[2 * i]);
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			pnRawData[i] = ((int16_t) ((uint16_t) buffer[2 * i] << 8) + buffer[2 * i + 1]);
		}
	}

	switch (ctrlx[0] & LSM303DLHC_FULLSCALE_16G)
	{
	case LSM303DLHC_FULLSCALE_2G:
		sensitivity = LSM303DLHC_ACC_SENSITIVITY_2G;
		break;
	case LSM303DLHC_FULLSCALE_4G:
		sensitivity = LSM303DLHC_ACC_SENSITIVITY_4G;
		break;
	case LSM303DLHC_FULLSCALE_8G:
		sensitivity = LSM303DLHC_ACC_SENSITIVITY_8G;
		break;
	case LSM303DLHC_FULLSCALE_16G:
		sensitivity = LSM303DLHC_ACC_SENSITIVITY_16G;
		break;
	}

	for (i = 0; i < 3; i++)
	{
		pData[i] = (pnRawData[i] * sensitivity);
	}
}
//--------------------------------------------
void Mag_GetXYZ(int16_t *pData)
{
	uint8_t buffer[6];
	uint8_t i = 0;

	buffer[0] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M);
	buffer[1] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_L_M);
	buffer[2] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_M);
	buffer[3] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_M);
	buffer[4] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_M);
	buffer[5] = Accel_IO_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_M);

	for (i = 0; i < 3; i++)
	{
		if (pData[i] != -4096)
			pData[i] = ((uint16_t) ((uint16_t) buffer[2 * i] << 8) + buffer[2 * i + 1]);
	}
}
//--------------------------------------------
uint8_t Accel_ReadID(void)
{
	uint8_t ctrl = 0x00;
	ctrl = Accel_IO_Read(0x32, 0x0F);
	return ctrl;
}
//--------------------------------------------
void AccelMag_Read(SensorMag *sensorMag)
{
	int16_t buffer[3] =
	{ 0 };
	static int16_t tmp16;
	float val[3];
	Mag_GetXYZ(buffer);
	tmp16 = buffer[0];
	if (tmp16 != -4096)
		val[0] = tmp16 + 64;
	tmp16 = buffer[1];
	if (tmp16 != -4096)
		val[1] = tmp16 + 174;
	tmp16 = buffer[2];
	if (tmp16 != -4096)
		val[2] = tmp16 + 1204;
	//âûçîâåì ôèëüòð ñêîëüçÿùåãî ñðåäíåãî
	MovingAverage(val);

	sensorMag->X = val[0];
	sensorMag->Y = val[1];
	sensorMag->Z = val[2];

	sensorMag->Pusula = atan2f(val[1], val[0]) * 100 / 3.14;
	if (sensorMag->Pusula > 0)
	{
		sensorMag->PusulaOKU = sensorMag->Pusula;
	}
	else
	{
		sensorMag->PusulaOKU = 360 + sensorMag->Pusula;
	}

//	printf("MAG_TEST - X:%.2f Y:%.2f Z:%.2f\r\n", val[0], val[1], val[2]);

//	buf2[0] = 0x11;
//	buf2[1] = 0x55;
//	buf2[2] = (uint8_t) (val[0] >> 8);
//	buf2[3] = (uint8_t) val[0];
//	buf2[4] = (uint8_t) (val[1] >> 8);
//	buf2[5] = (uint8_t) val[2];
//	buf2[6] = (uint8_t) (val[2] >> 8);
//	buf2[7] = (uint8_t) val[2];
}
//--------------------------------------------
void AccInit(uint16_t InitStruct)
{
	uint8_t ctrl = 0x00;
	ctrl = (uint8_t) InitStruct;
	Accel_IO_Write(0x32, LSM303DLHC_CTRL_REG1_A, ctrl);
	ctrl = (uint8_t) (InitStruct << 8);
	Accel_IO_Write(0x32, LSM303DLHC_CTRL_REG4_A, ctrl);
}
//--------------------------------------------
void MagInit(uint32_t InitStruct)
{
	uint8_t ctrl = 0x00;
	ctrl = (uint8_t) InitStruct;
	Accel_IO_Write(MAG_I2C_ADDRESS, LSM303DLHC_CRA_REG_M, ctrl);
	ctrl = (uint8_t) (InitStruct << 8);
	Accel_IO_Write(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, ctrl);
	ctrl = (uint8_t) (InitStruct << 16);
	Accel_IO_Write(MAG_I2C_ADDRESS, LSM303DLHC_MR_REG_M, ctrl);
}
//--------------------------------------------
void Accel_AccFilter(uint16_t FilterStruct)
{
	uint8_t tmpreg;
	tmpreg = Accel_IO_Read(0x32, LSM303DLHC_CTRL_REG2_A);
	tmpreg &= 0x0C;
	tmpreg |= FilterStruct;
	Accel_IO_Write(0x32, LSM303DLHC_CTRL_REG2_A, tmpreg);
}
//--------------------------------------------
void AccelMag_Ini(void)
{
	uint16_t ctrl = 0x0000;
	uint32_t ctrl32 = 0x00000000;
	avg_cnt = 0; //ñ÷åò÷èê çàïîëíåíèÿ
	HAL_Delay(1000);
	if (Accel_ReadID() == 0x33)
		Error();
	else
		Error();
//	ctrl|=(LSM303DLHC_NORMAL_MODE|LSM303DLHC_ODR_50_HZ|LSM303DLHC_AXES_ENABLE);
//	ctrl|=((LSM303DLHC_BlockUpdate_Continous|LSM303DLHC_BLE_LSB|LSM303DLHC_HR_ENABLE)<<8);
//	AccInit(ctrl);
//	ctrl=(uint8_t)(LSM303DLHC_HPM_NORMAL_MODE|LSM303DLHC_HPFCF_16|\
//								 LSM303DLHC_HPF_AOI1_DISABLE|LSM303DLHC_HPF_AOI2_DISABLE);
//	Accel_AccFilter(ctrl);
	ctrl32 |= (LSM303DLHC_TEMPSENSOR_DISABLE | LSM303DLHC_ODR_220_HZ);
	ctrl32 |= LSM303DLHC_FS_4_0_GA << 8;
	ctrl32 |= LSM303DLHC_CONTINUOS_CONVERSION << 16;
	MagInit(ctrl32);
}

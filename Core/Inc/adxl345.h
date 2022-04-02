/**
 *******************************************************************************
 * @file   adxl345.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "sensors.h"

#define SCALE_FACTOR 29.0
#define ADXL_ADR (0x53 << 1)

/* Flags */
extern uint8_t AdxlFlgInt1;
extern uint8_t AdxlFlgInt2;

struct AdxlCommands
{
	uint8_t DEVID; /*	Device ID	*/
	uint8_t THRESH_TAP; /*	Tap threshold	*/
	uint8_t OFSX; /*	X-axis offset	*/
	uint8_t OFSY; /*	Y-axis offset	*/
	uint8_t OFSZ; /*	Z-axis offset	*/
	uint8_t DUR; /*	Tap duration	*/
	uint8_t Latent; /*	Tap latency	*/
	uint8_t Window; /*	Tap window	*/
	uint8_t THRESH_ACT; /*	Activity threshold	*/
	uint8_t THRESH_INACT; /*	Inactivity threshold	*/
	uint8_t TIME_INACT; /*	Inactivity time	*/
	uint8_t ACT_INACT_CTL; /*	Axis enable control for activity and inactivity detection	*/
	uint8_t THRESH_FF; /*	Free-fall threshold	*/
	uint8_t TIME_FF; /*	Free-fall time	*/
	uint8_t TAP_AXES; /*	Axis control for single tap/double tap	*/
	uint8_t ACT_TAP_STATUS; /*	Source of single tap/double tap	*/
	uint8_t BW_RATE; /*	Data rate and power mode control	*/
	uint8_t POWER_CTL; /*	Power-saving features control	*/
	uint8_t INT_ENABLE; /*	Interrupt enable control	*/
	uint8_t INT_MAP; /*	Interrupt mapping control	*/
	uint8_t INT_SOURCE; /*	Source of interrupts	*/
	uint8_t DATA_FORMAT; /*	Data format control	*/
	uint8_t DATAX0; /*	X-Axis Data 0	*/
	uint8_t DATAX1; /*	X-Axis Data 1	*/
	uint8_t DATAY0; /*	Y-Axis Data 0	*/
	uint8_t DATAY1; /*	Y-Axis Data 1	*/
	uint8_t DATAZ0; /*	Z-Axis Data 0	*/
	uint8_t DATAZ1; /*	Z-Axis Data 1	*/
	uint8_t FIFO_CTL; /*	FIFO control	*/
	uint8_t FIFO_STATUS; /*	FIFO status	*/
};

enum AdxlBitNum
{
	D0, D1, D2, D3, D4, D5, D6, D7
};

void ADXL345_Init(void);
void ADXL345_Read(SensorAcc *data);

#endif /* INC_ADXL345_H_ */

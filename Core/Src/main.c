/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mpu6050.h"
#include "bmp280.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

MPU6050_t MPU6050;

BMP280_HandleTypedef bmp280;
float pressure, temperature, humidity;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len)
{
	HAL_StatusTypeDef hstatus;
	hstatus = HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	if (hstatus == HAL_OK)
		return len;
	else
		return 0;
}

#define TIMCLOCK   84000000
#define PRESCALAR  84

uint32_t receiverThrottleVal1 = 0;
uint32_t receiverThrottleVal2 = 0;
uint32_t receiverThrottleDifference = 0;
int isFirstCaptured = 0;

float frequency = 0;

float receiverThrottleMin = 1000;
float receiverThrottleMax = 2000;

float motorThrottleMin = 1000;
float motorThrottleMax = 2000;

float throttlePercent = 0;

// Get Pulse Widthx
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	switch (htim->Channel)
	{
	// RECEIVER_THROTTLE
	case HAL_TIM_ACTIVE_CHANNEL_1:
		if (isFirstCaptured == 0) // if the first value is not captured
		{
			receiverThrottleVal1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			isFirstCaptured = 1;  // set the first captured as true
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (isFirstCaptured == 1)   // if the first is already captured
		{
			receiverThrottleVal2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);

			if (receiverThrottleVal2 > receiverThrottleVal1)
			{
				receiverThrottleDifference = receiverThrottleVal2 - receiverThrottleVal1;

				throttlePercent = ((receiverThrottleDifference - receiverThrottleMin) / (receiverThrottleMax - receiverThrottleMin)) * 100;

				float motorThrottleWid = ((motorThrottleMax - motorThrottleMin) * (throttlePercent / 100)) + motorThrottleMin;
				htim1.Instance->CCR1 = motorThrottleWid; // MOTOR_FRONT_RIGHT
				htim1.Instance->CCR2 = motorThrottleWid; // MOTOR_BACK_RIGHT
				htim1.Instance->CCR3 = motorThrottleWid; // MOTOR_FRONT_LEFT
				htim1.Instance->CCR4 = motorThrottleWid; // MOTOR_BACK_LEFT

			}

			isFirstCaptured = 0;
			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
		break;
		// RECEIVER_YAW
	case HAL_TIM_ACTIVE_CHANNEL_2:
		break;
		// RECEIVER_PITCH
	case HAL_TIM_ACTIVE_CHANNEL_3:
		break;
		// RECEIVER_ROLL
	case HAL_TIM_ACTIVE_CHANNEL_4:
		break;
	default:
		break;
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_I2C2_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	MX_I2C3_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	printf("------- Start Software -------\r\n");

	// Init MPU6050
	if (MPU6050_Init(&hi2c1) != 1) // TODO: change to while
	{
		printf("Failed found MPU6050\r\n");
	}
	else
	{
		printf("------- Init MPU6050 -------\r\n");
	}

	// Init BMP280
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c2;
	bmp280_init(&bmp280, &bmp280.params);

	if (!bmp280_init(&bmp280, &bmp280.params))
	{
		printf("BMP280 initialization failed\n");
		HAL_Delay(2000);
	}
	//    while (!bmp280_init(&bmp280, &bmp280.params)) {
	//        printf("BMP280 initialization failed\n");
	//        HAL_Delay(2000);
	//    }
	bool bme280p = bmp280.id == BME280_CHIP_ID;
	printf("BMP280: found %s\r\n", bme280p ? "BME280" : "BMP280");

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // MOTOR_FRONT_RIGHT
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // MOTOR_BACK_RIGHT
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // MOTOR_FRONT_LEFT
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // MOTOR_BACK_LEFT
	printf("------- Initialize Motors -------\r\n");
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // RECEIVER_THROTTLE
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2); // RECEIVER_YAW
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3); // RECEIVER_PITCH
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4); // RECEIVER_ROLL

	printf("------- Initialize Receiver -------\r\n");

	// Set Default
	HAL_Delay(1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 1000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1000);
	printf("------- Set Default Motor Wid -------\r\n");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		MPU6050_Read_All(&hi2c1, &MPU6050);
		HAL_Delay(100);
		printf("MPU6050: Accel(X: %.3f, Y: %.3f, Z: %.3f) Gyro(X: %.3f, Y: %.3f, Z: %.3f) Kalman(X: %.3f, Y: %.3f) Temperature(%.3f)\r\n", MPU6050.Ax, MPU6050.Ay, MPU6050.Az, MPU6050.Gx, MPU6050.Gy, MPU6050.Gz, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.Temperature);
		HAL_Delay(200);

//		while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
//		{
//			printf("Temperature/pressure reading failed\n");
//			HAL_Delay(2000);
//		}

		printf("BMP280: Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
		if (bme280p)
		{
			printf(", Humidity: %.2f\r\n", humidity);
		}
		else
		{
			printf("\r\n");
		}

		printf("RECEIVER: Frequency: %.2f, throttlePercent: %.2f, Pulse Width: %lu\r\n", frequency, throttlePercent, (unsigned long) receiverThrottleDifference);

		HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);

		HAL_Delay(2000);

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sensors.h"

#include "mpu6050.h"
#include "bmp280.h"
#include "esp8266.h"
#include "hcsr04.h"
#include "adxl345.h"
//#include "gnss.h"
#include "flight.h"

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
uint16_t SysTicks = 0;

MPU6050_t MPU6050;

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
	hstatus = HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	if (hstatus == HAL_OK)
		return len;
	else
		return 0;
}

#define TIMCLOCK   84000000
#define PRESCALAR  84

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		if (Is_First_Captured == 0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured == 1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2 - IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			sonarEchoDownDistance = Difference * .034 / 2;
			Is_First_Captured = 0; // set it back to false

			// Custom Test
			if (sonarEchoDownDistance <= 20)
			{
				HAL_GPIO_WritePin(LIGHT_BELOW_GPIO_Port, LIGHT_BELOW_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(LIGHT_BELOW_GPIO_Port, LIGHT_BELOW_Pin, GPIO_PIN_RESET);
			}

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}

	if (htim->Instance == TIM2)
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
  MX_USART3_UART_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	Sensors_Init();
	// Initialize Flight Controller
//	FC_Init();
	// Switch Flight Mode
//	fsmState = FSM_TAKEOFF;
	printf("Start Software\r\n");

//  // GPS Module Init
//	GNSS_Init(&GNSS_Handle, &huart1);
//	HAL_Delay(1000);
//	GNSS_LoadConfig(&GNSS_Handle);
//	uint32_t Timer = HAL_GetTick();
//
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

	// Init MPU6050
	if (MPU6050_Init(&hi2c1) != 1) // TODO: change to while
	{
		printf("Failed found MPU6050\r\n");
	}
	else
	{
		printf("------- Init MPU6050 -------\r\n");
	}

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // RECEIVER_THROTTLE
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2); // RECEIVER_YAW
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3); // RECEIVER_PITCH
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4); // RECEIVER_ROLL

	printf("Initialize Receivers\r\n");

	Flight_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		printf("---------------------------------------------------------------\r\n");
//		if ((HAL_GetTick() - Timer) > 1000)
//		{
//			GNSS_GetUniqID(&GNSS_Handle);
//			GNSS_ParseBuffer(&GNSS_Handle);
//			HAL_Delay(250);
//			GNSS_GetPVTData(&GNSS_Handle);
//			GNSS_ParseBuffer(&GNSS_Handle);
//			HAL_Delay(250);
//			GNSS_SetMode(&GNSS_Handle, Automotiv);
//			HAL_Delay(250);
//			printf("Day: %d-%d-%d \r\n", GNSS_Handle.day, GNSS_Handle.month, GNSS_Handle.year);
//			printf("Time: %d:%d:%d \r\n", GNSS_Handle.hour, GNSS_Handle.min, GNSS_Handle.sec);
//			printf("Status of fix: %d \r\n", GNSS_Handle.fixType);
//			printf("Latitude: %f \r\n", GNSS_Handle.fLat);
//			printf("Longitude: %f \r\n", (float) GNSS_Handle.lon / 10000000.0);
//			printf("Height above ellipsoid: %d \r\n", GNSS_Handle.height);
//			printf("Height above mean sea level: %d \r\n", GNSS_Handle.hMSL);
//			printf("Ground Speed (2-D): %d \r\n", GNSS_Handle.gSpeed);
//			printf("Unique ID: %04X %04X %04X %04X %04X \n\r", GNSS_Handle.uniqueID[0], GNSS_Handle.uniqueID[1], GNSS_Handle.uniqueID[2], GNSS_Handle.uniqueID[3], GNSS_Handle.uniqueID[4], GNSS_Handle.uniqueID[5]);
//			Timer = HAL_GetTick();
//		}
		HCSR04_Read();
		printf("HCSR04 - Distance: %u cm\r\n", sonarEchoDownDistance);

		Sensors_Read();
		printf("ADXL345 - X: %.3f,  Y: %.3f, Z: %.3f\r\n", sensorAcc.X, sensorAcc.Y, sensorAcc.Z);
		printf("BMP280 - Pressure: %.2f Pa, Temperature: %.2f °C, Humidity: %.2f, Altitude: %.2f m\r\n", sensorBaro.Pressure, sensorBaro.Temperature, sensorBaro.Humidity, sensorBaro.Altitude);

		Flight_Control();

		HAL_Delay(300);

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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

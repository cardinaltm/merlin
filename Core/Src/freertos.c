/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readMPU6050Task */
osThreadId_t readMPU6050TaskHandle;
const osThreadAttr_t readMPU6050Task_attributes = {
  .name = "readMPU6050Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readBMP280Task */
osThreadId_t readBMP280TaskHandle;
const osThreadAttr_t readBMP280Task_attributes = {
  .name = "readBMP280Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readHMC5983Task */
osThreadId_t readHMC5983TaskHandle;
const osThreadAttr_t readHMC5983Task_attributes = {
  .name = "readHMC5983Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readNEOM8NTask */
osThreadId_t readNEOM8NTaskHandle;
const osThreadAttr_t readNEOM8NTask_attributes = {
  .name = "readNEOM8NTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readHCSR04Task */
osThreadId_t readHCSR04TaskHandle;
const osThreadAttr_t readHCSR04Task_attributes = {
  .name = "readHCSR04Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for flashLEDOutTask */
osThreadId_t flashLEDOutTaskHandle;
const osThreadAttr_t flashLEDOutTask_attributes = {
  .name = "flashLEDOutTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for receiverTask */
osThreadId_t receiverTaskHandle;
const osThreadAttr_t receiverTask_attributes = {
  .name = "receiverTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for readESP8266Task */
osThreadId_t readESP8266TaskHandle;
const osThreadAttr_t readESP8266Task_attributes = {
  .name = "readESP8266Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartReadMPU6050Task(void *argument);
void StartReadBMP280Task(void *argument);
void StartReadHMC5983Task(void *argument);
void StartReadNEOM8NTask(void *argument);
void StartReadHCSR04Task(void *argument);
void StartFlashLEDOutTask(void *argument);
void StartRevceiverTask(void *argument);
void StartReadESP8266Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of readMPU6050Task */
  readMPU6050TaskHandle = osThreadNew(StartReadMPU6050Task, NULL, &readMPU6050Task_attributes);

  /* creation of readBMP280Task */
  readBMP280TaskHandle = osThreadNew(StartReadBMP280Task, NULL, &readBMP280Task_attributes);

  /* creation of readHMC5983Task */
  readHMC5983TaskHandle = osThreadNew(StartReadHMC5983Task, NULL, &readHMC5983Task_attributes);

  /* creation of readNEOM8NTask */
  readNEOM8NTaskHandle = osThreadNew(StartReadNEOM8NTask, NULL, &readNEOM8NTask_attributes);

  /* creation of readHCSR04Task */
  readHCSR04TaskHandle = osThreadNew(StartReadHCSR04Task, NULL, &readHCSR04Task_attributes);

  /* creation of flashLEDOutTask */
  flashLEDOutTaskHandle = osThreadNew(StartFlashLEDOutTask, NULL, &flashLEDOutTask_attributes);

  /* creation of receiverTask */
  receiverTaskHandle = osThreadNew(StartRevceiverTask, NULL, &receiverTask_attributes);

  /* creation of readESP8266Task */
  readESP8266TaskHandle = osThreadNew(StartReadESP8266Task, NULL, &readESP8266Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartReadMPU6050Task */
/**
 * @brief Function implementing the readMPU6050Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadMPU6050Task */
void StartReadMPU6050Task(void *argument)
{
  /* USER CODE BEGIN StartReadMPU6050Task */
	/* Infinite loop */
	for (;;)
	{
		MPU6050_Read_All(&hi2c1, &MPU6050);
		osDelay(100);
		printf("MPU6050: Accel(X: %.3f, Y: %.3f, Z: %.3f) Gyro(X: %.3f, Y: %.3f, Z: %.3f) Kalman(X: %.3f, Y: %.3f) Temperature(%.3f)\r\n", MPU6050.Ax, MPU6050.Ay, MPU6050.Az, MPU6050.Gx, MPU6050.Gy, MPU6050.Gz, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.Temperature);
		osDelay(200);
	}
  /* USER CODE END StartReadMPU6050Task */
}

/* USER CODE BEGIN Header_StartReadBMP280Task */
/**
 * @brief Function implementing the readBMP280Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadBMP280Task */
void StartReadBMP280Task(void *argument)
{
  /* USER CODE BEGIN StartReadBMP280Task */
	/* Infinite loop */
	for (;;)
	{
		if (bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
		{
			printf("BMP280: Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
			if (bme280p)
			{
				printf(", Humidity: %.2f\r\n", humidity);
			}
			else
			{
				printf("\r\n");
			}
			osDelay(200);
		}
		else
		{
			printf("Temperature/pressure reading failed\n");
			osDelay(2000);
		}
	}
  /* USER CODE END StartReadBMP280Task */
}

/* USER CODE BEGIN Header_StartReadHMC5983Task */
/**
 * @brief Function implementing the readHMC5983Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadHMC5983Task */
void StartReadHMC5983Task(void *argument)
{
  /* USER CODE BEGIN StartReadHMC5983Task */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartReadHMC5983Task */
}

/* USER CODE BEGIN Header_StartReadNEOM8NTask */
/**
 * @brief Function implementing the readNEOM8NTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadNEOM8NTask */
void StartReadNEOM8NTask(void *argument)
{
  /* USER CODE BEGIN StartReadNEOM8NTask */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartReadNEOM8NTask */
}

/* USER CODE BEGIN Header_StartReadHCSR04Task */
/**
 * @brief Function implementing the readHCSR04Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadHCSR04Task */
void StartReadHCSR04Task(void *argument)
{
  /* USER CODE BEGIN StartReadHCSR04Task */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartReadHCSR04Task */
}

/* USER CODE BEGIN Header_StartFlashLEDOutTask */
/**
 * @brief Function implementing the flashLEDOutTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartFlashLEDOutTask */
void StartFlashLEDOutTask(void *argument)
{
  /* USER CODE BEGIN StartFlashLEDOutTask */
	/* Infinite loop */
	for (;;)
	{
		HAL_GPIO_WritePin(LED_OUT_GPIO_Port, LED_OUT_Pin, GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(LED_OUT_GPIO_Port, LED_OUT_Pin, GPIO_PIN_RESET);
		osDelay(800);
	}
  /* USER CODE END StartFlashLEDOutTask */
}

/* USER CODE BEGIN Header_StartRevceiverTask */
/**
 * @brief Function implementing the receiverTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartRevceiverTask */
void StartRevceiverTask(void *argument)
{
  /* USER CODE BEGIN StartRevceiverTask */
	/* Infinite loop */
	for (;;)
	{
		// print receiver data
		printf("RECEIVER: Frequency: %.2f, throttlePercent: %.2f, Pulse Width: %lu\r\n", frequency, throttlePercent, (unsigned long) receiverThrottleDifference);
		osDelay(200);
	}
  /* USER CODE END StartRevceiverTask */
}

/* USER CODE BEGIN Header_StartReadESP8266Task */
/**
* @brief Function implementing the readESP8266Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReadESP8266Task */
void StartReadESP8266Task(void *argument)
{
  /* USER CODE BEGIN StartReadESP8266Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartReadESP8266Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

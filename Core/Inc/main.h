/* USER CODE BEGIN Header */
/**
 *******************************************************************************
 * @file   main.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUTTON_K1_Pin GPIO_PIN_3
#define BUTTON_K1_GPIO_Port GPIOE
#define BUTTON_K0_Pin GPIO_PIN_4
#define BUTTON_K0_GPIO_Port GPIOE
#define RECEIVER_THROTTLE_Pin GPIO_PIN_0
#define RECEIVER_THROTTLE_GPIO_Port GPIOA
#define RECEIVER_YAW_Pin GPIO_PIN_1
#define RECEIVER_YAW_GPIO_Port GPIOA
#define RECEIVER_PITCH_Pin GPIO_PIN_2
#define RECEIVER_PITCH_GPIO_Port GPIOA
#define RECEIVER_ROLL_Pin GPIO_PIN_3
#define RECEIVER_ROLL_GPIO_Port GPIOA
#define LED_D2_Pin GPIO_PIN_6
#define LED_D2_GPIO_Port GPIOA
#define LED_D3_Pin GPIO_PIN_7
#define LED_D3_GPIO_Port GPIOA
#define LIGHT_BELOW_Pin GPIO_PIN_4
#define LIGHT_BELOW_GPIO_Port GPIOC
#define SONAR_ECHO_FRONT_Pin GPIO_PIN_0
#define SONAR_ECHO_FRONT_GPIO_Port GPIOB
#define SONAR_ECHO_REAR_Pin GPIO_PIN_1
#define SONAR_ECHO_REAR_GPIO_Port GPIOB
#define ESC_FRONT_RIGHT_Pin GPIO_PIN_9
#define ESC_FRONT_RIGHT_GPIO_Port GPIOE
#define ESC_REAR_RIGHT_Pin GPIO_PIN_11
#define ESC_REAR_RIGHT_GPIO_Port GPIOE
#define ESC_REAR_LEFT_Pin GPIO_PIN_13
#define ESC_REAR_LEFT_GPIO_Port GPIOE
#define ESC_FRONT_LEFT_Pin GPIO_PIN_14
#define ESC_FRONT_LEFT_GPIO_Port GPIOE
#define SONAR_TRIGGER_Pin GPIO_PIN_15
#define SONAR_TRIGGER_GPIO_Port GPIOB
#define ESP8266_TX_Pin GPIO_PIN_8
#define ESP8266_TX_GPIO_Port GPIOD
#define ESP8266_RX_Pin GPIO_PIN_9
#define ESP8266_RX_GPIO_Port GPIOD
#define SONAR_ECHO_BOTTOM_Pin GPIO_PIN_12
#define SONAR_ECHO_BOTTOM_GPIO_Port GPIOD
#define SONAR_ECHO_LEFT_Pin GPIO_PIN_6
#define SONAR_ECHO_LEFT_GPIO_Port GPIOC
#define SONAR_ECHO_RIGHT_Pin GPIO_PIN_7
#define SONAR_ECHO_RIGHT_GPIO_Port GPIOC
#define DEBUG_TX_Pin GPIO_PIN_9
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_10
#define DEBUG_RX_GPIO_Port GPIOA
#define NEOM8N_TX_Pin GPIO_PIN_5
#define NEOM8N_TX_GPIO_Port GPIOD
#define NEOM8N_RX_Pin GPIO_PIN_6
#define NEOM8N_RX_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32c0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define GND_SENSE_Pin GPIO_PIN_0
#define GND_SENSE_GPIO_Port GPIOA
#define VBUS_SENSE_Pin GPIO_PIN_1
#define VBUS_SENSE_GPIO_Port GPIOA
#define CC1_CTRL_Pin GPIO_PIN_2
#define CC1_CTRL_GPIO_Port GPIOA
#define CC2_CTRL_Pin GPIO_PIN_3
#define CC2_CTRL_GPIO_Port GPIOA
#define B_CC1_SENSE_Pin GPIO_PIN_4
#define B_CC1_SENSE_GPIO_Port GPIOA
#define B_CC2_SENSE_Pin GPIO_PIN_5
#define B_CC2_SENSE_GPIO_Port GPIOA
#define A_CC1_Pin GPIO_PIN_6
#define A_CC1_GPIO_Port GPIOA
#define A_CC2_Pin GPIO_PIN_7
#define A_CC2_GPIO_Port GPIOA
#define GND_LED_Pin GPIO_PIN_0
#define GND_LED_GPIO_Port GPIOB
#define VBUS_LED_Pin GPIO_PIN_1
#define VBUS_LED_GPIO_Port GPIOB
#define CC_aa_Pin GPIO_PIN_2
#define CC_aa_GPIO_Port GPIOB
#define CC_ab_Pin GPIO_PIN_3
#define CC_ab_GPIO_Port GPIOB
#define CC_bb_Pin GPIO_PIN_4
#define CC_bb_GPIO_Port GPIOB
#define CC_ba_Pin GPIO_PIN_5
#define CC_ba_GPIO_Port GPIOB
#define E_MARKER_LED_Pin GPIO_PIN_6
#define E_MARKER_LED_GPIO_Port GPIOB
#define CC_Rd_LED_Pin GPIO_PIN_7
#define CC_Rd_LED_GPIO_Port GPIOB
#define CC_Rp_LED_Pin GPIO_PIN_8
#define CC_Rp_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

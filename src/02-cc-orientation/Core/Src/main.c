/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */


// flags
uint8_t gnd_conn = 0;
uint8_t vbus_conn = 0;
uint8_t cc_conn_aa = 0;
uint8_t cc_conn_ab = 0;
uint8_t cc_conn_ba = 0;
uint8_t cc_conn_bb = 0;
uint8_t Rp_conn = 0;
uint8_t Rd_conn = 0;
uint8_t Ra_conn = 0;
uint8_t flag_firstrun = 1;
uint16_t RP_THRESH = 400;
int analog_val1;
int analog_val2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void ADC_Select04(void) {
	  // Configure PA4
	  ADC_ChannelConfTypeDef sConfig = {0};

	  sConfig.Channel = ADC_CHANNEL_4;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;

	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

static void ADC_Select05(void) {
	  // Configure PA5
	  ADC_ChannelConfTypeDef sConfig = {0};

	  sConfig.Channel = ADC_CHANNEL_5;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
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
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  // Note reconfigured GND_SENSE_Pin as Pullup in MX_GPIO_INIT() function above; all others are floating

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // #bug ground LED "latches" on  →Need to set GND_Sense as pullup each time?

	  // ------ Section 1 - Testing Ground Line ---------
	  // Requires: GND_SENSE pin as pullup input, SW2B == CLOSED (B_GND_B1 → GND)
	  // Check if the A side ground pin is pulled to ground (short), else HIGH via internal Rp
	  if (HAL_GPIO_ReadPin(GPIOA, GND_SENSE_Pin) == GPIO_PIN_RESET) {
		  gnd_conn = 1;
	  }
	  else {
		  gnd_conn = 0;
	  }

	  // ------ Section 2 - Testing VBUS Line ---------
	  // Requires: SW2A == Closed (VBUS_A4 → VDD via 2K)
	  // B_VBUS is pulled up to Vdd via 2K; A_VBUS_SENSE is pulled down via 10K; Vsense = 10K/12K = HIGH
	  if (HAL_GPIO_ReadPin(GPIOA, VBUS_SENSE_Pin) == GPIO_PIN_SET ) {
	    vbus_conn = 1;
	  }
	  else {
	    vbus_conn = 0;
	  }

	  // ------ Section 3 - Test for USB C <> USB C --------
	  // Test Requires: A_CC1 == HIGH & A_CC2_Pin, CCx_CTRL_PIN == LOW
	  // Check B_CCx_Sense == A_CC1 with A_CC1 HIGH and all others LOW
	  // TODO check A_CC1 is configured already in MX init
	    // Configure A_CC1 as GPIO output HIGH
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    HAL_GPIO_WritePin(GPIOA, A_CC1_Pin|A_CC2_Pin|B_CC1_SENSE_Pin|B_CC2_SENSE_Pin|CC1_CTRL_Pin|CC2_CTRL_Pin, GPIO_PIN_RESET); // reset before config
	    GPIO_InitTypeDef GPIO_InitStruct = {0};

	    // Configure A_CC1 as Output HIGH
	    GPIO_InitStruct.Pin = A_CC1_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	    HAL_GPIO_WritePin(GPIOA, A_CC1_Pin, GPIO_PIN_SET); // A_CC1 → HIGH

	    // Configure B_CCx_Sense pins as floating inputs
	    GPIO_InitStruct.Pin = B_CC1_SENSE_Pin | B_CC2_SENSE_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	    // Configure pins as output LOW (A_CC2, CCx_CTRL)
	    GPIO_InitStruct.Pin = (A_CC2_Pin|CC1_CTRL_Pin|CC2_CTRL_Pin);
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	    // pin already reset previously
	    // Test 3A STD>STD or STD>FLIP: if B_CCx_Sense is HIGH then it is connected to A_CC1
	    if (HAL_GPIO_ReadPin(GPIOA, B_CC1_SENSE_Pin) == 1) {
	    	cc_conn_aa = 1; // STD>STD is A_CC1→B_CC1
	    }
	    else {
	    	cc_conn_aa = 0;
	    }
	    if (HAL_GPIO_ReadPin(GPIOA, B_CC2_SENSE_Pin) == 1) {
	    	cc_conn_ab = 1; // STD>FLIP is A_CC1→B_CC2
	    }
	    else {
	    	cc_conn_ab = 0;
	    }
	    // TEST 3B FLIP>STD or FLIP>FLIP: with A_CC2 HIGH if B_CCx_Sense HIGH then → A_CC2
	    // Requires: Set pins = [ACC2], RESET = [A_CC1, CCx_CTRL]
	    HAL_GPIO_WritePin(GPIOA, A_CC1_Pin, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(GPIOA, A_CC2_Pin, GPIO_PIN_SET);
	    if (HAL_GPIO_ReadPin(GPIOA, B_CC1_SENSE_Pin) == 1) {
	    	cc_conn_ba = 1; // A_CC2→B_CC1 is FLIP>STD
	    }
	    else {
	    	cc_conn_ba = 0;
	    }
	    if (HAL_GPIO_ReadPin(GPIOA, B_CC2_SENSE_Pin) == 1) {
	    	cc_conn_bb = 1; // A_CC2→B_CC2 is FLIP>FLIP
	    }
	    else {
	    	cc_conn_bb = 0;
	    }

	    // ------- Section 4: Test Active Cable (E Marker) ------

	    // Test 4: Assuming aa or ab then we know VCONN would be on A_CC2
	    // Requires: A_CC2 as INPUT_PULLUP
	    GPIO_InitStruct.Pin = A_CC2_Pin | A_CC1_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	    //
	    if ( cc_conn_aa || cc_conn_ab ) {
	    	if (HAL_GPIO_ReadPin(GPIOA, A_CC2_Pin) == 0) {
	    		Ra_conn = 1; // if pulled to ground then active cable
	    	}
	    	else {
	    		Ra_conn = 0; // if we get here then it is missing
	    	}
	    }
	    else if ( cc_conn_bb || cc_conn_ba ) {
	    	if (HAL_GPIO_ReadPin(GPIOA, A_CC1_Pin) == 0) {
	    		Ra_conn = 1;
	    	}
	    	else {
	    		Ra_conn = 0;
	    	}
	    }
	    else {
	    	Ra_conn = 0;
	    }
	    // Set A side back to output -- TODO delete if dont' need
//	    GPIO_InitStruct.Pin = A_CC2_Pin | A_CC1_Pin;
//	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
//	    GPIO_InitStruct.Pull = GPIO_NOPULL;
//	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//	    HAL_GPIO_WritePin(GPIOA, A_CC1_Pin | A_CC2_Pin, GPIO_PIN_RESET);

	    // Test 5 & 6 - Type C to Type A or Type B Pullup or Pulldown
	    // Only test if cc not connected
	    if ( cc_conn_aa || cc_conn_ab || cc_conn_ba || cc_conn_bb == 0) {
	    	// Test 5 - Type C to B Pulldown 5K6
	    	// Requires: SW2B CLOSED, A_CC1 INPUT PULLUP
	    	// Result: If ACCx digital read is LOW then pulldown exists
	    	if ( (HAL_GPIO_ReadPin(GPIOA, A_CC1_Pin) == 0) || (HAL_GPIO_ReadPin(GPIOA, A_CC2_Pin) == 0) ) {
	    		Rd_conn = 1;
	    		// TODO/future also add a orientation test for the C side but it would require a seperate "is_usb_c-c" flag since we would need to use the conn_aa/ab flag here and thus not enter the loop again
	    	}
	    	else {
	    		Rd_conn = 0;
	    	}

	    	// Test 6 - Type C to A Pullup Rp 56K - reqs ADC
	    	// Configure CCx_CTRL Pins as Output Low
		    GPIO_InitStruct.Pin = CC1_CTRL_Pin | CC2_CTRL_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		    GPIO_InitStruct.Pull = GPIO_NOPULL;
		    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		    HAL_GPIO_WritePin(GPIOA, CC1_CTRL_Pin | CC2_CTRL_Pin, GPIO_PIN_RESET);

	    	// Configure B_CCx_Sense Pins as analog
	    	GPIO_InitStruct.Pin = B_CC1_SENSE_Pin | B_CC2_SENSE_Pin;
	    	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	    	GPIO_InitStruct.Pull = GPIO_NOPULL;
	    	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    	// Configure ADC for B_CC1_Sense Pin PA4
			ADC_Select04();
			HAL_ADC_Start(&hadc1); // restart since we stopped it earlier
			// start, poll, assign
		    HAL_ADC_PollForConversion(&hadc1, 500U);
			analog_val1 = HAL_ADC_GetValue(&hadc1); // Example ADC reads with STLink 3.3V supply: LOW=130, Rp=500, HIGH=3000
			HAL_ADC_Stop(&hadc1);

			ADC_Select05();
			HAL_ADC_Start(&hadc1);
			// start, poll, assign
		    HAL_ADC_PollForConversion(&hadc1, 500U);
			analog_val2 = HAL_ADC_GetValue(&hadc1); // Example ADC reads with STLink 3.3V supply: LOW=130, Rp=500, HIGH=3000
			HAL_ADC_Stop(&hadc1);
			// #here - only one side changes analog reads - probably getting the same analog read register
			// see discontniuous mode https://community.st.com/t5/stm32cubemx-mcus/reading-multiple-adc-channel/td-p/171369

			if ( (analog_val1 > RP_THRESH) || (analog_val2) > RP_THRESH ) {
				Rp_conn = 1;
			}
			else {
				Rp_conn = 0;
			}


			// #here Rp sense in one direction

	  // ------ Section X - Drive LEDs -----------
	  // Port A: [8..0] is p CC_Rp_LED_Pin CC_Rd_LED_Pin ... VBUS_LED_Pin GND_LED_Pin ]
	  uint16_t led_mask = (
			  gnd_conn << 0 |
			  vbus_conn << 1 |
			  cc_conn_aa << 2 |
			  cc_conn_ab << 3 |
			  cc_conn_bb << 4 |
			  cc_conn_ba << 5 |
			  Ra_conn << 6 |
			  Rd_conn << 7 |
			  Rp_conn << 8
			  );

	  // set the outputs to the current value of the flags (note no mask)
	  GPIOB->ODR = ((uint16_t)led_mask); // set bits 0 to 8
	  //HAL_Delay(10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_7CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  // Custom Configuration for B_CC1_SENSE_Pin=PA4 and B_CC2_SENSE_Pin=PA5
  // TODO/later Enable multiple scan mode
//  hadc1.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
//  hadc1.Init.NbrOfConversion = 2; // 2 channels
//  hadc1.Init.DiscontinuousConvMode = DISABLE; // single conversion

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CC1_CTRL_Pin|CC2_CTRL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GND_LED_Pin|VBUS_LED_Pin|CC_aa_Pin|CC_ab_Pin
                          |CC_bb_Pin|CC_ba_Pin|E_MARKER_LED_Pin|CC_Rd_LED_Pin
                          |CC_Rp_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GND_SENSE_Pin VBUS_SENSE_Pin */
  GPIO_InitStruct.Pin = GND_SENSE_Pin|VBUS_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CC1_CTRL_Pin CC2_CTRL_Pin */
  GPIO_InitStruct.Pin = CC1_CTRL_Pin|CC2_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GND_LED_Pin VBUS_LED_Pin CC_aa_Pin CC_ab_Pin
                           CC_bb_Pin CC_ba_Pin E_MARKER_LED_Pin CC_Rd_LED_Pin
                           CC_Rp_LED_Pin */
  GPIO_InitStruct.Pin = GND_LED_Pin|VBUS_LED_Pin|CC_aa_Pin|CC_ab_Pin
                          |CC_bb_Pin|CC_ba_Pin|E_MARKER_LED_Pin|CC_Rd_LED_Pin
                          |CC_Rp_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  // Reconfigure GND_LED_Pin as INPUT_PULLUP (couldn't figure out how to do this in the viewer)
  GPIO_InitStruct.Pin = GND_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // <---
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/* USER CODE END MX_GPIO_Init_2 */
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

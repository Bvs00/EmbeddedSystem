/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
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
#define RST_Pin GPIO_PIN_0
#define RST_GPIO_Port GPIOC
#define MFIO_Pin GPIO_PIN_1
#define MFIO_GPIO_Port GPIOC
#define Temperature_Pin GPIO_PIN_0
#define Temperature_GPIO_Port GPIOA
#define RelaxedLed_Pin GPIO_PIN_8
#define RelaxedLed_GPIO_Port GPIOA
#define AlertBuzzer_Pin GPIO_PIN_9
#define AlertBuzzer_GPIO_Port GPIOA
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

// We define the all treshould
enum Threshould{
	// alert thresholds
	goodMeasureTime = 10,
	goodNumberMeasure = 7,
	countFail = 5,
	highTemperature = 36,	// 36
	highHeartRate = 120,	// 120
	lowOxygen = 96,	// 96
	// confidence range
	lowConfTemp = 20,
	highConfTemp = 45,
	lowConfHeartRate = 50,
	highConfHeartRate = 220,
	lowConfOxygen = 95,
	highConfOxygen = 101,
	acceptableConfidenceHeartRate = 95
};

enum animationDuration{ //all are indicate in ms, convert them as necessary
	showDangerDuration = 6000,	// must be a odd value
	timeInhalationAndExhalation = 5000,
	repetitionBreathing = 5,
	repetitionDuration = 5000
};

typedef struct{
	bool highTemperatureFlag;
	bool highHeartRateFlag;
	bool lowOxygenFlag;
	bool dangerShowing;
	bool inhalation;
	bool exhalation;
}Flag;

typedef struct{
	bool tachycardia;
	bool hypoxemia;
	bool arrhythmia;
	bool covid;
	bool fever;
	bool highFever;
	bool highestFever;
}Disease;

typedef struct{
	// count
	uint32_t countTot;
	uint32_t goodTemp;
	uint32_t goodHeartRate;
	uint32_t goodOxygen;
	// count per il Led e Buzzer e showingTime
	uint32_t countDangerShowing;
	uint32_t ledCounter;
	uint32_t repetition;
	// count per avviso posizionamento dita
	uint32_t badValueTemp;
	uint32_t badValueHeartRate;
	uint32_t badValueOxygen;
	// cumulative
	float sumTemp;
	uint32_t sumHeartRate;
	uint32_t sumOxygen;
	// Average Value
	float averageValueTemp;
	uint32_t averageValueHeartRate;
	uint32_t averageValueOxygen;
}Measure;


Flag* getFlag(void);
Measure* getMeasure(void);
Disease* getDisease(void);

void FLAG_Init(void);
void MEASURE_Init(void);
void DISEASE_Init(void);

void resetValue(void);
void resetAvgValue(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

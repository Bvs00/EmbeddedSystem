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
#include "MAX32664.h"
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
#define RelaxLed_Pin GPIO_PIN_0
#define RelaxLed_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

// We define the all treshould
enum Threshould{
	// alert tresholds
	goodMeasureTime = 10,
	goodNumberMeasure = 6,
	countFail = 5,
	highTemperature = 37,
	highHeartRate = 120,
	lowOxygen = 97,
	// confidence range
	lowConfTemp = 20,
	highConfTemp = 45,
	lowConfHeartRate = 50,
	highConfHeartRate = 220,
	lowConfOxygen = 95,
	highConfOxygen = 101
};

typedef struct{
	bool failCount;  	// flag da attivare quando abbiamo un elevato numero di conteggi senza risultati
	bool lowFever;      // flag da attivare in caso di febbre
	bool tachycardia;  	// flag da attivare quando si ha un heartRate alto
	bool hypoxemia;    	// flag da attivare quando si ha una bassa ossigenazione
	bool covid;			// flag da attivare quando si ha una bassa ossigenazione e un alta temperatura corporea
	bool middleFever; 	// flag da attivare quando si ha un'alta temperatura e un alto heartRate
	bool arrhythmia;	// flag da attivare quando si ha battito alto e bassa ossigenzione
	bool highFever;		// flag da attivare quando si ha temperatura alta, bassa ossigenazione e battiti alti

	// buone (forse)
	bool highTemperatureFlag;
	bool highHeartRateFlag;
	bool lowOxygenFlag;



}Flag;

typedef struct{
	// count
	uint32_t countTot;
	uint32_t goodTemp;
	uint32_t goodHeartRate;
	uint32_t goodOxygen;
	// count per il Led e Buzzer
	uint32_t ledCounter;
	uint32_t noteCount;
	// count per avviso posizionamento dita
	uint32_t badValueTemp;
	uint32_t badValueHeartRate;
	uint32_t badValueOxygen;
	// cumulative
	uint32_t sumTemp;
	uint32_t sumHeartRate;
	uint32_t sumOxygen;
	// Average Value
	uint32_t averageValueTemp;
	uint32_t averageValueHeartRate;
	uint32_t averageValueOxygen;
}Measure;

typedef struct{
	MAX32664 max;
}Sensor;


Flag* getFlag(void);
Measure* getMeasure(void);
Sensor* getSensor(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

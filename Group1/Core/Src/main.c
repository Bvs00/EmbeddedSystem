/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "KY028.h"
#include "action.h"
#include "MAX32664.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


Flag* getFlag(void){
	static Flag flag;
	return &flag;
}

Measure* getMeasure(void){
	static Measure measure;
	return &measure;
}


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);  // Inizializzazione PWM canale 1
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);  // Inizializzazione PWM canale 2
  HAL_TIM_Base_Start_IT(&htim10);  // Inizializzazione TIM BASE 10
  HAL_TIM_Base_Start_IT(&htim11);  // Inizializzazione TIM BASE 11

  // inizializzare MAX
  MAX32664* max = getSensor();
  begin(max, &hi2c1, GPIOC, GPIOC, GPIO_PIN_0, GPIO_PIN_1);
  config_sensor(max, MODE_ONE);

  // PollingMode per l'ADC
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
/*
 * Di seguito facciamo la callback per il timer
 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM10){

		Flag* flags = getFlag();
		Measure* measure = getMeasure();
		MAX32664* max = getSensor();

		uint32_t previousValue;

		measure->countTot += 1;

		// attivo il sensore della temperatura, che è gia attivato dal polling
		uint32_t temperature = getTemperature();
		if ((temperature< highConfTemp) && (temperature >lowConfTemp)){
			measure->sumTemp += temperature;
			measure->goodTemp++;
		}
		// attivo il sensore MAX32664
		read_sensor(max);
		if(max->algorithm_state == 3){

			uint32_t heartRate = max->heart_rate;
			uint32_t oxygen = max->oxygen;

			if ((heartRate < highConfHeartRate) && (heartRate > lowConfHeartRate)){
				measure->sumHeartRate += heartRate;
				measure->goodHeartRate++;
			}
			if ((oxygen < highConfOxygen) && (oxygen > lowConfOxygen)){
				measure->sumOxygen += oxygen;
				measure->goodOxygen++;
			}
		}

		// Qui ho tutte le somme delle misure
		// Devo controllare se ho finito il numero di misurazioni
		// In questa fase attivo le flag e solo qui dentro mostro le azioni

		if (measure->countTot >= goodMeasureTime){  // se ho effettuato le misurazioni per tempo = a goodMeasureTime

			// controllo se le misurazioni del tempo possono essere valutate
			if (measure->goodTemp >= goodNumberMeasure){
				measure->badValueTemp = 0;

				computeAverageTemp();
				if (measure->averageValueTemp < highTemperature){
					flags->highTemperatureFlag = false;
					// mostrare a video il suo contenuto
				}else{
					// conseguenza di avere la febbre
					flags->highTemperatureFlag = true;
				}
				// posso calcolare il valore medio della misura del tempo e mostrarla a video
			}else{
				measure->badValueTemp++;
				// mostro a video il simbolo nullo
			}

			// controllo se le misurazioni del battito cardiaco possono essere valutate
			if (measure->goodHeartRate >= goodNumberMeasure){
				measure->badValueHeartRate = 0;
				// posso calcolare il valore medio della misura del battito cardiaco e mostrarla a video
			}else{
				measure->badValueHeartRate++;
				// mostro a video il simbolo nullo
			}

			// controllo se le misurazioni dell'ossigenazione possono essere valutate
			if (measure->goodOxygen >= goodNumberMeasure){
				measure->badValueOxygen = 0;
				// posso calcolare il valore medio della misura dell'ossigenazione e mostrarla a video
			}else{
				measure->badValueOxygen++;
				// mostro a video il simbolo nullo
			}

		}
		// controllo se i badValue sono pari a countFail per far in modo che compaia una schermata di riposizionamento dito
		if ((measure->badValueHeartRate >= countFail) || (measure->badValueOxygen >= countFail) || (measure->badValueTemp >= countFail)){
			// devo mostrare la schermata di posizionamento dito
		}




		// Lettura temperatura



		// Lettura Cardio


		// Lettura Ossigenazione



		// if-else di controllo
		if(flags->arrhythmia){

		}else if(flags->covid){

		}else{

		}
		if(flags->failCount){

		}






	}

	// TIM11
	if (htim->Instance == TIM11){

			if ((TIM1->CCER & TIM_CCER_CC1E) != 0){
				Measure* measures = getMeasure();



				uint32_t timeOffOnComplete = 20;
				uint32_t timeInspirationOrEspriration = 4500;
				uint32_t intervall = 1000;
				uint32_t passValue;

				if (measures->ledCounter >= 2*(timeInspirationOrEspriration - 1 )){
					measures->ledCounter = -1 * intervall;
				}
				if (measures->ledCounter >= (timeInspirationOrEspriration - 1)){
					passValue = measures->ledCounter - 2 * (measures->ledCounter - (timeInspirationOrEspriration - 1 ));
				}else{
					passValue = measures->ledCounter;
				}

				if ((passValue <= 0 || passValue >= (timeInspirationOrEspriration -1 - (timeOffOnComplete/2)))){
					passValue = 0;
				}else{
					passValue = ((passValue * 100) / (timeInspirationOrEspriration - 1 ));
				}

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, passValue);

				measures->ledCounter += 1;
			}

			if ((TIM1->CCER & TIM_CCER_CC2E) != 0){
				Measure* measures = getMeasure();

				uint32_t interval = 200;

				if (measures->noteCount >= 0 * interval && measures->noteCount < 1 * interval){
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)30);

				}else if(measures->noteCount >= 1 * interval && measures->noteCount < 2 * interval){
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)60);

				}else if(measures->noteCount >= 2 * interval && measures->noteCount < 3 * interval){
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)90);

				}else if(measures->noteCount >= 3 * interval){
					measures->noteCount = 0;
				}

				measures->noteCount++;
			}


	}


}




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

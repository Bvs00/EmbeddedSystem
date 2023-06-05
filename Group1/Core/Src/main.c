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
#include "ssd1306.h"
#include "ds1307rtc.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/*
 * This function return the Pointer to the Flag structure.
 * This function create a static pointer only once.
 */
Flag* getFlag(void){
	static Flag flag;
	return &flag;
}

/*
 * This function return the Pointer to the Measure structure.
 * This function create a static pointer only once.
 */
Measure* getMeasure(void){
	static Measure measure;
	return &measure;
}

/*
 * This function return the Pointer to the Disease structure.
 * This function create a static pointer only once.
 */
Disease* getDisease(void){
	static Disease diseases;
	return &diseases;
}


/*
 * Flag init function
 */
void FLAG_Init(void){
	Flag* flags = getFlag();
	flags->highHeartRateFlag = 0;
	flags->highTemperatureFlag = 0;
	flags->lowOxygenFlag = 0;
	flags->dangerShowing = 0;
	flags->inhalation = 0;
	flags->exhalation = 0;
}

void DISEASE_Init(void){
	Disease* diseases = getDisease();
	diseases->arrhythmia = false;
	diseases->covid = false;
	diseases->fever = false;
	diseases->highFever = false;
	diseases->highestFever = false;
	diseases->hypoxemia = false;
	diseases->tachycardia = false;
}

/*
 * Measure init function
 */
void MEASURE_Init(void){
	Measure* measure = getMeasure();
	measure->averageValueHeartRate = 0;
	measure->averageValueOxygen = 0;
	measure->averageValueTemp = 0;
	measure->badValueHeartRate = 0;
	measure->badValueOxygen = 0;
	measure->badValueTemp = 0;
	measure->countTot = 0;
	measure->countDangerShowing = 0;
	measure->goodHeartRate = 0;
	measure->goodOxygen = 0;
	measure->goodTemp = 0;
	measure->ledCounter = 0;
	measure->repetition = 0;
	measure->sumHeartRate = 0;
	measure->sumOxygen = 0;
	measure->sumTemp = 0;
}

/*
 * Reset temporary values of the Measure structure
 */
void resetValue(void){
	Measure* measure = getMeasure();
	measure->sumHeartRate = 0;
	measure->sumOxygen = 0;
	measure->sumTemp = 0;
	measure->goodHeartRate = 0;
	measure->goodOxygen = 0;
	measure->goodTemp = 0;
}

/*
 * Reset average values of the Measure structure
 */
void resetAvgValue(void){
	Measure* measure = getMeasure();
	measure->averageValueHeartRate = 0;
	measure->averageValueOxygen = 0;
	measure->averageValueTemp = 0;
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//
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
  //HAL_TIM_Base_Start_IT(&htim11);  // Inizializzazione TIM BASE 11
  HAL_ADC_Start(&hadc1);			// Inizializzazione ADC


  // Init Oled_SSD1306
  ssd1306_Init();


  // Init MAX32664
  MAX32664* max = getSensor();

  char mess[] = "PULSE NOK\n\r";
  char mess2[] = "PULSE OK\n\r";

  HAL_UART_Transmit(&huart2, mess, strlen(mess), HAL_MAX_DELAY);

  if(begin(max, &hi2c1, GPIOC, GPIOC, GPIO_PIN_0, GPIO_PIN_1) != OK || config_sensor(max, MODE_ONE) != OK){
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
	  HAL_UART_Transmit(&huart2, mess, strlen(mess), HAL_MAX_DELAY);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
  HAL_UART_Transmit(&huart2, mess2, strlen(mess2), HAL_MAX_DELAY);


  //Init Measure, Flag and Disease Structures
  FLAG_Init();
  MEASURE_Init();
  DISEASE_Init();

  // DATETIME
  DateTime datetime;

  ds1307rtc_init();

  datetime.seconds=40;
  datetime.minutes=04;
  datetime.hours=17;
  datetime.day=2;
  datetime.year=23;
  datetime.month=6;
  datetime.date=5;

  ds1307rtc_set_date_time(&datetime);



  // Define the ADC conversion in PollingMode
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

  // Active the Oled_SSD1306
  showMeasures();

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
 * Callback of the timer.
 * In this callback I check that my interrupt is generated by the TIM10
 * I take into account each measure done for the complete measure.
 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	// Basic Timer 10 is the timer that allow the new measurement
	if (htim->Instance == TIM10){

		Flag* flags = getFlag();
		Measure* measure = getMeasure();
		Disease* diseases = getDisease();
		MAX32664* max = getSensor();
		DateTime datetime;

		float temperature;
		uint32_t previousValueTemp, previousValueHeartRate, previousValueOxygen;

		char valueTmp[20], valueHeart[20], valueOxygen[20];


		measure->countTot += 1;

		// Put the value of temperature in this variable, from the function 'getTemperature'
		temperature = getTemperature();
		// I check that the value of temperature is between a min value and a max value
		// to don't take into account the out-of-bounds value for the mean.
		if ((temperature< highConfTemp) && (temperature >lowConfTemp)){
			measure->sumTemp += temperature;
			measure->goodTemp++;
		}

		// Read value of MAX32664
		read_sensor(max);

		uint32_t heartRate = max->heart_rate;
		uint32_t oxygen = max->oxygen;
		uint32_t confidence_heart_rate = max->confidence_heart_rate;

		// Check that the value of heart rate is acceptable
		if ((heartRate < highConfHeartRate) && (heartRate > lowConfHeartRate) && (confidence_heart_rate > acceptableConfidenceHeartRate)){
			measure->sumHeartRate += heartRate;
			measure->goodHeartRate++;
		}
		// Check that the value of oxygen is acceptable
		if ((oxygen < highConfOxygen) && (oxygen > lowConfOxygen)){
			measure->sumOxygen += oxygen;
			measure->goodOxygen++;
		}

		// BASTARDATA DEL SECOLO MI VEDO I VALORI CHE HO


		sprintf(valueTmp, "Temperatura: %.2f\n\r", temperature);
		HAL_UART_Transmit(&huart2,valueTmp , strlen(valueTmp), HAL_MAX_DELAY);

		sprintf(valueHeart, "HeartRate: %d\n\r", heartRate);
		HAL_UART_Transmit(&huart2,valueHeart , strlen(valueHeart), HAL_MAX_DELAY);

		sprintf(valueOxygen, "Oxygen: %d\n\r", oxygen);
		HAL_UART_Transmit(&huart2,valueOxygen , strlen(valueOxygen), HAL_MAX_DELAY);

		sprintf(valueTmp, "COUNT: %d\r\n", measure->countTot);
		HAL_UART_Transmit(&huart2, valueTmp, strlen(valueTmp), HAL_MAX_DELAY);


		// I show the measure on the screen only when I reach the 'goodMeasureTime'
		if (measure->countTot >= goodMeasureTime){

			char str[] = "Nuova misura disponibile\r\n";
			HAL_UART_Transmit(&huart2, str, strlen(str), HAL_MAX_DELAY);


			// Check that the measures of temperature are enough
			if (measure->goodTemp >= goodNumberMeasure){

				measure->badValueTemp = 0;

				previousValueTemp = measure->averageValueTemp;

				computeAverageTemp();

				// If my average value is over the threshold, set the flag = true
				flags->highTemperatureFlag = (measure->averageValueTemp < highTemperature) ? false : true;

			}else{
				// If the measures is not enough, increment the counter for error measure and reset the average value
				measure->badValueTemp++;
				measure->averageValueTemp = 0;     // In this way on my screen I will show that the measure is wrong
				flags->highTemperatureFlag = false;
			}

			// Check that the measures of heart rate are enough
			if (measure->goodHeartRate >= goodNumberMeasure){

				measure->badValueHeartRate = 0;

				previousValueHeartRate = measure->averageValueHeartRate;

				computeAverageHeartRate();

				// If my average value is over the threshold, set the flag = true
				flags->highHeartRateFlag = (measure->averageValueHeartRate < highHeartRate) ? false : true;

			}else{
				// If the measures is not enough, increment the counter for error measure and reset the average value
				measure->badValueHeartRate++;
				measure->averageValueHeartRate = 0;			// In this way on my screen I will show that the measure is wrong
				flags->highHeartRateFlag = false;
			}

			// Check that the measures of oxygen are enough
			if (measure->goodOxygen >= goodNumberMeasure){

				measure->badValueOxygen = 0;

				previousValueOxygen = measure->averageValueOxygen;

				computeAverageOxygen();

				// If my average value is over the threshold, set the flag = true
				flags->lowOxygenFlag = (measure->averageValueOxygen < lowOxygen) ? true : false;

			}else{
				// If the measures is not enough, increment the counter for error measure and reset the average value
				measure->badValueOxygen++;
				measure->averageValueOxygen = 0;		// In this way on my screen I will show that the measure is wrong
				flags->lowOxygenFlag = false;
			}

			resetValue();		// This line is important because for each measurement we initialize the value to compute the averages
		}


		// I check that the count: 'badValue' for all measure (temperature, heart_rate and oxygen) is enough to show the error finger position
		if (((measure->badValueHeartRate >= countFail) || (measure->badValueOxygen >= countFail) || (measure->badValueTemp >= countFail)) && (measure->countTot >= goodMeasureTime)){
			showReplaceFinger();
			resetAvgValue();		// This line is important because I must reset the avg value, otherwise I would still have these value
			measure->countTot = 0;	// This line is important because I must set to 0 the counter else if I'm in this state,
			//I will have a loop because compute always the avg values but I have only one value.

		}else if(measure->countTot >= goodMeasureTime){
			// If the values are good I can show the averages value and I must check which flags are active
			measure->countTot = 0;
			// 0-0-0
			if (!flags->highTemperatureFlag && !flags->highHeartRateFlag && !flags->lowOxygenFlag){
				// TUTTO BENE
				showTerminalTime(datetime);
				showMeasures();
			}
			// 0-0-1
			if (!flags->highTemperatureFlag && !flags->highHeartRateFlag && flags->lowOxygenFlag){
				// IPOSSIEMIA
				// check that isn't a first time
				if(previousValueOxygen < lowOxygen){ // if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();
				}else{
					// show the danger screen (IPOSSIEMIA)
					flags->dangerShowing = true;
					diseases->hypoxemia = true;
					showDangerIpossimeia();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}

			}

			// 0-1-0
			if (!flags->highTemperatureFlag && flags->highHeartRateFlag && !flags->lowOxygenFlag){
				// TACHICARDIA
				// check that isn't a first time
				if(previousValueHeartRate > highHeartRate){ // if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();
				}else{
					// show the danger screen (TACHICARDIA)
					flags->dangerShowing = true;
					diseases->tachycardia = true;
					showDangerTachicardia();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}
			}

			// 0-1-1
			if (!flags->highTemperatureFlag && flags->highHeartRateFlag && flags->lowOxygenFlag){
				// ARITMIA
				// check that isn't a first time
				if ((previousValueHeartRate > highHeartRate) && (previousValueOxygen < lowOxygen)){  // if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();

				}
				else{
					// show the danger screen (ARITMIA)
					flags->dangerShowing = true;
					diseases->arrhythmia = true;
					showDangerAritmia();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}
			}

			// 1-0-0
			if (flags->highTemperatureFlag && !flags->highHeartRateFlag && !flags->lowOxygenFlag){
				// FEBBRE
				// check that isn't a first time
				if(previousValueTemp > highTemperature){   // if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();

				}else{
					// show the danger screen (FEBBRE)
					flags->dangerShowing = true;
					diseases->fever = true;
					showDangerFebbre();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}
			}

			// 1-0-1
			if (flags->highTemperatureFlag && !flags->highHeartRateFlag && flags->lowOxygenFlag){
				// COVID
				// check that isn't a first time
				if ((previousValueTemp > highTemperature) && (previousValueOxygen < lowOxygen)){	// if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();

				}
				else{
					// show the danger screen (COVID)
					flags->dangerShowing = true;
					diseases->covid = true;
					showDangerCovid();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}

			}
			// 1-1-0
			if (flags->highTemperatureFlag && flags->highHeartRateFlag && !flags->lowOxygenFlag){
				// FEBBRE ALTA
				// check that isn't a first time
				if ((previousValueTemp > highTemperature) && (previousValueHeartRate > highHeartRate)){		// if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();

				}
				else{
					// show the danger screen (FEBBRE ALTA)
					flags->dangerShowing = true;
					diseases->highFever = true;
					showDangerFebbreAlta();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}

			}
			// 1-1-1
			if (flags->highTemperatureFlag && flags->highHeartRateFlag && flags->lowOxygenFlag){
				// FEBBRE MOLTO ALTA
				// check that isn't a first time
				if ((previousValueTemp > highTemperature) && (previousValueHeartRate > highHeartRate) && (previousValueOxygen < lowOxygen)){	// if isn't a first time, I don't show the danger screen
					showTerminalTime(datetime);
					showMeasures();

				}
				else{
					// show the danger screen (FEBBRE MOLTO ALTA)
					flags->dangerShowing = true;
					diseases->highestFever = true;
					showDangerFebbreMoltoAlta();
					HAL_TIM_Base_Stop_IT(&htim10);
					HAL_TIM_Base_Start_IT(&htim11);
				}
			}

		}

	}

	// TIM11  -> 1ms
	if (htim->Instance == TIM11){

		// in 5 secondi devo respirare, poi aspetto 1 secondo ed infine altri 5 secondi espiro, ed aspetto 1 secondo per riiniziare. Il tutto viene fatto 5 volte.
		// PWM del TIM1 ha un period di 1000. Quindi devo raggiungere un pulse di 1000 alla fine dei 5 secondi.
		// Aumento di 0.2 il pulse ogni millisecondo.

		Measure* measures = getMeasure();
		Flag* flags = getFlag();
		Disease* diseases = getDisease();

		if (flags->dangerShowing){
			if (measures->countDangerShowing <= showDangerDuration){

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2500+measures->countDangerShowing);
				measures->countDangerShowing++;

			}else{
				flags->dangerShowing = false;
//				showInhalation();
				measures->countDangerShowing = 0;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, measures->countDangerShowing);
//				flags->exhalation = false;
//				flags->inhalation = true;
			}
		}else if(diseases->arrhythmia && !flags->dangerShowing){

			// mostra azione per aritmia
			diseases->arrhythmia = false;


		}else if(diseases->covid && !flags->dangerShowing){

			// mostra azione per covid
			diseases->covid = false;

		}else if(diseases->fever && !flags->dangerShowing){

			// mostra azione per fever
			diseases->fever = false;

		}else if(diseases->highFever && !flags->dangerShowing){

			// mostra azione per high fever
			diseases->highFever = false;

		}else if(diseases->highestFever && !flags->dangerShowing){

			// mostra azione per highest fever
			diseases->highestFever = false;

		}else if(diseases->hypoxemia && !flags->dangerShowing){

			// mostra azione per ipossiemia
			diseases->hypoxemia = false;

		}else if (diseases->tachycardia && !flags->dangerShowing && (measures->repetition <= repetitionBreathing)){
			if ((measures->ledCounter < timeInhalationAndExhalation) && (flags->inhalation)){

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, measures->ledCounter);

				measures->ledCounter++;

			}
			if (measures->ledCounter == timeInhalationAndExhalation){
				flags->exhalation = true;
				flags->inhalation = false;
				showExhalation();
			}
			if(measures->ledCounter == 0){
				flags->exhalation = false;
				flags->inhalation = true;
				measures->repetition++;
				showInhalation();
			}
			if((measures->ledCounter > 0) && (flags->exhalation)){

				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, measures->ledCounter);

				measures->ledCounter--;

			}
		}else{
			showMeasures();
			measures->repetition = 0;
			DISEASE_Init();
			HAL_TIM_Base_Stop_IT(&htim11);
			HAL_TIM_Base_Start_IT(&htim10);
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

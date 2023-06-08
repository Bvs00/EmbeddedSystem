#include "action.h"

const uint8_t heartPic[] = {
		// 'heartPic', 23x20px
		0x0f, 0x01, 0xe0, 0x3f, 0xc7, 0xf8, 0x7f, 0xef, 0xfc, 0x7f, 0xff, 0xfc, 0xff, 0xff, 0xfe, 0xff,
		0xff, 0xfe, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xfc, 0x7f, 0xff, 0xfc, 0x3f, 0xff,
		0xf8, 0x1f, 0xff, 0xf0, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0xc0, 0x03, 0xff, 0x80, 0x01, 0xff, 0x00,
		0x00, 0xfe, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x10, 0x00
};

const uint8_t tempPic[] = {
		// 'tempPic', 20x20px
		0x00, 0x60, 0x00, 0x00, 0xf0, 0x00, 0x01, 0x98, 0x00, 0x01, 0x68, 0x00, 0x01, 0x60, 0x00, 0x01,
		0x60, 0x00, 0x01, 0x60, 0x00, 0x01, 0x60, 0x00, 0x01, 0x68, 0x00, 0x01, 0x68, 0x00, 0x01, 0x68,
		0x00, 0x01, 0x68, 0x00, 0x01, 0x68, 0x00, 0x02, 0x64, 0x00, 0x02, 0xf4, 0x00, 0x06, 0xf6, 0x00,
		0x02, 0xf4, 0x00, 0x03, 0x0c, 0x00, 0x01, 0x98, 0x00, 0x00, 0xf0, 0x00
};

const uint8_t oxygenPic[] = {
		// 'oxygenPic', 20x20px
		0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x80, 0x00, 0x04, 0x80, 0x00, 0x07, 0x03, 0xc0, 0x00,
		0x06, 0x20, 0x00, 0x04, 0x10, 0x06, 0x04, 0x10, 0x10, 0xc5, 0x10, 0x20, 0x22, 0x20, 0x41, 0x01,
		0xc0, 0x80, 0x90, 0x00, 0x80, 0x40, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00,
		0x40, 0x20, 0x00, 0x20, 0x20, 0x00, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00,

};

const uint8_t alertPic[] = {
		// 'alertPic', 20x20px
		0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xf0, 0x00, 0x01, 0xf8, 0x00, 0x03, 0xfc, 0x00, 0x03,
		0x9c, 0x00, 0x07, 0x9e, 0x00, 0x07, 0x9e, 0x00, 0x0f, 0x9f, 0x00, 0x1f, 0x9f, 0x80, 0x1f, 0x9f,
		0x80, 0x3f, 0x9f, 0xc0, 0x3f, 0xff, 0xc0, 0x7f, 0x9f, 0xe0, 0xff, 0x0f, 0xf0, 0xff, 0x0f, 0xf0,
		0xff, 0xff, 0xf0, 0x7f, 0xff, 0xe0, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00

};


/*
 * Compute Average Value
 */

void computeAverageTemp(void){
	Measure* measures = getMeasure();
	measures->averageValueTemp = measures->sumTemp / measures->goodTemp;
}

void computeAverageHeartRate(void){
	Measure* measures = getMeasure();
	measures->averageValueHeartRate = measures->sumHeartRate / measures->goodHeartRate;

}

void computeAverageOxygen(void){
	Measure* measures = getMeasure();
	measures->averageValueOxygen = measures->sumOxygen / measures->goodOxygen;
}

/*
 * Show Single Screen
 */

void showMeasures(void){
	Flag* flags = getFlag();
	char print[20];

	ssd1306_Fill(Black);
	ssd1306_DrawBitmap(0, 0, tempPic, 20, 20, White);
	ssd1306_DrawBitmap(0, 22, heartPic, 23, 20, White);
	ssd1306_DrawBitmap(0, 44, oxygenPic, 20, 20, White);

	ssd1306_SetCursor(30, 5);
	compositeString(print,TEMPERATURE_MODE);
	ssd1306_WriteString(print, Font_7x10, White);

	ssd1306_SetCursor(30, 26);
	compositeString(print,HEART_RATE_MODE);
	ssd1306_WriteString(print, Font_7x10, White);

	ssd1306_SetCursor(30, 48);
	compositeString(print,OXYGEN_MODE);
	ssd1306_WriteString(print, Font_7x10, White);

	if (flags->highTemperatureFlag){
		ssd1306_DrawBitmap(108, 0, alertPic, 20, 20, White); //Temp Alert
	}
	if (flags->highHeartRateFlag){
		ssd1306_DrawBitmap(108, 22, alertPic, 20, 20, White); //Heart Alert
	}
	if (flags->lowOxygenFlag){
		ssd1306_DrawBitmap(108, 44, alertPic, 20, 20, White); //Oxygen Alert
	}
	ssd1306_UpdateScreen();
}

void compositeString(char* print, uint8_t mode){
	Measure* measures = getMeasure();

	if (mode == TEMPERATURE_MODE){
		if (measures->averageValueTemp != 0.0)
			sprintf(print, "%.2f C", measures->averageValueTemp);
		else
			strcpy(print, "- C");

	}
	else if (mode == HEART_RATE_MODE){
		if (measures->averageValueHeartRate != 0)
			sprintf(print, "%d BPM", measures->averageValueHeartRate);
		else
			strcpy(print, "- BPM");

	}else if (mode == OXYGEN_MODE){
		if (measures->averageValueOxygen != 0)
			sprintf(print, "%d%%", measures->averageValueOxygen);
		else
			strcpy(print, "- %");
	}

}

void showReplaceFinger(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 20);
	ssd1306_WriteString("Please replace", Font_7x10, White);
	ssd1306_SetCursor(45, 35);
	ssd1306_WriteString("finger", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showTerminalTime(DateTime datetime){
	char strDateTime[50];
	ds1307rtc_get_date_time(&datetime);
	sprintf(strDateTime,"La misura è stata fatta: %d/%d/%d - %d:%d:%d\n\r", datetime.date, datetime.month, datetime.year, datetime.hours, datetime.minutes, datetime.seconds);
	HAL_UART_Transmit(&huart2, strDateTime, strlen(strDateTime), HAL_MAX_DELAY);
}


/*
 * Show Danger Screen
 */

void showDangerHypoxemia(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 15);
	ssd1306_WriteString("Oxigen level", Font_7x10, White);
	ssd1306_SetCursor(17, 30);
	ssd1306_WriteString("in your blood", Font_7x10, White);
	ssd1306_SetCursor(37, 45);
	ssd1306_WriteString("are low", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerTachycardia(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(12, 15);
	ssd1306_WriteString("Your heart rate", Font_7x10, White);
	ssd1306_SetCursor(40, 30);
	ssd1306_WriteString("is very", Font_7x10, White);
	ssd1306_SetCursor(50, 45);
	ssd1306_WriteString("high", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerArrhythmia(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 5);
	ssd1306_WriteString("Oxigen Level", Font_7x10, White);
	ssd1306_SetCursor(50, 20);
	ssd1306_WriteString("and", Font_7x10, White);
	ssd1306_SetCursor(25, 35);
	ssd1306_WriteString("Heart Rate", Font_7x10, White);
	ssd1306_SetCursor(20, 50);
	ssd1306_WriteString("are critical", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerFever(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(7, 15);
	ssd1306_WriteString("Body temperature", Font_7x10, White);
	ssd1306_SetCursor(20, 30);
	ssd1306_WriteString("is very high", Font_7x10, White);
	ssd1306_SetCursor(15, 45);
	ssd1306_WriteString("probably fever", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerCovid(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(28, 15);
	ssd1306_WriteString("Your value", Font_7x10, White);
	ssd1306_SetCursor(40, 30);
	ssd1306_WriteString("predict", Font_7x10, White);
	ssd1306_SetCursor(46, 45);
	ssd1306_WriteString("covid", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerHighFever(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(28, 15);
	ssd1306_WriteString("Your value", Font_7x10, White);
	ssd1306_SetCursor(40, 30);
	ssd1306_WriteString("predict", Font_7x10, White);
	ssd1306_SetCursor(27, 45);
	ssd1306_WriteString("high fever", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showDangerHighestFever(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(28, 15);
	ssd1306_WriteString("Your value", Font_7x10, White);
	ssd1306_SetCursor(20, 30);
	ssd1306_WriteString("predict very", Font_7x10, White);
	ssd1306_SetCursor(27, 45);
	ssd1306_WriteString("high fever", Font_7x10, White);
	ssd1306_UpdateScreen();
}


/*
 * Decide which action you have to do
 */

void showWhichAction(void){
	Disease* diseases = getDisease();

	if (diseases->tachycardia)
		showWhichActionTachycardia();
	else if(diseases->arrhythmia)
		showWhichActionArrhythmia;
	else if(diseases->covid)
		showWhichActionCovid;
	else if(diseases->fever)
		showWhichActionFever;
	else if(diseases->highFever)
		showWhichActionHighFever;
	else if(diseases->highestFever)
		showWhichActionHighestFever;
	else if(diseases->hypoxemia)
		showWhichActionHypoxemia;

}

void showWhichActionTachycardia(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 15);
	ssd1306_WriteString("I will now assist you", Font_7x10, White);
	ssd1306_SetCursor(15, 30);
	ssd1306_WriteString("in a breathing", Font_7x10, White);
	ssd1306_SetCursor(35, 45);
	ssd1306_WriteString("exercise", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showWhichActionArrhythmia(void);

void showWhichActionCovid(void);

void showWhichActionFever(void);

void showWhichActionHighFever(void);

void showWhichActionHighestFever(void);

void showWhichActionHypoxemia(void);


/*
 * All Action that you have to do
 */

void showActionTachycardia(void){
	Measure* measures = getMeasure();
	Flag* flags = getFlag();

	if ((measures->ledCounter < timeInhalationAndExhalation) && (flags->inhalation)){

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, measures->ledCounter);

		measures->ledCounter++;

	}
	if (measures->ledCounter == timeInhalationAndExhalation){
		char puls[30];
		uint32_t pulse = __HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1);
		sprintf(puls, "PULSE = %d\n\r", pulse);
		HAL_UART_Transmit(&huart2, puls, strlen(puls), HAL_MAX_DELAY);

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
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
}


void showActionHypoxemia(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 10);
	ssd1306_WriteString("I recommend that you do", Font_7x10, White);
}

void showActionArrhythmia(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 10);
	ssd1306_WriteString("I recommend that you do", Font_7x10, White);
}

void showActionCovid(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(10, 5);
	ssd1306_WriteString("I recommend a", Font_7x10, White);
	ssd1306_SetCursor(5, 20);
	ssd1306_WriteString("quarantine until", Font_7x10, White);
	ssd1306_SetCursor(8, 35);
	ssd1306_WriteString("you have a more", Font_7x10, White);
	ssd1306_SetCursor(5, 50);
	ssd1306_WriteString("accurate outcome", Font_7x10, White);
	ssd1306_UpdateScreen();

}

void showActionFever(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 10);
	ssd1306_WriteString("I recommend that", Font_7x10, White);
	ssd1306_SetCursor(17, 25);
	ssd1306_WriteString("you stay warm", Font_7x10, White);
	ssd1306_SetCursor(38, 40);
	ssd1306_WriteString("and rest", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showActionHighFever(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 10);
	ssd1306_WriteString("I recommend that", Font_7x10, White);
	ssd1306_SetCursor(17, 25);
	ssd1306_WriteString("you stay warm", Font_7x10, White);
	ssd1306_SetCursor(5, 40);
	ssd1306_WriteString("and contact your", Font_7x10, White);
	ssd1306_SetCursor(44, 54);
	ssd1306_WriteString("doctor", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void showActionHighestFever(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5, 10);
	ssd1306_WriteString("I recommend that", Font_7x10, White);
	ssd1306_SetCursor(17, 25);
	ssd1306_WriteString("you stay warm", Font_7x10, White);
	ssd1306_SetCursor(5, 40);
	ssd1306_WriteString("but should go to", Font_7x10, White);
	ssd1306_SetCursor(18, 54);
	ssd1306_WriteString("the hospital", Font_7x10, White);
	ssd1306_UpdateScreen();
}



/*
 * Sub-functions that use in Action Tachycardia
 */

void showInhalation(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(30, 25);
	ssd1306_WriteString("Inhale", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void showExhalation(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(30, 25);
	ssd1306_WriteString("Exhale", Font_11x18, White);
	ssd1306_UpdateScreen();
}




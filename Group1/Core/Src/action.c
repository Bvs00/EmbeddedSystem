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


void showMeasures(){
	Measure* measures = getMeasure();
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

void showDangerIpossimeia(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Ipossimeia", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerTachicardia(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Tachycardia", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerAritmia(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Aritmia", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerFebbre(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Febbre", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerCovid(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("Covid", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerFebbreAlta(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("FebbreAlta", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showDangerFebbreMoltoAlta(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Warning", Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString("FebbreMoltoAlta", Font_11x18, White);
    ssd1306_UpdateScreen();
}

void showReplaceFinger(void){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 10);
	ssd1306_WriteString("Please replace", Font_7x10, White);
	ssd1306_SetCursor(50, 25);
	ssd1306_WriteString("Finger", Font_7x10, White);
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



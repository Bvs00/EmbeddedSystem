#include "main.h"
#include "ssd1306.h"
#include "ds1307rtc.h"
#include "usart.h"
#include "tim.h"


/*
 * In questo file andremo a fare tutte le azioni
 */

enum Measure{
	TEMPERATURE_MODE = 0,
	HEART_RATE_MODE ,
	OXYGEN_MODE
};

void computeAverageTemp(void);

void computeAverageHeartRate(void);

void computeAverageOxygen(void);

void showMeasures(void);

void showDangerIpossimeia(void);

void showDangerTachicardia(void);

void showDangerAritmia(void);

void showDangerFebbre(void);

void showDangerCovid(void);

void showDangerFebbreAlta(void);

void showDangerFebbreMoltoAlta(void);

void showReplaceFinger(void);

void showInhalation(void);

void showExhalation(void);

void compositeString(char*, uint8_t);

void showTerminalTime(DateTime datetime);

// All action on Disease
void showActionTachycardia(void);

void showActionHypoxemia(void);

void showActionArrhythmia(void);

void showActionCovid(void);

void showActionFever(void);

void showActionHighFever(void);

void showActionHighestFever(void);

// show how to do
void showWhichAction(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);

void showWhichActionTachycardia(void);








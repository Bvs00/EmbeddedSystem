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

/*
 * Compute Average Value
 */
void computeAverageTemp(void);

void computeAverageHeartRate(void);

void computeAverageOxygen(void);

/*
 * Show Single Screen
 */
void showMeasures(void);

void showReplaceFinger(void);

void compositeString(char*, uint8_t);

void showTerminalTime(DateTime datetime);

/*
 * Show Danger Screen
 */
void showDangerHypoxemia(void);

void showDangerTachycardia(void);

void showDangerArrhythmia(void);

void showDangerFever(void);

void showDangerCovid(void);

void showDangerHighFever(void);

void showDangerHighestFever(void);

/*
 * Decide which action you have to do
 */
void showWhichAction(void);

void showWhichActionTachycardia(void);

void showWhichActionArrhythmia(void);

void showWhichActionCovid(void);

void showWhichActionFever(void);

void showWhichActionHighFever(void);

void showWhichActionHighestFever(void);

void showWhichActionHypoxemia(void);

/*
 * All Action that you have to do
 */
void showActionTachycardia(void);

void showActionHypoxemia(void);

void showActionArrhythmia(void);

void showActionCovid(void);

void showActionFever(void);

void showActionHighFever(void);

void showActionHighestFever(void);

/*
 * Sub-functions that use in Action Tachycardia
 */
void showInhalation(void);

void showExhalation(void);









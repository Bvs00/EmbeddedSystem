#include "main.h"
#include "ssd1306.h"


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

void compositeString(char*, uint8_t);

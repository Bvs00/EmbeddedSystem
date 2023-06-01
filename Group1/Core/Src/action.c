#include "action.h"

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

#include "KY028.h"
#include "usart.h"

float getTemperature(void){

	float R,temp;
	char value[15];
	uint32_t rawValue;

	rawValue = HAL_ADC_GetValue(&hadc1);
	R = (4095.0/rawValue)-1;

	R = 10000/R;

	temp = R/10000;
	temp = log(temp);
	temp /= 3950;
	temp += 1.0/298.15;
	temp = 1.0/temp;
	temp -= 273.15;

	return temp;
}

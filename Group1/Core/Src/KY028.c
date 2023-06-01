#include "KY028.h"

float getTemperature(void){
	uint32_t rawValue;
	float R,temp;

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


#ifndef INC_DS1307RTC_H_
#define INC_DS1307RTC_H_

#include <stdint.h>


#define DS1307_OK 		(0)
#define DS1307_ERR		(-1)
#define DS1307_IC2_ERR	(-2)


typedef struct{
	uint8_t     seconds;
	uint8_t     minutes;
	uint8_t     hours;
	uint8_t     day;		// 1 -> Sunday, 2 -> Monday ...
	uint8_t     date;
	uint8_t     month;
	uint16_t    year;
}DateTime;


int8_t ds1307rtc_init();

int8_t ds1307rtc_get_date_time(DateTime* datetime);

int8_t ds1307rtc_set_date_time(const DateTime* datetime);

void ds1307rtc_start();



#endif /* INC_DS1307RTC_H_ */

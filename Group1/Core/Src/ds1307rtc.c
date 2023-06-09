#include <stdint.h>
#include <string.h>
#include "i2c.h"
#include "ds1307rtc.h"

#define ADDRESS_SIZE				(1)
#define DATA_TRANSFER_SIZE			(7) //1 Seconds, 2 Minutes, 3 Hours, 4 Day, 5 Date, 6 Month, 7 Year

#define MAX_RETRY					(3)

//DEVICE ADDRESS (From the user guide)
#define DS1307_ADDRESS 				(0xD0) //It is 1101000<<1

//LIST OF INTERNAL ADDRESSES (From the user guide)
#define DS1307_SECONDS				(0x00)
#define DS1307_MINUTES				(0x01)
#define DS1307_HOURS				(0x02)
#define DS1307_DAY					(0x03)
#define DS1307_DATE					(0x04)
#define DS1307_MONTH				(0x05)
#define DS1307_YEAR					(0x06)
#define DS1307_CONTROL				(0x07)

/* Bits in control register */
#define DS1307_CONTROL_OUT			(7)
#define DS1307_CONTROL_SQWE			(4)
#define DS1307_CONTROL_RS1			(1)
#define DS1307_CONTROL_RS0			(0)


/*
* @fn          uint8_t bcd2Dec ( uint8_t val )
* @brief       Convert BCD to Decimal
* @param[in]   val value to convert from BCD to Decimal
* @return      res Converted value
*/
uint8_t bcd2Dec ( uint8_t val )
{
    uint8_t res = ((val/16*10) + (val % 16));
    return res;
}

/*
* @fn           uint8_t dec2Bcd ( uint8_t val )
* @brief        Convert Decimal to BCD
* @param[in]    val value to convert from Decimal to BCD
* @return       res Converted value
*/
uint8_t dec2Bcd ( uint8_t val )
{
    uint8_t res = ((val/10*16) + (val%10));
    return res;
}




int8_t ds1307rtc_get_date_time(DateTime* datetime)
{
	HAL_StatusTypeDef returnValue;
	uint8_t in_buff[DATA_TRANSFER_SIZE];



	//USING Mem_Read function
	//Mem_Read is equivalent for performing Transmit of the MemAddress and Receive



														 //DS1307_SECONDS is the first register to be read
	returnValue = HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, in_buff, DATA_TRANSFER_SIZE, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}



	//USING Master Receive and Transmit functions
																  //DS1307_SECONDS is the first register to be read
	returnValue = HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	returnValue = HAL_I2C_Master_Receive(&hi2c1, DS1307_ADDRESS, in_buff, DATA_TRANSFER_SIZE, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	datetime->seconds = bcd2Dec(in_buff[0]);
	datetime->minutes = bcd2Dec(in_buff[1]);
	datetime->hours = bcd2Dec(in_buff[2]);
	datetime->day = bcd2Dec(in_buff[3]);
	datetime->date = bcd2Dec(in_buff[4]);
	datetime->month = bcd2Dec(in_buff[5]);
	datetime->year = bcd2Dec(in_buff[6]);

	return DS1307_OK;
}






int8_t ds1307rtc_set_date_time(const DateTime* datetime)
{
	HAL_StatusTypeDef returnValue;
	uint8_t out_buff[DATA_TRANSFER_SIZE+ADDRESS_SIZE];

	//Putting the address
	out_buff[0] = DS1307_SECONDS;
	//Filling the data
	out_buff[1] = dec2Bcd(datetime->seconds);
	out_buff[2] = dec2Bcd(datetime->minutes);
	out_buff[3] = dec2Bcd(datetime->hours);
	out_buff[4] = dec2Bcd(datetime->day);
	out_buff[5] = dec2Bcd(datetime->date);
	out_buff[6] = dec2Bcd(datetime->month);
	out_buff[7] = dec2Bcd(datetime->year);


	returnValue = HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDRESS,  DS1307_SECONDS, ADDRESS_SIZE, out_buff+1, DATA_TRANSFER_SIZE, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	// USING Master_Transmit function
	/*returnValue = HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, out_buff, ADDRESS_SIZE+DATA_TRANSFER_SIZE, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}*/

	return DS1307_OK;
}




int8_t ds1307rtc_init()
{
	HAL_StatusTypeDef returnValue;
	returnValue = HAL_I2C_IsDeviceReady(&hi2c1, DS1307_ADDRESS, MAX_RETRY, HAL_MAX_DELAY);
	if(returnValue != HAL_OK)
	{
		return DS1307_ERR;
	}
	return DS1307_OK;
}


void ds1307rtc_start(){
	DateTime datetime;
//	datetime.seconds=00;
//	datetime.minutes=48;
//	datetime.hours=12;
//	datetime.day=9;
//	datetime.year=23;
//	datetime.month=6;
//	datetime.date=5;

	ds1307rtc_get_date_time(&datetime);

	ds1307rtc_set_date_time(&datetime);
}




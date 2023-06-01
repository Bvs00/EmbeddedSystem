
#include "MAX32664.h"

/*
 * This function allows to change the mode of the MFIO pin of the MAX32664 from an output pin to an input pin
 */
void mfio(MAX32664* max32664){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin =max32664->mfio;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(max32664->mfio_gpio, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*
 * The begin function allows to initialize the fields of the MAX32664 structure to non acceptable values.
 * Then, the device is started and initialized to application mode.
 * The function returns OK when all the operations are executed correctly,
 * instead it returns ERROR when the operations fail.
 */
uint8_t begin(MAX32664* sensor,I2C_HandleTypeDef *i2c_port, GPIO_TypeDef* reset_pin_gpio,GPIO_TypeDef* mfio_pin_gpio, uint16_t reset_pin, uint16_t mfio_pin){

	sensor->reset = reset_pin;
	sensor->mfio = mfio_pin;
	sensor->reset_gpio = reset_pin_gpio;
	sensor->mfio_gpio = mfio_pin_gpio;
	sensor->i2c_port = i2c_port;
	sensor->infrared_led = -1;
	sensor->red_led = -1;
	sensor->heart_rate = -1;
	sensor->confidence = -1;
	sensor->oxygen = -1;
	sensor->status = -1;
	sensor->algorithm_state = -1;
	sensor->algorithm_status = -1;
	sensor->ib_interval = -1;
	sensor->mode = -1;

	HAL_GPIO_WritePin(sensor->mfio_gpio, sensor->mfio, GPIO_PIN_SET);
	HAL_GPIO_WritePin(sensor->reset_gpio, sensor->reset, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(sensor->reset_gpio, sensor->reset, GPIO_PIN_SET);
	HAL_Delay(1000);
	mfio(sensor);

	sensor->mode = get_device_mode(sensor);

	if((sensor->status == SUCCESS) & (sensor->mode == APPLICATION_MODE))
		return OK;
	else
		return ERROR;
}

/*
 * The read_status functions allows to read the state of the MAX32664.
 * If the operations performed before the call to this functions were executed correctly, the status of the Pulse Oximeter
 * should be SUCCESS.
 * If the operations performed before the call to this functions have failed, then the status field will reflect the error
 * occurred.
 */

uint8_t read_status(MAX32664* max32664){
	uint8_t FAMILY_BYTE = 0x00;
	uint8_t INDEX_BYTE = 0x00;


	read(max32664, FAMILY_BYTE, INDEX_BYTE);
	if(max32664->status != SUCCESS)
		return ERROR;

	return OK;
}

/*
 * The write function allows to write in the specific memory location, the necessary command.
 * After the operation is executed, it is necessary to read the status of the MAX32664 to check if the command
 * was executed correctly.
 */
uint8_t write(MAX32664 *max32664, uint8_t family_byte, uint8_t index_byte, uint8_t write_byte){
	uint8_t array[] = {-1,-1,-1};
	array[0]=family_byte;
	array[1]= index_byte;
	array[2] = write_byte;

	if(HAL_I2C_Master_Transmit(max32664->i2c_port, MAX32664_I2C_WriteAddress, array, 3, HAL_MAX_DELAY) != HAL_OK)
		return ERROR;

	HAL_Delay(CMD_DELAY);

	return read_status(max32664);

}

/*
 * The write function allows to write in the specific memory location, the necessary command.
 * This function is needed to enable the MAX30101 sensor and to enable the algorithm that compute the necessary values
 * to read the blood oxygenation nad heart beat of the patient.
 * After the operation is executed, it is necessary to read the status of the MAX32664 to check if the command
 * was executed correctly.
 */
uint8_t enable_write(MAX32664 *max32664, uint8_t family_byte, uint8_t index_byte, uint8_t write_byte){
	uint8_t array[] = {-1,-1,-1};
	array[0]=family_byte;
	array[1]= index_byte;
	array[2] = write_byte;

	if(HAL_I2C_Master_Transmit(max32664->i2c_port, MAX32664_I2C_WriteAddress, array, 3, HAL_MAX_DELAY) != HAL_OK)
		return ERROR;

	HAL_Delay(CMD_DELAY_MAX30101);

	return read_status(max32664);

}

/*
 * The read function allows to read the specific memory location.
 * After the operation is executed, it is necessary to read the status of the MAX32664 to check if the command
 * was executed correctly.
 */
uint8_t read(MAX32664 *max32664, uint8_t family_byte, uint8_t index_byte){
	uint8_t address[] = {-1, -1};
	uint8_t read[] ={-1, -1};

	address[0]= family_byte;
	address[1]= index_byte;

	if(HAL_I2C_Master_Transmit(max32664->i2c_port, MAX32664_I2C_WriteAddress, address, 2, HAL_MAX_DELAY) != HAL_OK)
		return ERROR;

	HAL_Delay(CMD_DELAY);

	if(HAL_I2C_Master_Receive(max32664->i2c_port, MAX32664_I2C_ReadAddress, read, 2, HAL_MAX_DELAY) != HAL_OK)
		return ERROR;

	max32664->status = read[0];

	if(max32664->status == SUCCESS)
		return read[1];
	else
		return ERROR;
}

/*
 * This function allows to set the device mode of the MAX32664. If the mode parameter is not an acceptable modality
 * then the function returns ERROR.
 */
uint8_t set_device_mode(MAX32664 *max32664, uint8_t mode){
	uint8_t FAMILY_BYTE = 0x01;
	uint8_t INDEX_BYTE = 0x00;

	switch (mode) {
		case APPLICATION_MODE:
			return write(max32664, FAMILY_BYTE,INDEX_BYTE, APPLICATION_MODE);
		case BOOTLOADER_MODE:
			return write(max32664, FAMILY_BYTE,INDEX_BYTE, BOOTLOADER_MODE);
		case RESET_MODE:
			return write(max32664, FAMILY_BYTE,INDEX_BYTE, RESET_MODE);
		case SHUTDOWN_REQUEST_MODE:
			return write(max32664, FAMILY_BYTE,INDEX_BYTE, SHUTDOWN_REQUEST_MODE);
	}

	return ERROR;
}

/*
 * This function allows to read the device mode of the MAX32664.
 */
uint8_t	get_device_mode(MAX32664 *max32664){

	uint8_t FAMILY_BYTE = 0x02;
	uint8_t INDEX_BYTE = 0x00;

	max32664->mode = read(max32664, FAMILY_BYTE, INDEX_BYTE);
	return OK;


}

/*
 * This function allows to set the output mode of the MAX32664. If the mode parameter is not an acceptable modality
 * then the function returns ERROR, otherwise it writes the command to the specified location.
 */
uint8_t set_output_mode(MAX32664 *max32664, uint8_t mode){
	uint8_t FAMILY_BYTE = 0x10;
	uint8_t INDEX_BYTE = 0x00;

	if(mode > SENSOR_ALGORITHM_SAMPLE_COUNTER_BYTE)
		return ERR_INPUT_VALUE;

	return write(max32664, FAMILY_BYTE, INDEX_BYTE, mode);

}

/*
 * This function allows to read the output mode of the MAX32664.
 */
uint8_t get_output_mode(MAX32664 *max32664){
	uint8_t FAMILY_BYTE = 0x11;
	uint8_t INDEX_BYTE = 0x00;

	return read(max32664, FAMILY_BYTE, INDEX_BYTE);
}

/*
 * This function allows to enable and disable the MAX30101 pulse oximeter and heart rate monitor sensor.
 */
uint8_t MAX30101_enable(MAX32664 *max32664, uint8_t enable){
	uint8_t FAMILY_BYTE = 0x44;
	uint8_t INDEX_BYTE = 0x03;

	if(enable == ENABLE)
		return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, ENABLE);
	else
		return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, DISABLE);

	return ERR_INPUT_VALUE;


}

/*
 * This function allows to read the number of samples from the MAX30101 sensor.
 */
uint8_t get_number_of_samples_FIFO(MAX32664 *max32664){
	uint8_t FAMILY_BYTE = 0x12;
	uint8_t INDEX_BYTE = 0x00;

	return read(max32664, FAMILY_BYTE, INDEX_BYTE);
}

/*
 * This function allows to enable and disable the Automatic Gain Control algorithm.
 */
uint8_t AGC_enable(MAX32664 *max32664, uint8_t enable){
	uint8_t FAMILY_BYTE = 0x52;
	uint8_t INDEX_BYTE = 0x00;

	if(enable == ENABLE)
		return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, ENABLE);
	else
		return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, DISABLE);

	return ERR_INPUT_VALUE;

}
/*
 * This function allows to enable and disable the MaximFast algorithm.
 */
uint8_t MaximFast_enable(MAX32664 *max32664, uint8_t enable){

	uint8_t FAMILY_BYTE = 0x52;
	uint8_t INDEX_BYTE = 0x02;

		if(enable == ENABLE)
			return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, ENABLE);
		else
			return enable_write(max32664, FAMILY_BYTE, INDEX_BYTE, DISABLE);

	return ERR_INPUT_VALUE;

}

/*
 * This function allows to specify the number of remaining free slots in the MAX30101 FIFO that are used as a
 * threshold to launch an interrupt from the MFIO pin.
 */
uint8_t set_fifo_threshold(MAX32664 *max32664, uint8_t threshold){
	uint8_t FAMILY_BYTE = 0x10;
	uint8_t INDEX_BYTE = 0x01;

	return write(max32664, FAMILY_BYTE, INDEX_BYTE, threshold);

}

/*
 * This function is necessary to configure the MAX32664 sensor to read the heart beat and blood oxygenation of the patient.
 * The SENSOR_ALGORITHM_DATA output mode allows to read from the MAX30101 FIFO both the raw values and the values computed
 * from the algorithms enabled on the MAX32664.
 */

uint8_t config_sensor(MAX32664 *max32664, uint8_t mode){

	if(set_output_mode(max32664, SENSOR_ALGORITHM_DATA) !=  OK)
		return max32664->status;

    if(set_fifo_threshold(max32664, 0x01) !=  OK)
    	return max32664->status;

    if(AGC_enable(max32664, ENABLE) !=  OK)
    	 return max32664->status;

    if(MAX30101_enable(max32664, ENABLE) !=  OK)
		return max32664->status;

	if(MaximFast_enable(max32664, mode) !=  OK)
		return max32664->status;

	HAL_Delay(1000);

	return OK;
}

/*
 * This function is used to read the samples from the MAX30101 sensor.
 */
uint8_t read_sensor(MAX32664 *max32664){
	uint8_t FAMILY_BYTE = 0x12;
	uint8_t INDEX_BYTE = 0x01;
	uint8_t samples[32];

	uint8_t address[] = {-1, -1};

	address[0]= FAMILY_BYTE;
	address[1]= INDEX_BYTE;

	if(HAL_I2C_Master_Transmit(max32664->i2c_port, MAX32664_I2C_WriteAddress, address, 2, HAL_MAX_DELAY) != HAL_OK)
		return ERROR;
	if(HAL_I2C_Master_Receive(max32664->i2c_port,MAX32664_I2C_ReadAddress, samples, 32, HAL_MAX_DELAY) != OK)
		return ERROR;


	/* Infrared raw value */
	max32664->infrared_led = (uint32_t)(samples[0] <<16);
	max32664->infrared_led |= (uint32_t)(samples[1] <<8);
	max32664->infrared_led |= samples[2];

	/* Red Led raw value */
	max32664->red_led = (uint32_t)(samples[3] <<16);
	max32664->red_led |= (uint32_t)(samples[4] <<8);
	max32664->red_led |= samples[5];


	/* Values from samples [6:11] are values for leds that are not available on the MAX30101, so they're left empty.
	 * Value from sample [12] is a value for the empty accelerometer  */

	/* heart rate values */
	max32664->heart_rate = (uint16_t)(samples[13] << 8);
	max32664->heart_rate |= (samples[14]);
	max32664->heart_rate /= 10;

	/* confidence */
	max32664->confidence =(uint8_t) samples[15];

	/* oxygen */
	max32664->oxygen = (uint16_t)(samples[16] <<8);
	max32664->oxygen |= (samples[17]);
	max32664->oxygen /= 10;

	/* algorithm state */
	max32664->algorithm_state = (uint8_t) samples[18];

	/* extension of the algorithm state */
	max32664->algorithm_status = (uint8_t) samples[19];

	/* interbeat interval */
	max32664->ib_interval = (uint16_t)(samples[20] << 8);
	max32664->ib_interval |= (samples[21]);
	max32664->ib_interval /= 1000;

	return OK;

}


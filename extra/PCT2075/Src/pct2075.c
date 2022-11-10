#include "../../User_Drivers/Inc/pct2075.h"

HAL_StatusTypeDef PCT2075GVX_Init(I2C_HandleTypeDef *_hi2c, uint8_t _sensorId)
{
	uint8_t _I2C_Mailbox[2];
	uint8_t _array[4][4] =
	{
		{REG_ADDR_CONF,   SIZE_OF_DATA_CONF,	(uint8_t)DATA_CONF,		0},
		{REG_ADDR_HYST,   SIZE_OF_DATA_HYST,	(uint8_t)DATA_HYST_L,	(uint8_t)DATA_HYST_H},
		{REG_ADDR_OS,   	SIZE_OF_DATA_OS,	(uint8_t)DATA_OS_L,		(uint8_t)DATA_OS_H},
		{REG_ADDR_IDLE,   SIZE_OF_DATA_IDLE,	(uint8_t)DATA_IDLE,		0}
	};
	
	for( int _count = 0; _count < 4; _count++ )
	{
		_I2C_Mailbox[0] = _array[_count][2];
		_I2C_Mailbox[1] = _array[_count][3];
		
		if( HAL_OK != HAL_I2C_Mem_Write(_hi2c, _sensorId, _array[_count][0], 1, (uint8_t*)_I2C_Mailbox, _array[_count][1], 100) )
		{
			return HAL_ERROR;
		}
	}	
	return HAL_OK;
}

int8_t PCT2075GVX_Read(I2C_HandleTypeDef *_hi2c, uint8_t _sensorId)
{
	uint8_t _I2C_Mailbox[2] = {0,0};
	float _temp = 0;
    if(HAL_I2C_Mem_Read(_hi2c, _sensorId, 0x0, 1, (uint8_t*)_I2C_Mailbox, 2, 100)!= HAL_OK)
    {
    	Error_Handler();
    }

	_temp = ( ( ( _I2C_Mailbox[0] << 8 ) + _I2C_Mailbox[1] ) >> 5 ) * MINIMUM_TEMPERATURE_STEP_VALUE;
	return (int8_t)_temp;
}


/* Includes ------------------------------------------------------------------*/
#include "../Inc/cs43l22.h"
/*------------------------------------------------------------------*/
extern uint8_t i2CData[3];
extern ErrorStatus I2C_Write_Register(I2C_HandleTypeDef i2c_handle, uint8_t id, uint8_t reg, uint8_t *data);
extern ErrorStatus I2C_Read_Register(I2C_HandleTypeDef i2c_handle, uint8_t id, uint8_t reg, uint8_t *data);
extern void Error_Handler(void);
extern double USER_Map(long val, long in_min, long in_max, long out_min, long out_max);

//-------------- Public Functions ----------------//
CS43L22_STATE CS43_state;
// Function(1): Initialisation
void CS43_Init(I2C_HandleTypeDef i2c_handle, uint8_t id, CS43_MODE outputMode)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); // This is extremely important to work !!
	//(1): Get the I2C handle
	if(HAL_I2C_IsDeviceReady(&i2c_handle, id, 1, 100)!=HAL_OK)
	{
		Error_Handler();
	}
	//(2): Power down
	i2CData[1] = 0x01;
	I2C_Write_Register(i2c_handle, id, POWER_CONTROL1, &i2CData[1]);
	//(3): Enable Right and Left headphones
	i2CData[1] =  (2 << 6);  // PDN_HPB[0:1]  = 10 (HP-B always onCon)
	i2CData[1] |= (2 << 4);  // PDN_HPA[0:1]  = 10 (HP-A always on)
	i2CData[1] |= (3 << 2);  // PDN_SPKB[0:1] = 11 (Speaker B always off)
	i2CData[1] |= (3 << 0);  // PDN_SPKA[0:1] = 11 (Speaker A always off)
	I2C_Write_Register(i2c_handle, id,  POWER_CONTROL2,&i2CData[1]);
	//(4): Automatic clock detection
	i2CData[1] = (1 << 7);
	I2C_Write_Register(i2c_handle, id,  CLOCKING_CONTROL,&i2CData[1]);
	//(5): Interface control 1
	I2C_Read_Register(i2c_handle, id,  INTERFACE_CONTROL1, &i2CData[1]);
	i2CData[1] &= (1 << 5); // Clear all bits except bit 5 which is reserved
	i2CData[1] &= ~(1 << 7);  // Slave
	i2CData[1] &= ~(1 << 6);  // Clock polarity: Not inverted
	i2CData[1] &= ~(1 << 4);  // No DSP mode
	i2CData[1] &= ~(1 << 2);  // Left justified, up to 24 bit (default)
	i2CData[1] |= (1 << 2);
	
	i2CData[1] |=  (3 << 0);  // 16-bit audio word length for I2S interface
	I2C_Write_Register(i2c_handle, id,  INTERFACE_CONTROL1,&i2CData[1]);
	//(6): Passthrough A settings
	I2C_Read_Register(i2c_handle, id,  PASSTHROUGH_A, &i2CData[1]);
	i2CData[1] &= 0xF0;      // Bits [4-7] are reserved
	i2CData[1] |=  (1 << 0); // Use AIN1A as source for passthrough // Analog Girişler
	I2C_Write_Register(i2c_handle, id,  PASSTHROUGH_A,&i2CData[1]);
	//(7): Passthrough B settings
	I2C_Read_Register(i2c_handle, id,  PASSTHROUGH_B, &i2CData[1]);
	i2CData[1] &= 0xF0;      // Bits [4-7] are reserved
	i2CData[1] |=  (1 << 0); // Use AIN1B as source for passthrough // Analog Girişler
	I2C_Write_Register(i2c_handle, id,  PASSTHROUGH_B,&i2CData[1]);
	//(8): Miscellaneous register settings
	I2C_Read_Register(i2c_handle, id,  MISCELLANEOUS_CONTRLS, &i2CData[1]);
	if(outputMode == MODE_ANALOG)
	{
		i2CData[1] |=  (1 << 7);   // Enable passthrough for AIN-B
		i2CData[1] |=  (1 << 6);   // Enable passthrough for AIN-A
		i2CData[1] &= ~(1 << 5);   // Unmute passthrough on AIN-B
		i2CData[1] &= ~(1 << 4);   // Unmute passthrough on AIN-A
		i2CData[1] &= ~(1 << 3);   // Changed settings take affect immediately
	}
	else if(outputMode == I2S_MODE)
	{
		i2CData[1] = 0x02;
	}
	I2C_Write_Register(i2c_handle, id,  MISCELLANEOUS_CONTRLS,&i2CData[1]);
	//(9): Unmute headphone and speaker
	I2C_Read_Register(i2c_handle, id,  PLAYBACK_CONTROL, &i2CData[1]);
	i2CData[1] = 0x00;
	I2C_Write_Register(i2c_handle, id,  PLAYBACK_CONTROL,&i2CData[1]);
	//(10): Set volume to default (0dB)
	i2CData[1] = 0x00;
	I2C_Write_Register(i2c_handle, id,  PASSTHROUGH_VOLUME_A,&i2CData[1]);
	I2C_Write_Register(i2c_handle, id,  PASSTHROUGH_VOLUME_B,&i2CData[1]);
	I2C_Write_Register(i2c_handle, id,  PCM_VOLUME_A,&i2CData[1]);
	I2C_Write_Register(i2c_handle, id,  PCM_VOLUME_B,&i2CData[1]);
	//(10): BEEP Configurations
	I2C_Read_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
	i2CData[1] = 0x00;
	//i2CData[1] |=  (3 << 6); // Enable Beep as Continious Mode.
	i2CData[1] &= ~(1 << 5); // Mix Enabled; The beep signal mixes with the digital signal from the serial data input.
	I2C_Write_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
	//(11): BEEP Frequency & ON Time Settings
	I2C_Read_Register(i2c_handle, id,  BEEP_FREQ_ONTIME, &i2CData[1]);
	i2CData[1] = 0x00;// Beep frequency is set as default 260.87 Hz, ~86 ms beep ON time.
	I2C_Write_Register(i2c_handle, id,  BEEP_FREQ_ONTIME, &i2CData[1]);
	//(12): BEEP Volume Settings
	I2C_Read_Register(i2c_handle, id,  BEEP_VOL_OFFTIME, &i2CData[1]);
	i2CData[1] = 20; // Max. Vol is limited, ~1.23 s beep OFF time.
	I2C_Write_Register(i2c_handle, id,  BEEP_VOL_OFFTIME, &i2CData[1]);
}
void CS43_BeepEnable(I2C_HandleTypeDef i2c_handle, uint8_t id)
{
	I2C_Read_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
	i2CData[1] |=  (2 << 6); // Enable Beep as Continuous Mode.
	I2C_Write_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
}
void CS43_BeepDisable(I2C_HandleTypeDef i2c_handle, uint8_t id)
{
	I2C_Read_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
	i2CData[1] &= 0x3F; // Disable Beep
	I2C_Write_Register(i2c_handle, id,  BEEP_TONE_CONFIG, &i2CData[1]);
}
// Function(2): Enable Right and Left headphones
void CS43_Enable_RightLeft(I2C_HandleTypeDef i2c_handle, uint8_t id, uint8_t side)
{
	switch (side)
	{
		case 0:
			i2CData[1] =  (3 << 6);  // PDN_HPB[0:1]  = 11 (HP-B always OFF)
			i2CData[1] |= (3 << 4);  // PDN_HPA[0:1]  = 11 (HP-A always OFF)
			break;
		case 1:
			i2CData[1] =  (2 << 6);  // PDN_HPB[0:1]  = 10 (HP-B always ON)
			i2CData[1] |= (3 << 4);  // PDN_HPA[0:1]  = 11 (HP-A always OFF)
			break;
		case 2:
			i2CData[1] =  (3 << 6);  // PDN_HPB[0:1]  = 11 (HP-B always OFF)
			i2CData[1] |= (2 << 4);  // PDN_HPA[0:1]  = 10 (HP-A always ON)
			break;
		case 3:
			i2CData[1] =  (2 << 6);  // PDN_HPB[0:1]  = 10 (HP-B always ON)
			i2CData[1] |= (2 << 4);  // PDN_HPA[0:1]  = 10 (HP-A always ON)
			break;
		default:
			break;
	}
	i2CData[1] |= (3 << 2);  // PDN_SPKB[0:1] = 11 (Speaker B always off)
	i2CData[1] |= (3 << 0);  // PDN_SPKA[0:1] = 11 (Speaker A always off)
	I2C_Write_Register(i2c_handle, id,  POWER_CONTROL2,&i2CData[1]);
}

// Function(3): Set Volume Level
uint8_t CS43_SetVolume(uint8_t volume)
{
	uint8_t TempVol;
	if(volume>100)
	{
		TempVol=24;
	}
	else if((50<=volume)&&(volume<=100))
	{
		TempVol=USER_Map(volume,50,100,0,24);
	}
	else if((0<=volume)&&(volume<50))
	{
		TempVol=USER_Map(volume,0,49,(255-127),255);
	}
	return TempVol;
}
void CS43_SendVolume(I2C_HandleTypeDef i2c_handle, uint8_t id, uint8_t volume_A, uint8_t volume_B)
{
	i2CData[1]=CS43_SetVolume(volume_A);
	i2CData[2]=CS43_SetVolume(volume_B);
	I2C_Write_Register(i2c_handle, id,  CS43L22_REG_MASTER_A_VOL,&i2CData[1]);
	I2C_Write_Register(i2c_handle, id,  CS43L22_REG_MASTER_B_VOL,&i2CData[2]);
}

// Function(4): Start the Audio DAC
void CS43_Start(I2C_HandleTypeDef i2c_handle, uint8_t id)
{
	CS43_state=CS43_ON_AIR;
	// Write 0x99 to register 0x00.
	i2CData[1] = 0x99;
	I2C_Write_Register(i2c_handle, id, CONFIG_00,&i2CData[1]);
	// Write 0x80 to register 0x47.
	i2CData[1] = 0x80;
	I2C_Write_Register(i2c_handle, id, CONFIG_47,&i2CData[1]);
	// Write '1'b to bit 7 in register 0x32.
	I2C_Read_Register(i2c_handle, id,  CONFIG_32, &i2CData[1]);
	i2CData[1] |= 0x80;
	I2C_Write_Register(i2c_handle, id, CONFIG_32,&i2CData[1]);
	// Write '0'b to bit 7 in register 0x32.
	I2C_Read_Register(i2c_handle, id, CONFIG_32, &i2CData[1]);
	i2CData[1] &= ~(0x80);
	I2C_Write_Register(i2c_handle, id, CONFIG_32,&i2CData[1]);
	// Write 0x00 to register 0x00.
	i2CData[1] = 0x00;
	I2C_Write_Register(i2c_handle, id, CONFIG_00,&i2CData[1]);
	//Set the "Power Ctl 1" register (0x02) to 0x9E
	i2CData[1] = 0x9E;
	I2C_Write_Register(i2c_handle, id, POWER_CONTROL1,&i2CData[1]);
}

void CS43_Stop(I2C_HandleTypeDef i2c_handle, uint8_t id)
{
	CS43_state=CS43_PAUSED;
	i2CData[1] = 0x01;
	I2C_Write_Register(i2c_handle, id,  POWER_CONTROL1,&i2CData[1]);
}

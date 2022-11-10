
#include "stm32f4xx_hal.h"

#define POWER_CONTROL1					0x02
#define POWER_CONTROL2					0x04
#define CLOCKING_CONTROL 	  			0x05
#define INTERFACE_CONTROL1				0x06
#define INTERFACE_CONTROL2				0x07
#define PASSTHROUGH_A					0x08
#define PASSTHROUGH_B					0x09
#define MISCELLANEOUS_CONTRLS			0x0E
#define PLAYBACK_CONTROL				0x0F
#define PASSTHROUGH_VOLUME_A			0x14
#define PASSTHROUGH_VOLUME_B			0x15
#define PCM_VOLUME_A					0x1A
#define PCM_VOLUME_B					0x1B
#define BEEP_FREQ_ONTIME				0x1C
#define BEEP_VOL_OFFTIME				0x1D
#define BEEP_TONE_CONFIG				0x1E
#define CONFIG_00						0x00
#define CONFIG_47						0x47
#define CONFIG_32						0x32
#define CS43L22_REG_MASTER_A_VOL  		0x20
#define CS43L22_REG_MASTER_B_VOL  		0x21
#define HEADPHONE_DAC_VOL_CTRL_A 		0x22
#define HEADPHONE_DAC_VOL_CTRL_B  		0x23
#define DEVICE_CS43 					0x94
#define CS43_MUTE				 		0x00
#define CS43_RIGHT						0x01
#define CS43_LEFT				 		0x02
#define CS43_RIGHT_LEFT	 				0x03
/*
#define VOLUME_CONVERT_ANL(Volume)    (((Volume) > 100)? 255:((uint8_t)(((Volume) * 255) / 100)))
#define VOLUME_CONVERT_DIG(Volume)    (((Volume) > 100)? 24:((uint8_t)((((Volume) * 48) / 100) -24)))
*/
//1. Mode Select Enum
typedef enum
{
	I2S_MODE = 0,
	DAC_MODE=1,
}CS43_MODE;
typedef enum
{
	CS43_PAUSED=0,
	CS43_ON_AIR=!CS43_PAUSED
}CS43L22_STATE;

//(3): List of the functions prototypes
void CS43_Init(I2C_HandleTypeDef , uint8_t id, CS43_MODE outputMode);
void CS43_Enable_RightLeft(I2C_HandleTypeDef , uint8_t id, uint8_t side);
uint8_t CS43_SetVolume(uint8_t volume);
void CS43_SendVolume(I2C_HandleTypeDef, uint8_t id,uint8_t volume_A, uint8_t volume_B);
void CS43_Start(I2C_HandleTypeDef, uint8_t id);
void CS43_Stop(I2C_HandleTypeDef, uint8_t id);
void CS43_BeepEnable(I2C_HandleTypeDef , uint8_t id);
void CS43_BeepDisable(I2C_HandleTypeDef , uint8_t id);

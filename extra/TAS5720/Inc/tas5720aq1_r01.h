/*
 * TAS5720AQ1_R2.h
 *
 *  Created on: 4 Eyl 2020
 *      Author: Monster
 */

#ifndef USER_DRIVERS_INC_TAS5720AQ1_R01_H_
#define USER_DRIVERS_INC_TAS5720AQ1_R01_H_
#include "stm32f4xx.h"
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sogediagnostic.h"


#define SPK_AMP1	(0xDA)
#define SPK_AMP2	(0xD8)
#define SPK_SD		(0x01)
#define SPK_SLEEP	(0x02)
#define LEFT_CH		(0x01)
#define RIGHT_CH	(0x02)
#define VOL_FADE	(0x80)
#define HP_FILTER	(0x80)
#define DB0			(0x10)
#define DB1			(0x20)
#define SM			(0x08)
#define SM0			(0x04)
#define SIF2		(0x04)
#define SIF1		(0x02)
#define SIF0		(0x01)

#define TAS5720AQ1_ERROR_THRESHOLD 	(240)
#define TAS5720AQ1_ERROR_DEC		(5)
#define TAS5720AQ1_OTE_ERROR 		(40)
#define TAS5720AQ1_DCE_ERROR 		(30)
#define TAS5720AQ1_OCE_ERROR 		(30)
#define TAS5720AQ1_CLKE_ERROR 		(10)


/********** PORT FUNC*******/
typedef 	HAL_StatusTypeDef(*funcptr)(I2C_HandleTypeDef *, uint16_t , uint32_t , uint32_t );
typedef 	ErrorStatus		(*fun2ptr)(I2C_HandleTypeDef , uint8_t , uint8_t , uint8_t *);
typedef 	GPIO_PinState 	(*fun2ptr1)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
typedef 	void 			(*fun2ptr2)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);


typedef enum{
  CLEAR_ERR = 0U,
  SET_ERR   = !CLEAR_ERR
} tas5720aq1_flagstatus;

typedef enum{
	RChSelected = 0U,
	LChSelected = !RChSelected
}SerialAudioInputChSelect;

typedef enum{
	SingleSpeed = 0U,
	DoubleSpeed = !SingleSpeed
}SpeedModeSelect;

typedef enum{
	HPFilterIs_Bypassed    = 0U,
	HPFilterIsNot_Bypassed = !HPFilterIs_Bypassed
}HPFilterSelect;


typedef enum{
	tas5720aq1_SUCCESS = 0U,
	tas5720aq1_ERROR   = !tas5720aq1_SUCCESS
} tas5720aq1_ErrorStatus;


typedef enum{
 NERR= 	(0x00),		//(0b00000000),
 OTE=	(0x01),		//(0b00000001),
 DCE=	(0x02),		//(0b00000010),
 OCE=	(0x04),		//(0b00000100),
 CLKE= 	(0x08),		//(0b00001000)
}tas5720aq1_errorcodes;

typedef enum{
	SwRateIs_6LRCK  =	(0x000),
	SwRateIs_8LRCK  =	(0x001),
	SwRateIs_10LRCK =	(0x010),
	SwRateIs_12LRCK =	(0x011),
	SwRateIs_14LRCK =	(0x100),
	SwRateIs_16LRCK =	(0x101),
	SwRateIs_20LRCK =	(0x110),
	SwRateIs_24LRCK =	(0x111)
}PWMRateSelect;

typedef enum{
	AnlGainSettingIs_19dB = (0x00),
	AnlGainSettingIs_22dB = (0x01),
	AnlGainSettingIs_25dB =	(0x10)
}AnalogGainSelect;


typedef enum{
	Boosted_0dB	 = 	(0x00),
	Boosted_6dB	 = 	(0x01),
	Boosted_12dB =	(0x10),
	Boosted_18dB =  (0x11)
}DigitalBoostSelect;


typedef enum{
	VolumeFade_Disabled = 0x00,
	VolumeFade_Enabled  = 0x80
}VolumeFadeSelect;

typedef enum{
	RightJustified_24Bits	=	0x00, //(0b00000000),
	RightJustified_20Bits	=	0x01, //(0b00000001),
	RightJustified_18Bits	=	0x02, //(0b00000010),
	RightJustified_16Bits	=	0x03, //(0b00000011),
	Default_I2Sformat		=	0x04, //(0b00000100),
	LeftJustified_1624Bits	=	0x05, //(0b00000101),
}SerialAudioInputFormatSelect;

typedef enum{
	POWCTRR 	= 	0x01,
	DIGCTRR 	= 	0x02,
	VOLCTRR 	= 	0x03,
	LVOLCTRR 	= 	0x04,
	RVOLCTRR 	= 	0x05,
	ANLCTRR 	= 	0x06,
	ESR 		= 	0x08,
	DGCCRR2 	= 	0x10,
	DGCCRR1 	= 	0x11,
}tas5720aq1_registers;


typedef enum{
	tas5720aq1_playing 	= 	0x00,
	tas5720aq1_notInit 	= 	0x01,
	tas5720aq1_shutDown	= 	0x02,
	tas5720aq1_sleep 	= 	0x03,
	tas5720aq1_mute 	= 	0x04,
} tas5720aq1_State;


typedef struct{
	uint32_t						DigitalClipper;
	SerialAudioInputFormatSelect 	SerialInputFormat;
	SerialAudioInputChSelect 		ChannelSelect;
	SpeedModeSelect 				SpeedMode;
	DigitalBoostSelect 				DigitalBoost;
	HPFilterSelect 					HPFilter;
	AnalogGainSelect 				AnalogGain;
	PWMRateSelect 					PWMRate;
	VolumeFadeSelect				VolumeFade;
}tas5720aq1_parameters;


typedef struct{
	uint8_t 				I2cAddress;
	tas5720aq1_State 		State;
	GPIO_TypeDef*			SDPort;
	uint16_t				SDPin;
	GPIO_TypeDef*			FaultPort;
	uint16_t				FaultPin;
	I2C_HandleTypeDef 		I2c_handle;
	uint8_t					ErrorCounter;
	uint8_t 				ErrorCode;
	tas5720aq1_parameters   user_parameters;
	SogeHandlerTypedef		soge;
}tas5720aq1_Instance;

/********** PORT FUNC*******/
void tas5720aq1_SetCommonParam(void);
void tas5720aq1_Init(void);
void tas5720aq1_SendVolume( tas5720aq1_Instance *tas5720aq1, uint8_t volume);
void tas5720aq1_Wakeup(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_Sleep(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_Mute(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_Unmute(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_SwShutdown(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_SwStandby(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_ReadFaultPort(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_HwShutDown(tas5720aq1_Instance *tas5720aq1);
void tas5720aq1_HwStanby(tas5720aq1_Instance *tas5720aq1);
uint8_t tas5720aq1_DiagnosisCheck(tas5720aq1_Instance *tas5720aq1);
tas5720aq1_ErrorStatus tas5720aq1_LLInit(tas5720aq1_Instance *tas5720aq1);


#endif /* USER_DRIVERS_INC_TAS5720AQ1_R01_H_ */

/*
 * TAS5720AQ1_R2.c
 *
 *  Created on: 4 Eyl 2020
 *      Author: Arge-1
 */

/*Bu versiyon TI-TAS5720AQ1 AVAS grade ses yükseltecini nesne yönelimli programlama
 * teknikleriyle yazmak amacıyla güncellenmiştir.
 * Bu versiyonla A nesnesi B (ya da modellenecek diğer nesneler) nesnesinin özelliklerini kullanamaz ve erişemez.
 * Tüm nesneler için ortak olan parametreler ile her nesnenin kendine ait özellikleri (absraction)
 * yapı içine alınmış bunların kullanıcı tarafından doldurulması istenmiştir.
 * Alt katman başlatmasında girilen parametrelerin (eksikliği veya yanlışlığı kontrol edilmiş)
 * bilgi eksikliği, yanlışlığı veya fazlalığı önlenmiştir.
 * Geliştirmeler devam edecektir.
 *
Author is : IHK
*/

#include "tas5720aq1_r01.h"

/*-----------------ASSERTION BLOCK-----------------*/
#define ASSERT(_expression_) if (!_expression_) myparam_assert((char*)__FILE__, __LINE__)

void myparam_assert(const char* file_name, int line){
	printf("Parameters are not good, try again! Assertion failed -> file: %s, line: %d\n", file_name, line);
	while(1)
	;
}
/*-----------------ASSERTION BLOCK-----------------*/

/*-----------------DEPENDENCY BLOCK-----------------*/
static 			funcptr 			i2c_tas_detector	= &HAL_I2C_IsDeviceReady;
static			fun2ptr 			tas5720_i2c_write	= &I2C_Write_Register;
static 			fun2ptr 			tas5720_i2c_read	= &I2C_Read_Register;
static 			fun2ptr1 			tas5720_port_read	= &HAL_GPIO_ReadPin;
static 			fun2ptr2 			tas5720_port_write	= &HAL_GPIO_WritePin;
static 			uint8_t 			trial				= 0U;
static 			uint8_t 			tasData[3];
/*-----------------DEPENDENCY BLOCK-----------------*/

/*-----------------FUNCTION PROTOTYPES-----------------*/
static void 	tas5720aq1_ErrorHandler(tas5720aq1_flagstatus flag);
static uint8_t 	tas5720aq1_SetVolume(uint8_t volume);
static double  	tas5720aq1_VolumeMapping(long , long , long , long , long );
/*-----------------FUNCTION PROTOTYPES-----------------*/
tas5720aq1_registers 	TAS5720AQ1_LLReg;

tas5720aq1_ErrorStatus tas5720aq1_LLInit(tas5720aq1_Instance *tas5720aq1){
	ASSERT(tas5720aq1->I2cAddress!=0);
	ASSERT(tas5720aq1->SDPort!=0);
	ASSERT(tas5720aq1->SDPin!=0);
	tasData[1]=0x00; //Reset content
	tas5720aq1_HwShutDown(tas5720aq1);

	if(i2c_tas_detector(&(tas5720aq1->I2c_handle), tas5720aq1->I2cAddress,1,10)!=HAL_OK){
		tas5720aq1_ErrorHandler(SET_ERR);
		return ERROR;
	}

	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	SET_BIT(tasData[1],SPK_SLEEP); 	 //Sleep is ON
	CLEAR_BIT(tasData[1], SPK_SD);	 //ShutDown is ON
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]); //Shut down OK.

	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR,&tasData[1]);
	SET_BIT(tasData[1],RIGHT_CH);	//The right channel is muted.
	SET_BIT(tasData[1],LEFT_CH);	//The left channel is muted.

	if(tas5720aq1->user_parameters.VolumeFade==VolumeFade_Enabled){
		SET_BIT(tasData[1],VOL_FADE);
	}
	else{
		CLEAR_BIT(tasData[1],VOL_FADE);
	}

	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR,&tasData[1]);
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, DIGCTRR,&tasData[1]);

	if(tas5720aq1->user_parameters.HPFilter==HPFilterIs_Bypassed){
		SET_BIT(tasData[1],HP_FILTER);
	}
	else{
		CLEAR_BIT(tasData[1],HP_FILTER);
	}

	if(tas5720aq1->user_parameters.DigitalBoost==Boosted_0dB){
		CLEAR_BIT(tasData[1],DB0);
		CLEAR_BIT(tasData[1],DB1);
		//CLEAR_BIT(tasData[1],(DB1+DB0);
	}

	else if(tas5720aq1->user_parameters.DigitalBoost==Boosted_6dB){
		CLEAR_BIT(tasData[1],DB0);
		SET_BIT(tasData[1],DB1);
	}

	else if(tas5720aq1->user_parameters.DigitalBoost==Boosted_12dB){
		SET_BIT(tasData[1],DB0);
		CLEAR_BIT(tasData[1],DB1);
	}

	else{
		SET_BIT(tasData[1],DB0);
		SET_BIT(tasData[1],DB1);
		//SET_BIT(tasData[1],(DB1+DB0);
	}


	if(tas5720aq1->user_parameters.SpeedMode==SingleSpeed){
		CLEAR_BIT(tasData[1],SM);
	}
	else{
		SET_BIT(tasData[1],SM);
	}

	if(tas5720aq1->user_parameters.SerialInputFormat==Default_I2Sformat){
		SET_BIT(tasData[1],SIF2);
		CLEAR_BIT(tasData[1],SIF1);
		CLEAR_BIT(tasData[1],SIF0);
	}
	else if(tas5720aq1->user_parameters.SerialInputFormat==RightJustified_24Bits){
		CLEAR_BIT(tasData[1],SIF2);
		CLEAR_BIT(tasData[1],SIF1);
		CLEAR_BIT(tasData[1],SIF0);
	}
	else if(tas5720aq1->user_parameters.SerialInputFormat==RightJustified_20Bits){
		CLEAR_BIT(tasData[1],SIF2);
		CLEAR_BIT(tasData[1],SIF1);
		SET_BIT(tasData[1],SIF0);
	}
	else if(tas5720aq1->user_parameters.SerialInputFormat==RightJustified_18Bits){
		CLEAR_BIT(tasData[1],SIF2);
		SET_BIT(tasData[1],SIF1);
		CLEAR_BIT(tasData[1],SIF0);
	}
	else if(tas5720aq1->user_parameters.SerialInputFormat==RightJustified_16Bits){
		CLEAR_BIT(tasData[1],SIF2);
		SET_BIT(tasData[1],SIF1);
		SET_BIT(tasData[1],SIF0);
	}

	else{
		SET_BIT(tasData[1],SIF2);
		CLEAR_BIT(tasData[1],SIF1);
		SET_BIT(tasData[1],SIF0);
	}

	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, DIGCTRR,&tasData[1]);
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, ANLCTRR,&tasData[1]);
	SET_BIT(tasData[1],BIT7);//7.bit reserved but must be set to 1. Check datasheet for more info.

	if(tas5720aq1->user_parameters.PWMRate==SwRateIs_16LRCK){
		SET_BIT(tasData[1],BIT6);
		CLEAR_BIT(tasData[1],BIT5);
		SET_BIT(tasData[1],BIT4);
	}

	else if(tas5720aq1->user_parameters.PWMRate==SwRateIs_6LRCK){
		CLEAR_BIT(tasData[1],BIT6);
		CLEAR_BIT(tasData[1],BIT5);
		CLEAR_BIT(tasData[1],BIT4);
	}
	else if(tas5720aq1->user_parameters.PWMRate==SwRateIs_20LRCK){
		SET_BIT(tasData[1],BIT6);
		SET_BIT(tasData[1],BIT5);
		CLEAR_BIT(tasData[1],BIT4);
	}
	else if(tas5720aq1->user_parameters.PWMRate==SwRateIs_24LRCK){
		SET_BIT(tasData[1],BIT6);
		SET_BIT(tasData[1],BIT5);
		SET_BIT(tasData[1],BIT4);
	}
	if(tas5720aq1->user_parameters.AnalogGain==AnlGainSettingIs_25dB){
		SET_BIT(tasData[1],BIT3);
		CLEAR_BIT(tasData[1],BIT2);

	}
	else if(tas5720aq1->user_parameters.AnalogGain==AnlGainSettingIs_19dB){
		CLEAR_BIT(tasData[1],BIT3);
		CLEAR_BIT(tasData[1],BIT2);
	}
	else{
		CLEAR_BIT(tasData[1],BIT3);
		SET_BIT(tasData[1],BIT2);
	}

	if(tas5720aq1->user_parameters.ChannelSelect==RChSelected){
		CLEAR_BIT(tasData[1],BIT1);
	}
	else{
		SET_BIT(tasData[1],BIT1);
	}

	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, ANLCTRR, &tasData[1]);
	tas5720aq1_HwStanby(tas5720aq1);

	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);

	CLEAR_BIT(tasData[1],SPK_SLEEP); 	//Sleep is OFF
	SET_BIT(tasData[1], SPK_SD);	 //ShutDown is OFF
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR,&tasData[1]); //STAND BY.

	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);
	CLEAR_BIT(tasData[1],RIGHT_CH);	//The right channel is playing.
	CLEAR_BIT(tasData[1],LEFT_CH);	//The left channel is playing
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);

	tasData[1]=0x07; //Mute
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, LVOLCTRR, &tasData[1]);
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, RVOLCTRR, &tasData[1]);
	tas5720aq1->State=tas5720aq1_playing ;
	tas5720aq1->ErrorCode|= NERR;

	return SUCCESS;
}



static uint8_t tas5720aq1_SetVolume(uint8_t volume){
	return (volume!=0) ? tas5720aq1_VolumeMapping(volume, 1, 100, 190, 255) : 6;
	//Any settings less than 7 places chanel in mute.
}

void tas5720aq1_SendVolume( tas5720aq1_Instance *tas5720aq1, uint8_t volume){
	if(tas5720aq1->State==tas5720aq1_sleep)
		tas5720aq1_Wakeup(tas5720aq1);
	tasData[1]=tas5720aq1_SetVolume(volume);
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, RVOLCTRR, &tasData[1]);
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, LVOLCTRR, &tasData[1]);
}

void tas5720aq1_Wakeup(tas5720aq1_Instance *tas5720aq1){
	tas5720aq1->State=tas5720aq1_playing;
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	tasData[1]&= ~(2);//Device is now placed out of sleep mode.
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
}

void tas5720aq1_Sleep(tas5720aq1_Instance *tas5720aq1){
	tas5720aq1->State= tas5720aq1_sleep;
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	tasData[1]&= ~(2); // Masks all bits except [1] to save their current value.
	tasData[1]|= (1<<1);//Device is now placed in sleep mode.
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
}

void tas5720aq1_Mute(tas5720aq1_Instance *tas5720aq1){
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);
	SET_BIT(tasData[1],RIGHT_CH);	//The right channel in mute.
	SET_BIT(tasData[1],LEFT_CH);	//The left channel in mute
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);
	tas5720aq1->State= tas5720aq1_mute;
}

void tas5720aq1_Unmute(tas5720aq1_Instance *tas5720aq1){
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);
	CLEAR_BIT(tasData[1],RIGHT_CH);	//The right channel out of  mute.
	CLEAR_BIT(tasData[1],LEFT_CH);	//The left channel out of mute
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, VOLCTRR, &tasData[1]);
	tas5720aq1->State= tas5720aq1_playing;
}

void tas5720aq1_SwShutdown(tas5720aq1_Instance *tas5720aq1){
	tas5720aq1_Sleep(tas5720aq1);
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	CLEAR_BIT(tasData[1],SPK_SD);//Shut Down Speaker
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	tas5720aq1->State=tas5720aq1_shutDown;
}

void tas5720aq1_SwStandby(tas5720aq1_Instance *tas5720aq1){
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	SET_BIT(tasData[1],SPK_SD);//Standby Speaker
	tas5720_i2c_write(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	tas5720aq1->State=tas5720aq1_playing;
}

void tas5720aq1_Reset(tas5720aq1_Instance *tas5720aq1){
	tas5720aq1_SwShutdown(tas5720aq1);
	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, POWCTRR, &tasData[1]);
	tas5720aq1_LLInit(tas5720aq1);
}

uint8_t tas5720aq1_DiagnosisCheck(tas5720aq1_Instance *tas5720aq1){

	tas5720_i2c_read(tas5720aq1->I2c_handle, tas5720aq1->I2cAddress, ESR, &tasData[1]);
	tas5720aq1->ErrorCode=tasData[1];

	if(tas5720aq1->ErrorCode!=0x00){
		 if(READ_BIT(tas5720aq1->ErrorCode, OTE)){
			 tas5720aq1->ErrorCounter+=TAS5720AQ1_OTE_ERROR;
			 tas5720aq1->ErrorCode|=OTE;
			//tas5720aq1_Reset(tas5720aq1);
		}

		else if(READ_BIT(tas5720aq1->ErrorCode, DCE)){
			tas5720aq1->ErrorCounter+=TAS5720AQ1_DCE_ERROR;
			tas5720aq1->ErrorCode|=DCE;
		}

		else if(READ_BIT(tas5720aq1->ErrorCode, OCE)){
			tas5720aq1->ErrorCounter+=TAS5720AQ1_OCE_ERROR;
			tas5720aq1->ErrorCode|= OCE;
		}

		else if(READ_BIT(tas5720aq1->ErrorCode, CLKE)){
			tas5720aq1->ErrorCounter+=TAS5720AQ1_CLKE_ERROR;
			tas5720aq1->ErrorCode|= CLKE;
		}


		if((tas5720aq1->ErrorCounter>= TAS5720AQ1_ERROR_THRESHOLD)){
			tas5720aq1_ErrorHandler(SET_ERR);
			//tas5720aq1_Reset(tas5720aq1);// ???
			tas5720aq1->ErrorCounter=0;
			if(trial>=3){
				trial=0;
					//tas5720aq1_Shutdown(tas5720aq1);
			}
				trial++;
		}
	}

	else{
		if(tas5720aq1->ErrorCounter>0){
			tas5720aq1->ErrorCounter-=TAS5720AQ1_ERROR_DEC;

			if(tas5720aq1->ErrorCounter==0){
				tas5720aq1->ErrorCode= NERR;
				tas5720aq1_ErrorHandler(CLEAR_ERR);
			}
		}

	}
	return tas5720aq1->ErrorCode;
}

void tas5720aq1_ReadFaultPort(tas5720aq1_Instance *tas5720aq1){
	tas5720_port_read(tas5720aq1->FaultPort, tas5720aq1->FaultPin);
}

void tas5720aq1_HwShutDown(tas5720aq1_Instance *tas5720aq1){
	tas5720_port_write(tas5720aq1->SDPort, tas5720aq1->SDPin, GPIO_PIN_RESET); //SD by HW
}

void tas5720aq1_HwStanby(tas5720aq1_Instance *tas5720aq1){
	tas5720_port_write(tas5720aq1->SDPort, tas5720aq1->SDPin, GPIO_PIN_SET); //Sby by HW
}

static double tas5720aq1_VolumeMapping(long volume, long in_min_volume, long in_max_volume, long out_min_volume, long out_max_volume){
	return (volume - in_min_volume) * (out_max_volume - (out_min_volume)) / (in_max_volume - in_min_volume) + (out_min_volume);
}

static void tas5720aq1_ErrorHandler(tas5720aq1_flagstatus flag){
	/*if(flag==SET_ERR)
		tas5720_port_write(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
	if(flag==CLEAR_ERR)
		tas5720_port_write(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);*/
}


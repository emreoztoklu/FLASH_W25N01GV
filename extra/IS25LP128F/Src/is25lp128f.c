/*
 * stm32fxx_user_is25lp128f.c
 *
 *  Created on: 23 Tem 2020
 *      Author: arge1
 */


#include "../Inc/is25lp128f.h"

#include "stdlib.h"

static IS25LP128F_typedef IS25LP128F;
#ifdef _4BYTE_ADDRESS_MODE
uint8_t spiData[5];
#else
uint8_t spiData[260];
#endif
uint8_t dummy[3];

void IS25LP128F_ReadID()
{
	spiData[0]=RDID;
	spiData[1]=DUMMY;
	spiData[2]=DUMMY;
	spiData[3]=DUMMY;
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, 4);
	IS25LP128F.ReadDev(spiData, 1);
	IS25LP128F.StopComm();
	IS25LP128F.ID= spiData[0];
}
void IS25LP128F_Enable4ByteAdressingMode()
{
	spiData[0]=EN4B;
	IS25LP128F.WriteDev(spiData, 1);
}
void IS25LP128F_ReadDataFromFlash(uint32_t read_address,  void *rdBuff, uint32_t size, DataTypeDef dataType)
{
	/*TO DO: eğer malloc yeterli bellek alanı ayıramazsa hata döndersin.*/
	/*WARNING: dinamik belleğin sisteme geri verilmesi hangi noktada yapılacak
	 * tekrardan gözden geçirilecek (global olarak tanımlanabılır)*/
	void *tempBuff;
	if(dataType==DATA_TYPE_8)
		tempBuff = (uint8_t*) malloc(size*sizeof(uint8_t));
	else if(dataType==DATA_TYPE_16)
		tempBuff = (uint16_t*) malloc(size*sizeof(uint16_t));
	else
		tempBuff = (uint32_t*) malloc(size*sizeof(uint32_t));
#ifdef _4BYTE_ADDRESS_MODE
#if(FAST_READ_MODE)
		spiData[0]=_4FRD;
#else //NORMAL_READ_MODE
		spiData[0]=_4NRD;
#endif
		spiData[1]= (read_address >> 24) & 0x000000FF;
		spiData[2]= (read_address >> 16) & 0x000000FF;
		spiData[3]= (read_address >> 8) & 0x000000FF;
		spiData[4]= read_address & 0x000000FF;
		WriteDev(spiData, sizeof(spiData));
		ReadDev(tempBuff, size);
		rdBuff= tempBuff;
		free (tempBuff);
#else //3 byte address mode
#if(FAST_READ_MODE)
	spiData[0]=_3FRD;
#else
	spiData[0]=_3NRD;
#endif
	spiData[1]= (read_address >> 16) & 0x000000FF;
	spiData[2]= (read_address >> 8) & 0x000000FF;
	spiData[3]= read_address & 0x000000FF;
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, 4);
	IS25LP128F.ReadDev(&spiData[0], size);
	IS25LP128F.StopComm();
	rdBuff= tempBuff;
	free (tempBuff);
#endif

}
void IS25LP128F_SetWriteEnable()
{
	IS25LP128F.StartComm();
	spiData[0]= WREN;
	IS25LP128F.WriteDev(&spiData[0], 1);
	IS25LP128F.StopComm();
	//ReadDev(spiData, 1);
}
void IS25LP128F_ReadStatusRegister()
{
	IS25LP128F.StartComm();
	spiData[0]= RDSR;
	IS25LP128F.WriteDev(&spiData[0], 1);
	IS25LP128F.ReadDev(&spiData[1], 1);
	IS25LP128F.StopComm();
	IS25LP128F.SR=spiData[1];
}
void IS25LP128F_WriteStatusRegister()
{

}
IS25LP128F_Status IS25LP128F_CheckWriteStatus()
{
	IS25LP128F_ReadStatusRegister();
	if((READ_BIT(IS25LP128F.SR,WIP)==SET)||(READ_BIT(IS25LP128F.SR,WEL)==RESET))
	{
		Error_Handler();
		return BUSY;
	}
	else
		return READY;
}
IS25LP128F_Status IS25LP128F_CheckWriteFlag()
{
	IS25LP128F_ReadStatusRegister();
	if((READ_BIT(IS25LP128F.SR,WIP)==SET))
	{
		Error_Handler();
		return BUSY;
	}
	else
		return READY;
}
ErrorStatus IS25LP128F_CheckBlockProtectionStatus()
{
	IS25LP128F_ReadStatusRegister();
	if((READ_BIT(IS25LP128F.SR,(BP0+BP1+BP2+BP3)))==RESET)
		return SUCCESS;
	else
		return ERROR;
}
ErrorStatus IS25LP128F_EraseSector(uint32_t sector_address)
{
	uint32_t timerstart;
	uint16_t Timeout=MAX_CHIP_ERASE_TIME;
	IS25LP128F_SetWriteEnable();
	if(IS25LP128F_CheckWriteStatus()!=READY)
	{
		Error_Handler();
		return ERROR;
	}
	IS25LP128F.StartComm();
	spiData[0]=CER;
	IS25LP128F.WriteDev(spiData, 1);
	IS25LP128F.StopComm();
	timerstart= IS25LP128F.TickTime();
	while(IS25LP128F_CheckWriteFlag()!=READY)
	{
		 if (((IS25LP128F.TickTime() - timerstart) > Timeout))
		 {
			 IS25LP128F.StopComm();
			 return ERROR;
		 }

	}

	return SUCCESS;
}
ErrorStatus IS25LP128F_WritePage(uint32_t write_address, const void* data, uint32_t size)
{

	/*TO DO: CHECK AREA TO BE WRITE WHETHER PROTECTED AREA IS.*/
	/* TO DO: ERASE OPERATIONS */
	uint32_t timerstart;
	uint16_t Timeout=100;
	//IS25LP128F_EraseSector(write_address);
	if(size> MAX_PAGE_WR_SIZE)
	{
		Error_Handler();
		return 0;
	}
	IS25LP128F_SetWriteEnable();
	if(IS25LP128F_CheckWriteStatus()!=READY)
	{
		Error_Handler();
		return ERROR;
	}
#ifdef _4BYTE_ADDRESS_MODE
	/*CHECK WHETHER EXTADDR=1 */
	spiData[0]=_4PP;
	spiData[1]= (write_address >> 24) & 0x000000FF;
	spiData[2]= (write_address >> 16) & 0x000000FF;
	spiData[3]= (write_address >> 8) & 0x000000FF;
	spiData[4]= write_address & 0x000000FF;
	WriteDev(spiData, sizeof(spiData));
	WriteDev((uint8_t*)data, size);
	IS25LP128F.StopComm();
#else
	spiData[0]=_3PP;
#endif
	spiData[1]= (write_address >> 16) & 0x000000FF;
	spiData[2]= (write_address >> 8) & 0x000000FF;
	spiData[3]= write_address & 0x000000FF;
	for(uint8_t i=4;i<size+4;i++)
	{
		spiData[i]=*(uint8_t*)data;
		data++;
	}
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, size+4);
	//WriteDev((uint8_t*)data, size);/*CLOCK KAÇIRMA IHTIMALINI GOZDEN GECIR*/
	IS25LP128F.StopComm();
	timerstart= HAL_GetTick();
	while(IS25LP128F_CheckWriteFlag()!=READY)
	{
		 if (((IS25LP128F.TickTime() - timerstart) > Timeout))
		 {
			 IS25LP128F.StopComm();
			 return ERROR;
		 }

	}
	return SUCCESS;
}

ErrorStatus IS25LP128F_EraseBlock32KB(uint32_t block_address)
{
	uint32_t timerstart;
	uint16_t Timeout=MAX_32KBLOCK_ERASE_TIME;
	IS25LP128F_SetWriteEnable();
	if(IS25LP128F_CheckWriteStatus()!=READY)
	{
		Error_Handler();
		return ERROR;
	}
#ifdef _4BYTE_ADDRESS_MODE
	spiData[0]=_4BER32K;
	spiData[1]= (block_address >> 24) & 0xFF;
	spiData[2]= (block_address >> 16) & 0xFF;
	spiData[3]= (block_address >> 8) & 0xFF;
	spiData[4]= block_address & 0xFF;
#else
	spiData[0]=BER32K;
	spiData[1]= (block_address >> 16) & 0xFF;
	spiData[2]= (block_address >> 8) & 0xFF;
	spiData[3]= block_address & 0xFF;
#endif
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, 4);
	IS25LP128F.StopComm();
	timerstart= IS25LP128F.TickTime();
	while(IS25LP128F_CheckWriteFlag()!=READY)
	{
		 if (((IS25LP128F.TickTime() - timerstart) > Timeout))
		 {
			 IS25LP128F.StopComm();
			 return ERROR;
		 }
	}
	return SUCCESS;
}
ErrorStatus IS25LP128F_EraseBlock64KB(uint32_t block_address)
{
	uint32_t timerstart;
	uint16_t Timeout=MAX_64KBLOCK_ERASE_TIME;
	IS25LP128F_SetWriteEnable();
	if(IS25LP128F_CheckWriteStatus()!=READY)
	{
		Error_Handler();
		return ERROR;
	}
#ifdef _4BYTE_ADDRESS_MODE
	spiData[0]=_4BER64K;
	spiData[1]= (block_address >> 24) & 0xFF;
	spiData[2]= (block_address >> 16) & 0xFF;
	spiData[3]= (block_address >> 8) & 0xFF;
	spiData[4]= block_address & 0xFF;
#else
	spiData[0]=BER64K;
	spiData[1]= (block_address >> 16) & 0xFF;
	spiData[2]= (block_address >> 8) & 0xFF;
	spiData[3]= block_address & 0xFF;
#endif
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, sizeof(spiData));
	IS25LP128F.StopComm();
	timerstart= IS25LP128F.TickTime();
	while(IS25LP128F_CheckWriteFlag()!=READY)
	{
		 if (((IS25LP128F.TickTime() - timerstart) > Timeout))
		 {
			 IS25LP128F.StopComm();
			 return ERROR;
		 }

	}
	return SUCCESS;
}
ErrorStatus IS25LP128F_EraseChip()
{
	uint32_t timerstart;
	uint16_t Timeout=MAX_CHIP_ERASE_TIME;
	IS25LP128F_SetWriteEnable();
	if(IS25LP128F_CheckBlockProtectionStatus()!=SUCCESS)
	{
		Error_Handler();
		return ERROR;
	}
	spiData[0]=CER;
	IS25LP128F.StartComm();
	IS25LP128F.WriteDev(spiData, 1);
	IS25LP128F.StopComm();
	timerstart= IS25LP128F.TickTime();
	while(IS25LP128F_CheckWriteFlag()!=READY)
	{
		 if (((IS25LP128F.TickTime() - timerstart) > Timeout))
		 {
			 IS25LP128F.StopComm();
			 return ERROR;
		 }

	}
	return SUCCESS;
}


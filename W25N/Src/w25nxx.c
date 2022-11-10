/****************************************************************
 * Project_Name: W25N01GV_test
 * File_name   : w25n01gv.c
 *
 *  Created on: Sep 21, 2022
 *      Author: Eng.Emre ÖZTOKLU
 *
 *****************************************************************/

/*********************  INCLUDES *********************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "w25nxx.h"
#include "../../../Peripherals/eo_spi.h"
#include "../../../Peripherals/eo_tim.h"
#include "../../../Peripherals/eo_usart.h"

#if(_W25NXX_DEBUG == 1)
#include <stdio.h>
#endif

#define PUBLIC
#define PRIVATE static
#define POW_2(x)  (2*x)


/**************************************************************************************/
w25nxx_t _w25nxx;


/**************************************************************************************/
PRIVATE W25_RESULT_t W25N_WriteSpi(uint8_t *Txdata, uint16_t size){
	if(HAL_SPI_Transmit((SPI_HandleTypeDef *)&_W25NXX_SPI, Txdata, size, 500) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}



PRIVATE W25_RESULT_t W25N_ReadSpi(uint8_t *Rxdata, uint16_t size){
	if(HAL_SPI_Receive((SPI_HandleTypeDef *)&_W25NXX_SPI, Rxdata, size, 500) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}



PRIVATE W25_RESULT_t W25N_WriteReadSpi(uint8_t Txdata, uint8_t *Rxdata, uint16_t size){
	if(HAL_SPI_TransmitReceive((SPI_HandleTypeDef *)&_W25NXX_SPI, &Txdata, Rxdata, size, 500) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}

PRIVATE W25_RESULT_t W25N_ReadSpiDMA(uint8_t *Rxdata, uint16_t size){
	if(HAL_SPI_Receive_DMA((SPI_HandleTypeDef *)&_W25NXX_SPI, Rxdata, size) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}

PRIVATE W25_RESULT_t W25N_WriteSpiDMA(uint8_t *Txdata, uint16_t size){
	if(HAL_SPI_Transmit_DMA((SPI_HandleTypeDef *)&_W25NXX_SPI, Txdata, size) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}

PRIVATE W25_RESULT_t W25N_WriteReadSpiDMA(uint8_t Txdata, uint8_t *Rxdata, uint16_t size){
	if(HAL_SPI_TransmitReceive_DMA((SPI_HandleTypeDef *)&_W25NXX_SPI, &Txdata, Rxdata, size) == HAL_ERROR)
		return W25_ERROR;
	return W25_OK;
}

/**************************************************************************************/
/**************************************************************************************/
PUBLIC uint32_t W25nxx_PageToSector(uint32_t pageAddr){
	if(pageAddr > _w25nxx.PageCount-1) return 1;
	return (uint32_t)((pageAddr * _w25nxx.PageSize)/_w25nxx.SectorSize);
	// (0.page*2048pagesize)/512sectorsize = 0.sector
	// (1.page*2048pagesize)/512sectorsize = 4.sector
	// (2.page*2048pagesize)/512sectorsize = 8.sector
}

PUBLIC uint32_t W25nxx_PageToBlock(uint32_t pageAddr){
	if(pageAddr > _w25nxx.PageCount-1) return 1;
	return (uint32_t)((pageAddr * _w25nxx.PageSize)/_w25nxx.BlockSize);
	// (1.page*2048pagesize)  / 131072blocksize = 0,0... (0.block)
	// (63.page*2048pagesize) / 131072blocksize = 0,98.. (0.block)
	// (64.page*2048pagesize) / 131072blocksize = 1. (1.block)
}

PUBLIC uint32_t W25nxx_SectorToBlock(uint32_t SectorAddr){
	if(SectorAddr > _w25nxx.SectorCount-1) return 1;
	return (uint32_t)((SectorAddr * _w25nxx.SectorSize)/_w25nxx.BlockSize); // (1025 * 512)/131072 = 4,
	// 0.sector * 512 sectorsize / 131072blocksize =  0.block
	// 4.sector * 512 sectorsize / 131072blocksize =  0.block
	// 256.sector * 512 sectorsize / 131072blocksize = 1.block
	// 512.sector * 512 sectorsize / 131072blocksize = 2.block

}

PUBLIC uint32_t W25nxx_SectorToPage(uint32_t SectorAddr){
	if(SectorAddr > _w25nxx.SectorCount-1) return 1;
	return (uint32_t)((SectorAddr * _w25nxx.SectorSize)/_w25nxx.PageSize); // (1025 *512)/2048 = 256
	// 0.sector * 512 sectorsize / 2048page size = 0.page
	// 4.sector * 512 sectorsize / 2048page size = 1.page
	// 256.sector* 512 sectorsize / 2048page size = 64.page
	// 512.sector * 512 sectorsize/ 2048page size = 128.page
}

PUBLIC uint32_t W25nxx_BlockToPage(uint32_t BlockAddr){
	if(BlockAddr > _w25nxx.BlockCount-1) return 1;

	return (uint32_t)((BlockAddr * _w25nxx.BlockSize)/_w25nxx.PageSize);  // (10 * 131072)/2048 = 640.Block
	// (0.block*131072blocksize) / 2048page size  = 0 pageaddress
	// (1.block*131072blocksize) / 2048page size  = 64 pageaddress
	// (2.block*131072blocksize) / 2048page size  = 128 pageaddres

}
/**************************************************************************************/
PUBLIC uint8_t W25nxx_Read_SR(uint8_t noSR){

	uint8_t status = 0;
	uint8_t rx = 0;
	uint8_t cmdbuf[3][3] = {{READ_STATUS_REGISTER, SR1_PROT_REG, W25N_DUMMY_BYTE },
							{READ_STATUS_REGISTER, SR2_CONFIG_REG, W25N_DUMMY_BYTE },
							{READ_STATUS_REGISTER, SR3_STAT_REG, W25N_DUMMY_BYTE}};

	W25N_SELECT;
	for(int i = 0; i < 3; i++){
		if ((status = W25N_WriteReadSpi(cmdbuf[noSR-1][i], &rx, 1)) != W25_OK){
			return status;
		}
	}
	W25N_DESELECT;

	switch (noSR) {
		case Prot_Reg:
			_w25nxx.flash_SR.SR1 = rx;
			break;
		case Config_reg:
			_w25nxx.flash_SR.SR2 = rx;
			break;
		case Stat_reg:
			_w25nxx.flash_SR.SR3 = rx;
			break;
	}

	return W25_OK;
	// After Read_SR register check BUSY bit for next instructer cmd
}

PUBLIC void W25nxx_CheckStatusRegisters(void){
	W25nxx_Read_SR(Prot_Reg);
	W25nxx_Read_SR(Config_reg);
	W25nxx_Read_SR(Stat_reg);
}

PRIVATE void W25nxx_WaitForReady(void){
	do{
		if (W25nxx_Read_SR(Stat_reg)!= W25_OK){
#if(_W25NXX_DEBUG == 1)
		printf("Error: W25nxx_Read_SR");
#endif
		}
	}while(_w25nxx.flash_SR.SR3 & SR3_S0_BUSY);
}

PUBLIC void W25nxx_DeviceReset(void){
	//W25nxx_WaitForReady(); 			// its recommended to check BUSY bit before restart
	uint8_t cmd = DEVICE_RESET;
	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
		W25N_DESELECT;
	}
	W25N_DESELECT;
	W25N_Delayus(600);  				//the device will take approximately tRST to reset,
										//depending on the current operation the device is performing, tRST can be 5us~500us.
										//During this period, no command will be accepted.
	//Check after this function for the next instruction BUSY bit on Status register
	W25nxx_WaitForReady();
}

PRIVATE void W25nxx_Write_SR(uint8_t noSR, uint8_t data){
	uint8_t cmd = WRITE_STATUS_REGISTER;

	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
		W25N_DESELECT;
	}

	if(noSR == Prot_Reg){
		cmd = SR1_PROT_REG;
		if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
			W25N_DESELECT;
		}
		_w25nxx.flash_SR.SR1 = data;
	}

	else if(noSR == Config_reg){
		cmd = SR2_CONFIG_REG;
		if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
			W25N_DESELECT;
		}
		_w25nxx.flash_SR.SR2 = data;
	}

	else{
		cmd = SR3_STAT_REG;
		if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
			W25N_DESELECT;
		}
		_w25nxx.flash_SR.SR3 = data;
	}

	if(W25N_WriteSpi((uint8_t*)&data, 1)!= W25_OK){
		W25N_DESELECT;
	}
	W25N_DESELECT;
}

PRIVATE W25_RESULT_t W25nxx_WriteEnable(void){
	uint8_t status = 0;
	uint8_t cmd = WRITE_ENABLE;

	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
		W25N_DESELECT;
		return W25_ERROR;
	}
	W25N_DESELECT;
	W25N_Delayus(500);     //W25N_Delayms(1);

	if ((status = W25nxx_Read_SR(3)) != W25_OK)
		return W25_ERROR;

	if(_w25nxx.flash_SR.SR3 & SR3_S1_WEL)
		return W25_OK;
	else
		return W25_ERROR;
}

PRIVATE W25_RESULT_t W25nxx_WriteDisable(void){
	uint8_t status = 0;
	uint8_t cmd = WRITE_DISABLE;

	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)&cmd, 1)!= W25_OK){
		W25N_DESELECT;
		return W25_ERROR;
	}
	W25N_DESELECT;
	W25N_Delayus(500);     //W25N_Delayms(1);

	if ((status = W25nxx_Read_SR(3)) == W25_OK){
		if ((_w25nxx.flash_SR.SR3 & SR3_S1_WEL) == 0x00)
			return W25_OK;
	}
	return W25_ERROR;
}

PRIVATE W25_RESULT_t W25nxx_BlockErase(uint16_t pageAddr){
	while(_w25nxx.Lock == 1)
		W25N_Delayus(400);     //W25N_Delayms(1);
	_w25nxx.Lock = 1;

	if(pageAddr > (_w25nxx.PageCount-1))
		return W25_ERROR;

	char cmdbuf[4] = {BLOCK_ERASE_128KB, W25N_DUMMY_BYTE, ((pageAddr & 0xFF00) >> 8), (pageAddr & 0xFF)};

#if(_W25NXX_DEBUG == 1)
	//printf("w25nxx EraseBlock %d Started...\r\n", (int)pageAddr);
	//uint32_t StartTime = HAL_GetTick();
#endif

	W25nxx_WaitForReady();

	if(!W25nxx_WriteEnable()){
		W25N_SELECT;
		if(W25N_WriteSpi((uint8_t*)cmdbuf, 4)!= W25_OK){
			W25N_DESELECT;
			return W25_ERROR;
		}
		W25N_DESELECT;
		W25N_Delayms(20);  //Tbe = 10ms

#if(_W25NXX_DEBUG == 1)
	//	printf("w25nxx EraseBlock done after %d ms\r\n", (int)(W25N_GetTick - StartTime));
		W25N_Delayms(10);
#endif

		W25N_Delayms(1);	 // W25N_Delayus(300); //W25N_Delayms(1);
		_w25nxx.Lock = 0;
		/*
		 The Block Erase instruction will not be executed if
		 	 the addressed block is protected by the Block Protect (TB, BP2, BP1, and BP0) bits.
		*/
	}
	else{
		return W25_ERROR;
	}
	return W25_OK;
}

//section: 8.2.11   LoadProg :0  / Random_Load: 1
PRIVATE W25_RESULT_t W25nxx_LoadProgOrRandomLoadData(uint16_t columnAddr, uint8_t* dataArr, uint32_t lenght , LoadRandLoad_t loadtype){
	if(columnAddr >= (uint32_t)(_w25nxx.PageSize)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorColumn Addr should be between 0 and %d\r\n",(int)_w25nxx.PageSize-1);
#endif
		return W25_ERROR;
	}

	if(lenght > (uint32_t)(_w25nxx.PageSize - columnAddr)){
#if(_W25NXX_DEBUG == 1)
		printf("Errorlenght: SR2_BUF:1 ReadMode 2112 __SR2_BUF:0 ContinuosRead 2048 and %d\r\n",(int)lenght);
#endif
		return W25_ERROR;
	}

	uint8_t cmdbuf[3] = {LOAD_PROGRAM_DATA, ((columnAddr & 0xFF00) >> 8), (columnAddr & 0xFF)};


	if(loadtype == RandomLoad_Prog){
		cmdbuf[0] = RANDOM_LOAD_PROGRAM_DATA;
	}

	W25nxx_WaitForReady();

	if (!W25nxx_WriteEnable()){
		W25N_SELECT;
		if(W25N_WriteSpi(cmdbuf, 3)!= W25_OK){
			W25N_DESELECT;
			return W25_ERROR;
		}
		else {
			if (W25N_WriteSpi(dataArr, lenght)!= W25_OK){
				W25N_DESELECT;
				return W25_ERROR;
			}
		}
		W25N_DESELECT;
	}
	return W25_OK;
	//8.2.11 Load Program Data (02h) / Random Load Program Data (84h)
	//1.LoadProgramData: writes your data into Flash's Databuffer
	//2.Program Execute: Data fuffer will be written specifield memory page
}

PRIVATE W25_RESULT_t W25nxx_ProgramExecute(uint32_t pageAddr){
	if(pageAddr > (_w25nxx.PageCount-1)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorPage Addr should be between 0 and %d\r\n",(int)_w25nxx.PageCount-1);
#endif
		return W25_ERROR;
	}

	char cmdbuf[4] = {PROGRAM_EXECUTE, W25N_DUMMY_BYTE, ((pageAddr & 0xFF00) >> 8), (pageAddr & 0xFF)};

	W25nxx_WaitForReady();

	W25N_SELECT;

	if(W25N_WriteSpi((uint8_t*)cmdbuf, 4)!= W25_OK){
		W25N_DESELECT;
		return W25_ERROR;
	}

	W25N_DESELECT;

	W25N_Delayus(700);	//tppmax = 700us  typy: 250us

	return W25_OK;
}

PRIVATE W25_RESULT_t W25nxx_PageDataRead(uint32_t pageAddr){
	if(pageAddr > (_w25nxx.PageCount-1)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorPage Addr should be between 0 and %d\r\n",(int)_w25nxx.PageCount-1);
#endif
		return W25_ERROR;
	}

	char cmdbuf[4] = {PAGE_DATA_READ, W25N_DUMMY_BYTE,((pageAddr & 0xFF00) >> 8), (pageAddr & 0xFF) };

	W25nxx_WaitForReady();

	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)cmdbuf, 4)!= W25_OK){
		W25N_DESELECT;
		return W25_ERROR;
	}
	W25N_DESELECT;

	W25N_Delayus(70);		// instruction wont accept 60us other inst.


	return W25_OK;
}

PRIVATE W25_RESULT_t W25nxx_ReadData(uint16_t columnAddr, uint8_t* recieve_data, uint32_t lenght){
	if(columnAddr >= (uint32_t)(_w25nxx.PageSize-1)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorColumn Addr should be between 0 and %d\r\n",(int)_w25nxx.PageSize-1);
#endif
		return W25_ERROR;
	}

	if(lenght > (uint32_t)(_w25nxx.PageSize - columnAddr)){
#if(_W25NXX_DEBUG == 1)
		printf("Errorlenght: SR2_BUF:1 ReadMode 2112 __SR2_BUF:0 ContinuosRead 2048 and %d\r\n",(int)lenght);
#endif
		return W25_ERROR;
	}

	//lenght should be BUF:1 2112  BUF:0 2048  max
#ifdef _W25NXX_CONTINUOUS_READ
	//Check ECC is enabled bu sure enabled
#endif
	char cmdbuf[4] = {READ_DATA, 0x00, 0x00, W25N_DUMMY_BYTE };
	uint8_t status = 0;

	if(_w25nxx.flash_SR.SR2 & SR2_S3_BUF){
		cmdbuf [1] = ((columnAddr & 0xFF00) >> 8);
		cmdbuf [2] = (columnAddr & 0xFF);
	}else if (!(_w25nxx.flash_SR.SR2 & SR2_S3_BUF)){
		cmdbuf [1] = W25N_DUMMY_BYTE;
		cmdbuf [2] = W25N_DUMMY_BYTE;
	}

	W25nxx_WaitForReady();

	W25N_SELECT;
	if(W25N_WriteSpi((uint8_t*)cmdbuf, 4)!= W25_OK){
		W25N_DESELECT;
		return status;
	}

	else{
		if(W25N_ReadSpi(recieve_data, lenght) != W25_OK){
		//if(W25N_ReadSpiDMA(recieve_data, lenght) != W25_OK){
			W25N_DESELECT;
			return status;
		}
	}
	W25N_DESELECT;

	return W25_OK;
}
/**************************************************************************************/

/*All Blocks Erase*/
PUBLIC W25_RESULT_t W25nxx_ChipBlockErase(uint16_t pageAddr){
	//Pagecount = 65536

	if(W25nxx_BlockErase(pageAddr)){
			return W25_ERROR;
	}
	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_ChipErase(void){
	//Pagecount = 65536
#if(_W25NXX_DEBUG == 1)
	printf("W25nxx_ChipErase Started...\r\n");
	W25N_Delayms(10);
#endif

	for(uint32_t i = 0; i < (_w25nxx.PageCount-1); i+=W25_NUM_OF_PAGE_IN_BLOCK){
		if(W25nxx_BlockErase(i))
			return W25_ERROR;
	}


#if(_W25NXX_DEBUG == 1)
	printf("w25nxx EraseBlock done %lu\r\n",_w25nxx.PageCount);
	W25N_Delayms(10);
#endif

	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_PageErase(uint16_t pageno){
	uint8_t buff[2048] = {0xFF};
	return W25nxx_WritePage(pageno, buff, 0, 0);
}
/**************************************************************************************/
PUBLIC W25_RESULT_t W25nxx_WriteData(uint16_t columnAddr, uint32_t pageAddr, uint8_t* dataArr, uint32_t lenght){
	uint8_t status;

	if((status = W25nxx_LoadProgOrRandomLoadData(columnAddr, dataArr, lenght, Load_Prog)) != W25_OK)
		return W25_ERROR;

	if((status = W25nxx_ProgramExecute(pageAddr)) != W25_OK)
		return W25_ERROR;

	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_Write(uint32_t sector, uint8_t* dataArr, uint32_t lenght){
	if(sector > _w25nxx.SectorCount-1)
		return W25_ERROR;

	uint32_t pageAddr = W25nxx_SectorToPage(sector);
	uint16_t columnAdrr;

	if(sector % 4 == 0)
		columnAdrr = SECTOR0_COL_ADDRESS;
	else if (sector % 4 == 1)
		columnAdrr = SECTOR1_COL_ADDRESS;
	else if (sector % 4 == 2)
		columnAdrr = SECTOR2_COL_ADDRESS;
	else if (sector % 4 == 3)
		columnAdrr = SECTOR3_COL_ADDRESS;

	W25nxx_WriteData(columnAdrr, pageAddr, dataArr, lenght);

	return W25_OK;
}

/*You may need to check Pagesize*/
PUBLIC W25_RESULT_t W25nxx_WriteSector(uint32_t sectorNo, uint8_t* dataArr){
	//Total 128MB = 2621444 sector each of 512B
	while(_w25nxx.Lock == 1)
		W25N_Delayms(1);
	_w25nxx.Lock = 1;

	uint8_t status;

	status  = W25nxx_Write(sectorNo, dataArr, _w25nxx.SectorSize);

	W25N_Delayms(1);
	_w25nxx.Lock = 0;

	return status;
}

PUBLIC W25_RESULT_t W25nxx_WritePage(uint32_t pageAddr,  uint8_t* dataArr, uint32_t offset, uint32_t NByteWrtup2PageSiz){
	while(_w25nxx.Lock == 1)
		W25N_Delayms(1);
	_w25nxx.Lock = 1;

#if(_W25NXX_DEBUG == 1)
	printf("W25N WritePage: %lu, Offset:%lu, Write %lu Bytes,begin \r\n", pageAddr, offset, NByteWrtup2PageSiz);
	W25N_Delayms(10);
	uint32_t StartTime = W25N_GetTick;
#endif

	W25nxx_WriteData(SECTOR0_COL_ADDRESS, pageAddr, dataArr, NByteWrtup2PageSiz);


#if(_W25NXX_DEBUG == 1)
	StartTime = W25N_GetTick - StartTime;
	printf("W25N WritePage done after %d ms\r\n", (int)StartTime);
	W25N_Delayms(10);
#endif
	W25N_Delayms(1);
	_w25nxx.Lock = 0;

	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_WriteSector2(uint32_t sector, const uint8_t* dataArr, uint32_t offset, uint32_t NByteWrtup2SecSiz){
	if((NByteWrtup2SecSiz > _w25nxx.SectorSize) || (NByteWrtup2SecSiz == 0)){
		NByteWrtup2SecSiz = _w25nxx.SectorSize;
#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteSector: %lu, Offset:%lu, Write %lu Bytes,begin \r\n", sector, offset, NByteWrtup2SecSiz);
		W25N_Delayms(10);
#endif
	}

	if(offset >=_w25nxx.SectorSize){
#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteSector Faild!\r\n");
		W25N_Delayms(10);
#endif
		return W25_ERROR;
	}

	uint32_t StartPage;
	int32_t Bytes2Write;
	uint32_t localoffset;

	if((offset + NByteWrtup2SecSiz) > _w25nxx.SectorSize)
		Bytes2Write = _w25nxx.SectorSize - offset;
	else
		Bytes2Write = NByteWrtup2SecSiz;

	StartPage = W25nxx_SectorToPage(sector) + (offset/_w25nxx.PageSize);
	localoffset = offset % _w25nxx.PageSize;


	do{
		W25nxx_WriteData(0, StartPage+ localoffset,(uint8_t*) dataArr, Bytes2Write);
		StartPage++;
		Bytes2Write-=_w25nxx.PageSize-localoffset;
		dataArr += _w25nxx.PageSize-localoffset;
		localoffset = 0;

	}while(Bytes2Write > 0);

#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteBlockDone\r\n");
		W25N_Delayms(10);
#endif

		return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_WriteBlock(uint32_t block,  uint8_t* dataArr, uint32_t offset, uint32_t NByteWrtup2BlokSiz){
	if((NByteWrtup2BlokSiz > _w25nxx.BlockSize) || (NByteWrtup2BlokSiz == 0)){
		NByteWrtup2BlokSiz = _w25nxx.BlockSize;
#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteBlock: %lu, Offset:%lu, Write %lu Bytes,begin..\r\n", block, offset, NByteWrtup2BlokSiz);
		W25N_Delayms(10);
#endif
	}

	if(offset >=_w25nxx.BlockSize){
#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteBlock Faild!\r\n");
		W25N_Delayms(10);
#endif
		return W25_ERROR;
	}

	uint32_t StartPage;
	int32_t Bytes2Write;
	uint32_t localoffset;

	if((offset + NByteWrtup2BlokSiz) > _w25nxx.BlockSize)
		Bytes2Write = _w25nxx.BlockSize - offset;
	else
		Bytes2Write = NByteWrtup2BlokSiz;

	StartPage = W25nxx_BlockToPage(block) + (offset/_w25nxx.PageSize);
	localoffset = offset % _w25nxx.PageSize;

	do{
		W25nxx_WritePage(StartPage, dataArr, localoffset, Bytes2Write);
		StartPage++;
		Bytes2Write-=_w25nxx.PageSize-localoffset;
		dataArr += _w25nxx.PageSize-localoffset;
		localoffset = 0;

	}while(Bytes2Write > 0);

#if(_W25NXX_DEBUG == 1)
		printf("W25N WriteSector Done\r\n");
		W25N_Delayms(10);
#endif

		return W25_OK;

}

/***********************************************************************/
PUBLIC W25_RESULT_t W25nxx_FastRead(uint16_t columnAddr,uint32_t pageAddr, uint8_t* data, uint32_t lenght){
	if(pageAddr > (_w25nxx.PageCount-1)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorPage Addr should be between 0 and %d\r\n",(int)_w25nxx.PageCount-1);
#endif
		return W25_ERROR;
	}

	if(columnAddr >= (uint32_t)(_w25nxx.PageSize-1)){
#if(_W25NXX_DEBUG == 1)
		printf("ErrorColumn Addr should be between 0 and %d\r\n",(int)_w25nxx.PageSize-1);
#endif
		return W25_ERROR;
	}

	//lenght should be BUF:1 2112  BUF:0 2048  max
#ifdef _W25NXX_CONTINUOUS_READ
	//Check ECC is enabled be sure enabled
#endif

	uint8_t cmdbuf[5];
	uint8_t status;
	uint8_t rx;
	// W25nxx_Read_SR(Config_reg);  	// ?? _w25nxx.StatusRegister_2

	W25nxx_PageDataRead(pageAddr);

	cmdbuf [0] = FAST_READ;
	cmdbuf [3] = W25N_DUMMY_BYTE;

	if(_w25nxx.flash_SR.SR2 & SR2_S3_BUF){
		cmdbuf [1] = ((columnAddr & 0xFF00) >> 8);
		cmdbuf [2] = (columnAddr & 0xFF);
		W25N_SELECT;
		for(int j = 0; j < array_size(cmdbuf)-1 ; j++)
			W25N_WriteReadSpi(*(cmdbuf+j),&rx, 1);

	}else if (!(_w25nxx.flash_SR.SR2 & SR2_S3_BUF)){
		cmdbuf [1] = W25N_DUMMY_BYTE;
		cmdbuf [2] = W25N_DUMMY_BYTE;
		cmdbuf [4] = W25N_DUMMY_BYTE;
		W25N_SELECT;
		for(int j = 0; j < array_size(cmdbuf); j++)
			W25N_WriteReadSpi(*(cmdbuf+j),&rx, 1);
	}

	if ((status = W25N_ReadSpi(data, lenght)) != W25_OK){
		W25N_DESELECT;
		return status;
	}

	W25N_DESELECT;
	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_ReadByte(uint32_t sector, uint8_t *data, uint32_t lenght){

	if(sector > _w25nxx.SectorCount-1)
		return W25_ERROR;

	uint32_t pageAddr = W25nxx_SectorToPage(sector);
	uint16_t columnAdrr;

	if(sector % 4 == 0)
		columnAdrr = SECTOR0_COL_ADDRESS;
	else if (sector % 4 == 1)
		columnAdrr = SECTOR1_COL_ADDRESS;
	else if (sector % 4 == 2)
		columnAdrr = SECTOR2_COL_ADDRESS;
	else if (sector % 4 == 3)
		columnAdrr = SECTOR3_COL_ADDRESS;

	if(W25nxx_PageDataRead(pageAddr))
		return W25_ERROR;

	if (W25nxx_ReadData(columnAdrr, data, lenght))		// Check this lenght+1
		return W25_ERROR;

	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_ReadArray(uint16_t columnAddr, uint32_t pageAddr, uint8_t *data, uint32_t lenght){

	if((W25nxx_PageDataRead(pageAddr)))
		return W25_ERROR;

	if (W25nxx_ReadData(columnAddr, data, lenght)) 		// Check this lenght+1
		return W25_ERROR;

	return W25_OK;
}

PUBLIC W25_RESULT_t W25nxx_ReadSector512(uint32_t sectorNo, uint8_t* data){
	return W25nxx_ReadByte(sectorNo, data, _w25nxx.SectorSize);
}

PUBLIC W25_RESULT_t W25nxx_ReadPage2048(uint32_t pageAddr, uint8_t *data){
	return W25nxx_ReadArray((uint16_t)SECTOR0_COL_ADDRESS, pageAddr, data, _w25nxx.PageSize);
}


PUBLIC W25_RESULT_t W25nxx_FastReadSector(uint32_t sectorNo, uint8_t* data){
	//Total 128MB = 2621444 sector each of 512B
	if(sectorNo > _w25nxx.SectorCount-1)
		return W25_ERROR;

	uint16_t ColumnAdrres;

	if(sectorNo % 4 == 0){
		ColumnAdrres = SECTOR0_COL_ADDRESS;
	}else if (sectorNo % 4 == 1){
		ColumnAdrres = SECTOR1_COL_ADDRESS;
	}else if (sectorNo % 4 == 2){
		ColumnAdrres = SECTOR2_COL_ADDRESS;
	}else if (sectorNo % 4 == 3){
		ColumnAdrres = SECTOR3_COL_ADDRESS;
	}

	return W25nxx_FastRead(ColumnAdrres, W25nxx_SectorToPage(sectorNo), data, _w25nxx.SectorSize);
}

PUBLIC void W25nxx_DisplayData(uint8_t* buff, uint32_t buffsize){

	for(int i = 0; i< buffsize; i++){
#if(_W25NXX_DEBUG == 1)
		if(!i){
			printf("0x%08X ", 0);
		}
		if(i){
			printf(" ");
			if(!(i % 16))
				printf(" \r\n0x%08X ", i);
		}
		printf("0x%02X", *(uint8_t*)(buff + i));
#endif
	}
	printf("\r\n");

}

/******************************************************************/

PUBLIC uint32_t W25nxx_ReadID(void){
	uint8_t temp[3] = {0x00, 0x00 ,0x00};
	uint8_t tx[2] = {READ_JEDEC_ID, W25N_DUMMY_BYTE};
	uint8_t rx = 0;
	W25N_SELECT;
	W25N_WriteReadSpi(tx[0], &rx, 1);
	W25N_WriteReadSpi(tx[1], &rx, 1);
	W25N_WriteReadSpi(tx[1], temp, 1);
	W25N_WriteReadSpi(tx[1], temp+1, 1);
	W25N_WriteReadSpi(tx[1], temp+2, 1);
	W25N_DESELECT;
	return ((temp[0]<<16) | (temp[1]<<8)  | (temp[2]));
}

W25_RESULT_t W25nxx_Init(void){
	_w25nxx.Lock = 1;
	W25N_SPI_INIT();
	W25N_TIMER_INIT();

#if(_W25NXX_DEBUG == 1)
	W25N_DEBUG_UART_INIT();
	W25N_DEBUG_TARGET(&huart3);
#endif
	W25N_TIMER_START(&htim1);

	W25nxx_WriteDisable();  //??
	W25nxx_DeviceReset();

	while(W25N_GetTick < 100)
		W25N_Delayus(500);     //W25N_Delayms(1);

	W25N_Delayms(100);

	_w25nxx.flashID.JECEDID =W25nxx_ReadID();
	_w25nxx.flashID.ManufacID = ((_w25nxx.flashID.JECEDID >> 16) & 0xFF);
	_w25nxx.flashID.PysicalID  = (_w25nxx.flashID.JECEDID & 0xFFFF);

	switch (_w25nxx.flashID.PysicalID) {
		case DEVICE_ID:
			W25nxx_Write_SR(Prot_Reg, 0x00);      //Allow block erase , page prog   skip potection.
#ifdef _W25NXX_CONTINUOUS_READ
		//	W25nxx_Write_SR(Config_reg, 0x10);  //Default Val :0x18  load 0x10 for BUF = 0; 													 //BUFF = 0 Continuous Read 2048, //BUFF = 1 Continuous Read 2048+64
#endif
			W25nxx_CheckStatusRegisters();
			_w25nxx.flashID.DeviceID = W25N01GV;
			_w25nxx.flashID.DeviceName = "W25N01GV_(128Mb)\0";
			_w25nxx.BlockCount = W25_NUM_OF_BLOCK;
			break;
		default:
			printf(">SYS:NoDevice or UnknownDevice\r\n");
			return W25_ERROR;
	}
	_w25nxx.PageSize 	= W25_PAGE_SIZE1;			//2048+64		//1024block x 64pages
	_w25nxx.SectorSize 	= W25_NUM_OF_A_SECTOR_SIZE;							//1 page = 2048byte (512kB x 4sector) + Spare 64 byte(16b x 4spare)
	_w25nxx.PageSectorCount = (_w25nxx.PageSize/_w25nxx.SectorSize);
	_w25nxx.SectorCount = ((_w25nxx.BlockCount * W25_NUM_OF_SECTOR_IN_PAGE) * W25_NUM_OF_PAGE_IN_BLOCK) ;
	_w25nxx.PageCount	= ((_w25nxx.SectorCount *_w25nxx.SectorSize) / _w25nxx.PageSize);
	_w25nxx.BlockSize 	= ((_w25nxx.SectorSize * W25_NUM_OF_SECTOR_IN_PAGE) * W25_NUM_OF_PAGE_IN_BLOCK);
	_w25nxx.CapasityMB 	= ((_w25nxx.SectorCount * _w25nxx.SectorSize)/(1024*1024));


#if(_W25NXX_DEBUG == 1)
	printf("**********************************************************\r\n");
	printf(">W25Nxx: Device : %s\r\n" , _w25nxx.flashID.DeviceName);
	printf(">W25Nxx: FlashID: 0x%04X\r\n", (int)_w25nxx.flashID.PysicalID);
	printf(">W25Nxx: Page Size: %9d\r\n", _w25nxx.PageSize);
	printf(">W25Nxx: Page Count: %8ld\r\n", _w25nxx.PageCount);
	printf(">W25Nxx: Sector Size: %7ld\r\n", _w25nxx.SectorSize);
	printf(">W25Nxx: Sector Count: %ld\r\n", _w25nxx.SectorCount);
	printf(">W25Nxx: Block Size: %8ld\r\n", _w25nxx.BlockSize);
	printf(">W25Nxx: Block Count: %7ld\r\n", _w25nxx.BlockCount);
	printf(">W25Nxx: ProtectReg: 0x%02X\r\n", _w25nxx.flash_SR.SR1);
	printf(">W25Nxx: Config.Reg: 0x%02X\r\n", _w25nxx.flash_SR.SR2);
	printf(">W25Nxx: StatusReg : 0x%02X\r\n", _w25nxx.flash_SR.SR3);
	printf("**********************************************************\r\n");
#endif
	_w25nxx.Lock = 0;
	return W25_OK;
}





//void W25nxx_FastRead4ByteAddr(void);
//void W25nxx_FastReadDualOutput(void);
//void W25nxx_FastReadDualOutput4ByteAddr(void);
//void W25nxx_FastReadQuadOutput(void);
//void W25nxx_FastReadQuadOutput4ByteAddr(void);
//void W25nxx_FastReadDualIO(void);
//void W25nxx_FastReadDualIO4ByteAddr(void);
//void W25nxx_FastReadQuadIO(void);
//void W25nxx_FastReadQuadIO4ByteAddr(void);






/**********************************************************
* project_name : W25N01GV_test
* file_name    : w25n01gv.h
*
* date         : Sep 21, 2022
* file_path    : /W25N01GV_test/Core/Inc/w25n01gv.h
*
* Autor        : Eng.Emre ÖZTOKLU
***********************************************************/
#ifndef INC_W25NXX_H_
#define INC_W25NXX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************  INCLUDES   *******************/
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "main.h"
#include "w25n_Conf.h"
#include "retarget.h"

/*******************    DEFINES   *******************/
#define W25N_SPI_INIT				MX_SPI1_Init
#define W25N_TIMER_INIT				MX_TIM1_Init
#define W25N_DEBUG_UART_INIT		MX_USART3_UART_Init
#define W25N_DEBUG_TARGET			RetargetInit
#define W25N_TIMER_START			HAL_TIM_Base_Start

#define W25N_DUMMY_BYTE 			0xA5		//A5 ????
#define W25N_Delayms(delayms)		HAL_Delay((uint32_t)delayms)
#define W25N_Delayus(delayus)		delay_us((uint16_t)delayus)
#define W25N_GetTick				HAL_GetTick()

#define W25N_SELECT					HAL_GPIO_WritePin(_W25NXX_CS_PORT, _W25NXX_CS_PIN, GPIO_PIN_RESET)
#define W25N_DESELECT				HAL_GPIO_WritePin(_W25NXX_CS_PORT, _W25NXX_CS_PIN, GPIO_PIN_SET)
/*****************************************************/
#define W25N01GV_SELECTED			(1U)
#define W25N01_CHIPID				(0xEFAA21)
#define MANUFACTURER_ID 			(0XEF)
#define DEVICE_ID					(0xAA21)

#if (W25N01GV_SELECTED == 1)
#define W25_NUM_OF_BLOCK		  		(1024U)
#define W25_NUM_OF_PAGE_IN_BLOCK	  	(64U)
#define W25_PAGE_COUNT					(W25_NUM_OF_PAGE_IN_BLOCK * W25_NUM_OF_BLOCK)  //65536

#define W25_NUM_OF_SECTOR_IN_PAGE 		(4U)
#define W25_NUM_OF_A_SECTOR_SIZE 		(512U)
#define W25_PAGE_MEM_SIZE				(W25_NUM_OF_SECTOR_IN_PAGE * W25_NUM_OF_A_SECTOR_SIZE ) //2048

#define W25_SPARE_NUM_SECTOR_IN_PAGE 	(4U)
#define W25_SPARE_NUM_A_SECTOR_SIZE	 	(16U)
#define W25_PAGE_SPARE_SIZE				(W25_SPARE_NUM_SECTOR_IN_PAGE * W25_SPARE_NUM_A_SECTOR_SIZE) //64

#define W25_PAGE_SIZE1					(W25_PAGE_MEM_SIZE)							//BUF:0 2048
#define W25_PAGE_SIZE2					(W25_PAGE_MEM_SIZE + W25_PAGE_SPARE_SIZE)	//BUF:1 2112

#endif
/**********************************************/
#define DEVICE_RESET			(0XFF)
#define READ_JEDEC_ID			(0X9F)
/**********************************************/
#define SR1_PROT_REG			(0xA0)	//Protection Register
#define SR2_CONFIG_REG			(0xB0)	//Configuration Register
#define SR3_STAT_REG			(0xC0)	//Status Only

//Protection Register
#define SR1_S0_SRP1				(0x01)
#define SR1_S1_WPE				(0x02)
#define SR1_S2_TP				(0x04)
#define SR1_S3_BP0				(0x08)
#define SR1_S4_BP1				(0x10)
#define SR1_S5_BP2				(0x20)
#define SR1_S6_BP3				(0x40)
#define SR1_S7_SRP0				(0x80)
//Configuration Register
#define SR2_S3_BUF				(0x08)
#define SR2_S4_ECCE				(0x10)
#define SR2_S5_SR1L				(0x20)
#define SR2_S6_OTPE				(0x40)
#define SR2_S7_OTPL				(0x80)
//Status Only
#define SR3_S0_BUSY				(0x01)
#define SR3_S1_WEL				(0x02)
#define SR3_S2_EFAIL			(0x04)
#define SR3_S3_PFAIL			(0x08)
#define SR3_S4_ECC0				(0x10)
#define SR3_S5_ECC1				(0x20)
#define SR3_S6_LUTF				(0x40)
/**********************************************/
#define WRITE_ENABLE					(0X06)
#define WRITE_DISABLE					(0X04)

#define READ_STATUS_REGISTER			(0X05)
#define WRITE_STATUS_REGISTER			(0X01)

#define BLOCK_ERASE_128KB				(0XD8)

#define LOAD_PROGRAM_DATA				(0X02)
#define RANDOM_LOAD_PROGRAM_DATA		(0X84)
#define PROGRAM_EXECUTE					(0X10)

#define READ_DATA						(0X03)
#define PAGE_DATA_READ					(0X13)

#define QUAD_LOAD_PROGRAM_DATA			(0X32)
#define QUAD_RANDOM_LOAD_PROGRAM_DATA	(0X34

#define FAST_READ									(0X0B)
#define FAST_READ_WITH_4BYTE_ADDRESS				(0X0C)

#define FAST_READ_DUAL_OUTPUT						(0X3B)
#define FAST_READ_DUAL_OUTPUT_WITH_4BYTE_ADDRESS	(0X3C)
#define FAST_READ_DUAL_IO							(0XBB)
#define FAST_READ_DUAL_IO_WITH_4BYTE_ADDRESS		(0XBC)

#define FAST_READ_QUAD_OUTPUT						(0X6B)
#define FAST_READ_QUAD_OUTPUT_WITH_4BYTE_ADDRESS	(0X6C)
#define FAST_READ_QUAD_IO							(0XEB)
#define FAST_READ_QUAD_IO_WITH_4BYTE_ADDRESS		(0XEC)


#define BAD_BLOCK_MANAGEMENT			(0XA1)
#define READ_BBM_LOOK_UP_TABLE			(0XA5)
#define LAST_ECC_FAILURE_PAGE_ADDRESS	(0XA9)
/***************************************(**************/
#define SECTOR0_COL_ADDRESS		(0x0000)
#define SECTOR1_COL_ADDRESS		(0x0200)
#define SECTOR2_COL_ADDRESS		(0x0400)
#define SECTOR3_COL_ADDRESS		(0x0600)
#define SECTORS_COL_SIZE		(512)

#define SPARE0_COL_ADDRESS		(0x0800)
#define SPARE1_COL_ADDRESS		(0x0810)
#define SPARE2_COL_ADDRESS		(0x0820)
#define SPARE3_COL_ADDRESS		(0x0830)
#define SPARES_COL_SIZE			(16)


/*******************    TYPEDEF   *******************/

typedef enum {
	W25N01GV = 1,
	W25M02GV,
}W25NXX_ID_t;

typedef enum {
	Prot_Reg = 1,
	Config_reg,
	Stat_reg,
}Status_Reg_t;

typedef enum{
	Load_Prog,
	RandomLoad_Prog,
}LoadRandLoad_t;

/* Results of Disk Functions */
typedef enum {
	W25_OK = 0,			/* 0: Successful */
	W25_ERROR,			/* 1: R/W Error */
	//W25_WRPRT,		/* 2: Write Protected */
	//W25_NOTRDY,		/* 3: Not Ready */
	//W25_PARERR		/* 4: Invalid Parameter */
} W25_RESULT_t;

typedef struct {
	W25NXX_ID_t DeviceID;
	char *DeviceName;
	uint32_t JECEDID;
	uint8_t ManufacID;
	uint16_t PysicalID;
}DeviceID_t;

typedef struct {
	uint8_t SR1;
	uint8_t SR2;
	uint8_t SR3;
}StatReg_t;

typedef struct {

	DeviceID_t flashID;
	StatReg_t flash_SR;

	uint16_t PageSize;
	uint32_t PageCount;
	uint8_t  PageSectorCount;
	uint32_t SectorSize;
	uint32_t SectorCount;
	uint32_t BlockSize;
	uint32_t BlockCount;
	uint32_t CapasityMB;

	uint8_t Lock;

}w25nxx_t;


/*******************    MACROS    *******************/
#define array_size(x)			(sizeof(x)/sizeof(x[0]))
/*******************  DATA_TYPES  *******************/

extern w25nxx_t _w25nxx;
extern SPI_HandleTypeDef _W25NXX_SPI;

/*************** FUNCTION_PROTOTYPES  *******************/
uint32_t W25nxx_PageToSector(uint32_t pageAddr);
uint32_t W25nxx_PageToBlock(uint32_t pageAddr);
uint32_t W25nxx_SectorToBlock(uint32_t SectorAddr);
uint32_t W25nxx_SectorToPage(uint32_t SectorAddr);
uint32_t W25nxx_BlockToPage(uint32_t BlockAddr);
/*********************************************************/
uint8_t W25nxx_Read_SR(uint8_t noSR);
void W25nxx_CheckStatusRegisters(void);
void W25nxx_DeviceReset(void);
uint32_t W25nxx_ReadID(void);

/*Delete functions*/
W25_RESULT_t W25nxx_ChipBlockErase(uint16_t pageAddr);
W25_RESULT_t W25nxx_ChipErase(void);				//Bulk-Erase
W25_RESULT_t W25nxx_PageErase(uint16_t pageno);
/*write functions*/
W25_RESULT_t W25nxx_WriteData(uint16_t columnAddr, uint32_t pageAddr, uint8_t* dataArr, uint32_t lenght);
W25_RESULT_t W25nxx_WriteSector(uint32_t sector,  uint8_t* dataArr);
W25_RESULT_t W25nxx_WritePage(uint32_t pageAddr,  uint8_t* dataArr, uint32_t offset, uint32_t NByteWrtup2PageSiz);
W25_RESULT_t W25nxx_WriteBlock(uint32_t block,  uint8_t* dataArr, uint32_t offset, uint32_t NByteWrtup2BlokSiz);


/*Read functions*/
W25_RESULT_t W25nxx_ReadByte(uint32_t sector, uint8_t *data, uint32_t lenght);
W25_RESULT_t W25nxx_ReadArray(uint16_t columnAddr, uint32_t pageAddr, uint8_t *data, uint32_t lenght);
W25_RESULT_t W25nxx_ReadSector512(uint32_t sectorNo, uint8_t* data);
W25_RESULT_t W25nxx_ReadPage2048(uint32_t pageNo, uint8_t* data);


//W25_RESULT_t W25nxx_FastRead(uint16_t columnAddr,uint32_t pageAddr, uint8_t* data, uint32_t lenght);
//W25_RESULT_t W25nxx_FastReadArray(uint16_t columnAddr, uint32_t pageAddr, uint8_t *data, uint32_t lenght);
//W25_RESULT_t W25nxx_FastReadSector(uint32_t sectorNo, uint8_t* data);


void W25nxx_DisplayData(uint8_t* buff, uint32_t buffsize);
W25_RESULT_t W25nxx_Init(void);



/*
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
*/

/*
void W25nxx_FastRead(void);
void W25nxx_FastRead4ByteAddr(void);
void W25nxx_FastReadDualOutput(void);
void W25nxx_FastReadDualOutput4ByteAddr(void);
void W25nxx_FastReadQuadOutput(void);
void W25nxx_FastReadQuadOutput4ByteAddr(void);
void W25nxx_FastReadDualIO(void);
void W25nxx_FastReadDualIO4ByteAddr(void);
void W25nxx_FastReadQuadIO(void);
void W25nxx_FastReadQuadIO4ByteAddr(void);
*/


#ifdef __cplusplus
}
#endif

#endif /* INC_W25NXX_H_ */

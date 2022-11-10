/*
 * stm32fxx_user_is25lp128f.h
 *
 *  Created on: 23 Tem 2020
 *      Author: arge1
 */

#ifndef USER_DRIVERS_INC_IS25LP128F_H_
#define USER_DRIVERS_INC_IS25LP128F_H_

#include "stm32f405xx.h"
#include "main.h"

/*MACROS*/


#define MAX_PAGE_WR_SIZE 			255
#define MAX_SECTOR_ERASE_TIME 		300
#define MAX_32KBLOCK_ERASE_TIME 	500
#define MAX_64KBLOCK_ERASE_TIME 	1000
#define MAX_CHIP_ERASE_TIME			9000
#define TIMEOUT 100
/*STANDARD BITS*/

/*STATUS REGISTER*/
#define WIP 	BIT0
#define WEL 	BIT1
#define BP0 	BIT2
#define BP1		BIT3
#define BP2 	BIT4
#define BP3 	BIT5
#define QE		BIT6
#define SRWD 	BIT7
/*FUNCTION REGISTER*/
#define RST		BIT0
#define TBS		BIT1
#define PSUS	BIT2
#define ESUS	BIT3
#define IRL0	BIT4
#define IRL1	BIT5
#define IRL2	BIT6
#define IRL3	BIT7
/*READ REGISTER*/
#define BURST_LENGTH_0 	BIT0
#define BURST_LENGTH_1	BIT1
#define WRAP_ENABLE		BIT2
#define DUMMY_CYCLES_0	BIT3
#define DUMMY_CYCLES_1	BIT4
#define DUMMY_CYCLES_2	BIT5
#define DUMMY_CYCLES_3	BIT6
#define HOLD_RESET		BIT7
/*EXTENDED REGISTER*/
#define WIP				BIT0
#define PROT_E			BIT1
#define P_ERR			BIT2
#define E_ERR			BIT4
#define ODS0			BIT5
#define ODS1			BIT6
#define ODS2			BIT7
/*BANK ADDRESS REGISTER*/
#define EXTADD			BIT7
/*ADVANCED SECTOR/BLOCK PROTECTION REGISTER*/
#define PSTMLB			BIT1
#define PWDMLB			BIT2
#define TBPARM			BITF
/*PASSWORD REGISTER*/
#define RDPWD 			0xE7 //Read Password
#define PGPWD			0xE8 //Write Password
/*PPB LOCK REGISTER*/
#define PPBLK			BIT0
#define FREEZE			BIT7
/*INSTRUCTION SET TABLE*/
#define DUMMY			0xFF
#define _3NRD 			0x03 	//normal read operation Fclk= 80Mhz
//#define _4NRD 		0x13 	//normal read operation Fclk= 80Mhz
//#define _3FRD 		0x0B	//fast read operation Fclk=166Mhz
//#define _4FRD 		0x0C
#define EN4B 			0xB7
#define _4PP			0x12
#define _3PP			0x02
#define RDSR			0x05
#define WRSR			0x01
#define WREN 			0x06 	//write enable
#define WRDI 			0x04	//write disable
#define SER				0x20
#define _4SER			0x21
#define BER32K			0x52
#define _4BER32K		0x5C
#define BER64K			0xD8
#define _4BER64K		0xDC
#define CER				0xC7
#define RDBR			0x16
#define WRBRV			0x17
#define RDID			0xAB
/*DEVICE OPERATION MODE*/
#define NORMAL_READ_MODE
//#define FAST_READ_MODE
#define _3BYTE_ADDRESS_MODE
//#define _4BYTE_ADDRESS_MODE

typedef uint16_t size_t_;
typedef uint16_t pin_t;
typedef uint16_t port_t;
typedef uint16_t state_t;
typedef uint8_t (*Spi_Write)(void* pTxData, size_t_ Size);
typedef uint8_t (*Spi_Read)(void* pRxData, size_t_  Size);
typedef uint8_t	(*Gpio_WriteHigh)(void);
typedef uint8_t	(*Gpio_WriteLow)(void);
typedef uint32_t (*GetSysTick)(void);
typedef state_t (*Gpio_Read)(port_t Port, pin_t Pin);

typedef enum
{
	READY=0,
	BUSY=1
}IS25LP128F_Status;
 typedef struct
{
	uint8_t 		ID;
	uint8_t 		SR;		/*Status Register*/
	uint8_t 		FR;		/*Function Register*/
	uint8_t 		RR;		/*Read Register*/
	uint8_t 		ERR;	/*Extended Read Register*/
	uint32_t 		ABT; 	/*AutoBoot Register*/
	uint8_t 		BAR;	/*Bank Adress Register*/
	Spi_Write		WriteDev;
	Spi_Read		ReadDev;
	Gpio_WriteHigh  StopComm;
	Gpio_WriteLow   StartComm;
	GetSysTick		TickTime;
}IS25LP128F_typedef;

typedef enum
{
	DATA_TYPE_8=0,
	DATA_TYPE_16=1,
	DATA_TYPE_32=2,
}DataTypeDef;

void IS25LP128F_ReadID(void);
ErrorStatus IS25LP128F_WritePage(uint32_t , const void* , uint32_t );
void IS25LP128F_ReadDataFromFlash(uint32_t ,  void *, uint32_t , DataTypeDef );
IS25LP128F_Status IS25LP128F_CheckWriteStatus();
IS25LP128F_Status IS25LP128F_CheckWriteFlag();
#endif /* USER_DRIVERS_INC_IS25LP128F_H_ */

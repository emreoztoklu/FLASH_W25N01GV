/**********************************************************
* project_name : W25N01GV_test
* file_name    : w25n_Conf.h
*
* date         : Sep 21, 2022
* file_path    : /W25N01GV_test/Core/Inc/w25n_Conf.h
*
* Autor        : Eng.Emre ÖZTOKLU
***********************************************************/
#ifndef INC_W25N_CONF_H_
#define INC_W25N_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************  INCLUDES   *******************/
#include "../../../Peripherals/eo_spi.h"
#include "main.h"

/*******************    DEFINES   *******************/
#define _W25NXX_SPI 	hspi1
#define _W25NXX_CS_PORT SP1_CS_GPIO_Port
#define _W25NXX_CS_PIN  SP1_CS_Pin

/*this comment related with printf and UART output */
#define _W25NXX_DEBUG	(1U)

/*
   0U: CONTINUOUS READ (2048) , 1U:BUFFER READ (2048+64)
   default is "1" meand BUFFER READ  if you decomment this line it will work buffer read
   it will take memory and spare area
*/
#define _W25NXX_CONTINUOUS_READ


/*******************    TYPEDEF   *******************/

/*******************    MACROS    *******************/

/*******************  DATA_TYPES  *******************/

/*************** FUNCTION_PROTOTYPES  *******************/



#ifdef __cplusplus
}
#endif

#endif /* INC_W25N_CONF_H_ */

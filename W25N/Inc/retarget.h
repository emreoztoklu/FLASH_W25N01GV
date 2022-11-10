
// All credit to Carmine Noviello for this code
// https://github.com/cnoviello/mastering-stm32/blob/master/nucleo-f030R8/system/include/retarget/retarget.h

#ifndef _RETARGET_H__
#define _RETARGET_H__

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdint.h>
#include "stm32f4xx.h"

void RetargetInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif //#ifndef _RETARGET_H__


// #define	EPERM 1		/* Not owner */
// #define	ENOENT 2	/* No such file or directory */
// #define	ESRCH 3		/* No such process */
// #define	EINTR 4		/* Interrupted system call */
// #define	EIO 5		/* I/O error */
// #define	ENXIO 6		/* No such device or address */
// #define	E2BIG 7		/* Arg list too long */
// #define	ENOEXEC 8	/* Exec format error */

// #define	EBADF 9		/* Bad file number */

// #define	ECHILD 10	/* No children */
// #define	EAGAIN 11	/* No more processes */
// #define	ENOMEM 12	/* Not enough space */
// #define	EACCES 13	/* Permission denied */
// #define	EFAULT 14	/* Bad address */

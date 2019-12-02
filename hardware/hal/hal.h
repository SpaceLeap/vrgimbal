#ifndef _HAL_H_
#define _HAL_H_

//#include <errno.h>

#include "adc.h"
#include "delay.h"
#include "dma.h"
#include "error.h"
#include "exti.h"
#include "flash.h"
#include "gpio.h"
#include "hal_types.h"
#include "i2c.h"
#include "iwdg.h"
#include "pwr.h"
#include "rtc.h"
#include "spi.h"
#include "stm32.h"
#include "systick.h"
#include "timer.h"
#include "usart.h"
#include "util.h"

#define OK		1
#define ERROR	0
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/*
 * Where to put usercode, based on space reserved for bootloader.
 *
 * FIXME this has no business being here
 */
#define USER_ADDR_ROM 0x08005000
#define USER_ADDR_RAM 0x20000C00
#define STACK_TOP     0x20000800

#endif


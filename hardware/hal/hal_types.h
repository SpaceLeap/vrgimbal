#ifndef _HAL_TYPES_H_
#define _HAL_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;

typedef void (*voidFuncPtr)(void);

#define __attr_flash
#define __packed
//#define __attr_flash __attribute__((section(".DataFlash")))
//#define __attr_flash __attribute__((section(".USER_FLASH")))
//#define __packed __attribute__((__packed__))

#ifndef NULL
#define NULL 0
#endif



#endif


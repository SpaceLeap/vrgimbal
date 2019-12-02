/**
 * @file error.h
 * @brief Error handling and debug utilities
 */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <errno.h>

#define errno_r			(*(__errno()))

#ifndef EINVAL
  #define	EINVAL		22	/* Invalid argument */
#endif
#ifndef EBUSY
  #define EBUSY			16	/* Device or resource busy. */
#endif
#ifndef ETIMEDOUT
  #define ETIMEDOUT	    60  /* Connection timed out */ 
#endif
#ifndef EAGAIN
  #define EAGAIN           6 // Resource unavailable, try again.
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
#include "stm32.h"
#include "util.h"
#include "delay.h"

//#define LED_DEBUG_PIN 7
//#define LED_DEBUG_PORT GPIOB
//#define LED_DEBUG_CLK CLK_GPIOE
//#define DEBUG_LEDS

#define debug_printf

static inline void assert_failed(uint8_t* file, uint32_t line)
{
	/* Infinite loop */
	while (1)
	{
#if defined(DEBUG_LEDS)
          /* let's blink the the debug led */
          LED_DEBUG_PORT->ODR ^= BIT(LED_DEBUG_PIN);	
          //gpio_toggle_bit(_GPIOE, 5);
//          LED_DEBUG ^= 1;
          delay_us(200000);
#endif
	}
}


#endif /* USE_FULL_ASSERT */


#ifdef __cplusplus
  }
#endif
 

#endif


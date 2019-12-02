/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Michael Hope.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file iwdg.c
 * @brief Independent watchdog (IWDG) support
 */

#include "iwdg.h"

/**
 * @brief Initialise and start the watchdog
 *
 * The prescaler and reload set the timeout.  For example, a prescaler
 * of IWDG_PRE_32 divides the 40 kHz clock by 32 and gives roughly 1
 * ms per reload.
 *
 * @param prescaler Prescaler for the 40 kHz IWDG clock.
 * @param reload Independent watchdog counter reload value.
 */
void iwdg_init(iwdg_prescaler prescaler, uint16_t reload) {
   IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	
   IWDG_SetPrescaler(prescaler);
   IWDG_SetReload(reload);

   /* Start things off */
   IWDG_Enable();
   iwdg_feed();
}
void iwdg_stop(void) {
   IWDG->KR = IWDG_KR_RESET;

}
/**
 * @brief Reset the IWDG counter.
 *
 * Calling this function will cause the IWDG counter to be reset to
 * its reload value.
 */
void iwdg_feed(void) {
	IWDG_ReloadCounter();
}

/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file flash.c
 * @brief Flash management functions
 */

#include "flash.h"
#include "stm32.h"

/**
 * @brief Turn on the hardware prefetcher.
 */
void flash_enable_prefetch(void) {
	FLASH_PrefetchBufferCmd(ENABLE);
}

/**
 * @brief Set flash wait states
 *
 * See ST PM0042, section 3.1 for restrictions on the acceptable value
 * of wait_states for a given SYSCLK configuration.
 *
 * @param wait_states number of wait states (one of
 *                    FLASH_WAIT_STATE_0, FLASH_WAIT_STATE_1,
 *                    FLASH_WAIT_STATE_2, ..., FLASH_WAIT_STATE_7).
 */
void flash_set_latency(uint32_t wait_states) {
	FLASH_SetLatency(wait_states);
}

/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs, LLC.
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
 * @file stm32.h
 * @brief STM32 chip-specific definitions
 */

#ifndef _STM32_H_
#define _STM32_H_

#define NR_INTERRUPTS STM32_NR_INTERRUPTS

#define STM32_TICKS_PER_US          32
#define STM32_NR_GPIO_PORTS          5
#define STM32_DELAY_US_MULT         12
#define STM32_SRAM_END              ((void*)0x20010000)

#define NR_GPIO_PORTS               STM32_NR_GPIO_PORTS
#define DELAY_US_MULT               STM32_DELAY_US_MULT

#include <stm32f10x.h>
#include <system_stm32f10x.h>

#define STM32_NR_INTERRUPTS 60

#define GPIO_DEFAULT_SPEED GPIO_Speed_50MHz

typedef void (*rcc_clockcmd)(uint32_t, FunctionalState);

#endif  /* _STM32_H_ */

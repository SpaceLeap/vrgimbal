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
 * @brief Generic board initialization routines.
 *
 * By default, we bring up all Maple boards to 72MHz, clocked off the
 * PLL, driven by the 8MHz external crystal. AHB and APB2 are clocked
 * at 72MHz.  APB1 is clocked at 36MHz.
 */

#include "boards.h"
#include <gpio.h>
#include <timer.h>
#include <adc.h>

#include "usb.h"

static void setupFlash(void);
static void setupClocks(void);
static void setupNVIC(void);
static void setupADC(void);
static void setupTimers(void);

void init(void) {
    setupFlash();
    setupClocks();
    setupNVIC();
    systick_init(SYSTICK_RELOAD_VAL);
    gpio_init_all();
    afio_init();
    setupADC();
    setupTimers();
    PWR_init();

    setupUSB();

    boardInit();

}

/**
 * @brief Board reset function.
 *
 */
void boardReset(void)
{
	PWR_reset();
}

static void setupFlash(void) {
    flash_enable_prefetch();
}

/*
 * Clock setup.  Note that some of this only takes effect if we're
 * running bare metal and the bootloader hasn't done it for us
 * already.
 *
 * If you change this function, you MUST change the file-level Doxygen
 * comment above.
 */
static void setupClocks() {
}

static void setupNVIC() {
#ifdef VECT_TAB_FLASH
    NVIC_SetVectorTable(USER_ADDR_ROM, 0);
#elif defined VECT_TAB_RAM
    NVIC_SetVectorTable(USER_ADDR_RAM, 0);
#elif defined VECT_TAB_BASE
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#else
#error "You must select a base address for the vector table."
#endif
    /* Configure the NVIC Preemption Priority Bits 
     * 1 bits for pre-emption priority 3 bits for subpriority 
     */              
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}

static void adcDefaultConfig(const adc_dev* dev);

static void setupADC() {
    adc_foreach(adcDefaultConfig);
}

static void timerDefaultConfig(timer_dev*);

static void setupTimers() {
    timer_foreach(timerDefaultConfig);
}

static void adcDefaultConfig(const adc_dev *dev) {
    adc_init(dev);
    adc_enable(dev);
}

static void timerDefaultConfig(timer_dev *dev) {
    const uint16 full_overflow = 0xFFFF;
    const uint16 half_duty = 0x8FFF;

    timer_reset(dev);
    timer_pause(dev);

    dev->TIMx->CR1 = TIMER_CR1_ARPE;
    dev->TIMx->PSC = 1;
    dev->TIMx->SR = 0;
    dev->TIMx->DIER = 0;
    dev->TIMx->EGR = TIMER_EGR_UG;

    switch (dev->type) {
    case TIMER_ADVANCED:
    	//TIMER_BDTR_MOE equivale a  TIM_CtrlPWMOutputs(dev->regs, ENABLE);
    	dev->TIMx->BDTR = TIMER_BDTR_MOE | TIMER_BDTR_LOCK_OFF;
    case TIMER_GENERAL:
        timer_set_reload(dev, full_overflow);

        for (int channel = 1; channel <= 4; channel++) {
            timer_set_compare(dev, channel, half_duty);
            timer_oc_set_mode(dev, channel, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
        }
    case TIMER_BASIC:
        break;
    }

    timer_resume(dev);
}

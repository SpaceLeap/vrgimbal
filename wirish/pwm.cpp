/**
 *  @brief Arduino-style PWM implementation.
 */

#include "pwm.h"
#include "boards.h"
#include <timer.h>

void pwmWrite(uint8 pin, uint16 duty_cycle) {
    timer_dev *dev = PIN_MAP[pin].timer_device;
    if (pin >= BOARD_NR_GPIO_PINS || dev == NULL || dev->type == TIMER_BASIC) {
		errno_r = ENODEV;
        return;
    }

    timer_set_compare(dev, PIN_MAP[pin].timer_channel, duty_cycle);

//LASER_PWM_PATCH
#if defined (MCU_TYPE_STM32F10x) || (MCU_TYPE_STM32F1xx)
    TIM_ARRPreloadConfig(dev->TIMx, ENABLE);
#endif
//LASER_PWM_PATCH END

	/* TIMx enable counter */    
    TIM_Cmd(dev->TIMx, ENABLE);

    //LASER_PWM_PATCH
#if defined (MCU_TYPE_STM32F10x) || (MCU_TYPE_STM32F1xx)

	if (dev->type == TIMER_ADVANCED)
	{
		TIM_CtrlPWMOutputs(dev->TIMx, ENABLE);
	}
#endif
	//LASER_PWM_PATCH END
}

uint32_t pwmSetTimerFrequency(timer_dev *dev, uint16_t pwm_freq)
{
	uint32_t clock;
	clock = SystemCoreClock;

	uint32_t _pwm_period, pwm_period, pwm_lowest_prescaler;
	_pwm_period = ((clock/pwm_freq)-1);
	pwm_lowest_prescaler = (_pwm_period/0xFFFF);
	pwm_period = ((clock/pwm_freq/(pwm_lowest_prescaler+1))-1);
	   
	/* set timer prescaler for PWM */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
	TIM_TimeBaseStructure.TIM_Prescaler = pwm_lowest_prescaler;
	TIM_TimeBaseStructure.TIM_Period = pwm_period;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(dev->TIMx, &TIM_TimeBaseStructure);
	return pwm_period;	    	
}

uint32_t pwmSetFrequency(uint8_t pin, uint16_t pwm_freq)
{
    timer_dev *dev = PIN_MAP[pin].timer_device;
    if (pin >= BOARD_NR_GPIO_PINS || dev == NULL || dev->type == TIMER_BASIC) {
		errno_r = ENODEV;
        return 0;
    }

	return pwmSetTimerFrequency(dev, pwm_freq);
}

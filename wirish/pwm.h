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
 *  @file pwm.h
 *
 *  @brief Arduino-compatible PWM interface.
 */

#ifndef _PWM_H_
#define _PWM_H_

#include <hal_types.h>

/**
 * As a convenience, analogWrite is an alias of pwmWrite to ease
 * porting Arduino code.  However, period and duty will have to be
 * recalibrated.
 */
#define analogWrite pwmWrite

#ifdef __cplusplus
  extern "C" {
#endif
 
/* in us */
#define MIN_STD_WIDTH 1000
#define MAX_STD_WIDTH 2000
 
/**
 * Set the PWM duty on the given pin.
 *
 * User code is expected to determine and honor the maximum value
 * (based on the configured period).
 *
 * @param pin PWM output pin
 * @param duty_cycle Duty cycle to set.
 */
void pwmWrite(uint8 pin, uint16 duty_cycle);

/**
 * Set the PWM frequency of the timer mapped to the given pin.
 *
 * @param pin PWM output pin
 * @param freq Frequency to set.
 * @return timer period for specified pwm frequency
 */
uint32_t pwmSetFrequency(uint8_t pin, uint16_t freq);

/* internal use only */
static inline long mymap(long value, long fromStart, long fromEnd, long toStart, long toEnd) {
    return (value - fromStart) * (toEnd - toStart) / (fromEnd - fromStart) + toStart;
}

/**
 * Calculate the pwm width given the timer period and frequency and the range to map with.
 *
 * @param value desidered width in us
 * @param period timer period
 * @param freq timer frequency
 * @param min Minumun width
 * @param max Maximum width
 * @return scaled value to use with pwmWrite
 */
static inline uint16_t get_pwm_width(uint16_t value, uint32_t period, uint16_t freq, uint32_t min, uint32_t max)
{
	uint32_t pwm_us = (period * freq) / (1000000.0f / min);
	return  mymap(value, min, max, pwm_us, pwm_us*(max/min)); 
}

/**
 * Calculate the pwm width given the timer period and frequency.
 * The mapping range used is MIN_STD_WIDTH - MAX_STD_WIDTH
 *
 * @param value desidered width in us
 * @param period timer period
 * @param freq timer frequency
 * @return scaled value to use with pwmWrite
 */
static inline uint16_t get_pwm_servo_width(uint16_t value, uint32_t period, uint16_t freq)
{
	return  get_pwm_width(value, period, freq, MIN_STD_WIDTH, MAX_STD_WIDTH);
}

#ifdef __cplusplus
  }
#endif

#endif


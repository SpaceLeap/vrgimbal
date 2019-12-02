#ifndef _RTC_H_
#define _RTC_H_

#include "stm32.h"
#include "hal_types.h"

#define LSE_FREQ 32768 //Hz

#ifdef __cplusplus
extern "C"{
#endif

/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI used as RTC source clock. The RTC Clock may varies due to LSI frequency dispersion. */

uint32_t rtc_enable(void);
void rtc_disable(void);

void rtc_timeAdjust(uint8_t hours, uint8_t minutes, uint8_t seconds);

void rtc_setAlarm(uint32_t AlarmValue);

void attachRTCHandlerCallback(void (*callback)(void));
void attachRTCAlarmCallback(void (*callback)(void));
void attachRTCWakeUpCallback(void (*callback)(void));
void attachRTCDebugCallback(void (*callback)(const char *));

void rtc_calendar_enable(void);
void rtc_calendar_disable(void);
void rtc_wakeup_enable(void);
void rtc_wakeup_disable(void);
void rtc_alarm_enable(void);
void rtc_alarm_disable(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

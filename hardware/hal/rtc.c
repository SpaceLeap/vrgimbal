#include "rtc.h"

__IO uint32_t PeriodValue = 0,  LsiFreq = 0;
__IO uint32_t OperationComplete = 0;
uint16_t tmpCC4[2] = {0, 0};
static void (*rtc_handler_attach_callback)(void);
static void (*rtc_alarm_attach_callback)(void);
static void (*rtc_wakeup_attach_callback)(void);
static void (*rtc_debug_attach_callback)(const char *);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
static uint32_t GetLSIFrequency(void);
#endif

void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear Interrupt pending bit */
		RTC_ClearITPendingBit(RTC_FLAG_SEC);

		if (rtc_handler_attach_callback) {
			rtc_handler_attach_callback();
		}

	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	}
}

void RTCAlarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
		if (rtc_alarm_attach_callback) {
			rtc_alarm_attach_callback();
		}

		/* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);

		/* Check if the Wake-Up flag is set */
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		{
			/* Clear Wake Up flag */
			PWR_ClearFlag(PWR_FLAG_WU);
		}

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Clear RTC Alarm interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
}

void attachRTCHandlerCallback(void (*callback)(void))
{
    rtc_handler_attach_callback = callback;
}

void attachRTCAlarmCallback(void (*callback)(void))
{
    rtc_alarm_attach_callback = callback;
}

void attachRTCWakeUpCallback(void (*callback)(void))
{
    rtc_wakeup_attach_callback = callback;
}

void attachRTCDebugCallback(void (*callback)(const char *))
{
    rtc_debug_attach_callback = callback;
}

void rtc_calendar_enable(void)
{
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

void rtc_calendar_disable(void)
{
	/* Disable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, DISABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

void rtc_wakeup_enable(void)
{
	/* Enable WKUP pin */
	PWR_WakeUpPinCmd(ENABLE);
}

void rtc_wakeup_disable(void)
{
	/* Disable WKUP pin */
	PWR_WakeUpPinCmd(DISABLE);
}

void rtc_alarm_enable(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void rtc_alarm_disable(void)
{
	/* Clear RTC Alarm flag */
	RTC_ClearFlag(RTC_FLAG_ALR);

	/* Disable the RTC Alarm Interrupt */
	RTC_ITConfig(RTC_IT_ALR, DISABLE);

	/* Clear EXTI line17 Interrupt pending bit */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void rtc_disable(void)
{
	rtc_wakeup_disable();
	rtc_alarm_disable();
	rtc_calendar_disable();

	PWR_BackupAccessCmd(ENABLE);

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

	/* Disable the RTC Clock */
	RCC_RTCCLKCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
	/* LSI can not be disabled if the IWDG is running.  */
#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
	RCC_LSEConfig(RCC_LSE_OFF);
#else
	#error Please select the RTC Clock source inside the rtc.h file
#endif

	PWR_BackupAccessCmd(DISABLE);
}

uint32_t rtc_enable(void)
{
	__IO uint32_t freq = 0;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Enable WKUP pin */
	PWR_WakeUpPinCmd(ENABLE);

	/*!< Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/*!< Reset RTC Domain */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
	/* LSI used as RTC source clock */
	/* The RTC Clock may varies due to LSI frequency dispersion. */
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
	/*!< LSE Enable */
	RCC_LSEConfig(RCC_LSE_ON);

	/*!< Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/*!< LCD Clock Source Selection */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
	#error Please select the RTC Clock source inside the rtc.h file
#endif

	/*!< Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/*!< Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
	/* Get the LSI frequency:  TIM10 is used to measure the LSI frequency */
	freq = GetLSIFrequency();
#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
	freq = LSE_FREQ;
#else
	#error Please select the RTC Clock source inside the rtc.h file
#endif

	/* Set the RTC time base to 1s */
	RTC_SetPrescaler(freq);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	return 	freq;
}

void rtc_setAlarm(uint32_t AlarmValue)
{
    RTC_SetAlarm(RTC_GetCounter()+ AlarmValue);
}

void rtc_timeAdjust(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Change the current time */
	RTC_SetCounter(hours*3600 + minutes*60 + seconds);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
static uint32_t GetLSIFrequency(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

	/* Get the Frequency value */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* Enable TIM5 APB1 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Connect internally the TM5_CH4 Input Capture to the LSI clock output */
	GPIO_PinRemapConfig(GPIO_Remap_TIM5CH4_LSI, ENABLE);

	/* TIM5 Time base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/* TIM5 Channel4 Input capture Mode configuration */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	/* Reinitialize the index for the interrupt */
	OperationComplete = 0;

	/* Enable the TIM5 Input Capture counter */
	TIM_Cmd(TIM5, ENABLE);
	/* Reset all TIM5 flags */
	TIM5->SR = 0;
	/* Enable the TIM5 channel 4 */
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);

	/* NVIC configuration */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Wait the TIM5 measuring operation to be completed */
	while (OperationComplete != 2)
	{}

	/* Compute the actual frequency of the LSI. (TIM5_CLK = 2 * PCLK1)  */
	if (PeriodValue != 0)
	{
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
	LsiFreq = (uint32_t)((uint32_t)(RCC_Clocks.PCLK1_Frequency) / (uint32_t)PeriodValue);
#else
	LsiFreq = (uint32_t)((uint32_t)(RCC_Clocks.PCLK1_Frequency * 2) / (uint32_t)PeriodValue);
#endif
	}
	return LsiFreq;
}

void TIM5_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM5, TIM_IT_CC4) == SET)
  {
	OperationComplete++;
    tmpCC4[OperationComplete -1] = (uint16_t)(TIM5->CCR4);

    TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);

    if (OperationComplete >= 2)
    {
      /* Compute the period length */
      PeriodValue = (uint16_t)(tmpCC4[1] - tmpCC4[0] + 1);

      /* Disable the interrupt */
      TIM_ITConfig(TIM5, TIM_IT_CC4, DISABLE);
      TIM_Cmd(TIM5, DISABLE);
    }
  }
}
#endif

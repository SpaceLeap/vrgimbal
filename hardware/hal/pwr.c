#include "pwr.h"

/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	ErrorStatus HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{

		#ifdef STM32F10X_CL
			/* Enable PLL2 */
			RCC_PLL2Cmd(ENABLE);

			/* Wait till PLL2 is ready */
			while(RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);

		#endif

		/* Enable PLL */
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08);
	}
}

bool WakeUpFromStanby(void)
{
  bool ret = false;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Check if the StandBy flag is set */
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
    /* System resumed from STANDBY mode */
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB); 

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
       
    /* set StandbyWakeup indicator*/
    ret = true;

    /* No need to configure the RTC as the RTC config(clock source, enable,
       prescaler,...) are kept after wake-up from STANDBY */    
  } 
  else
  {
    /* Reset StandbyWakeup indicator*/
    ret = false;    
  }   

  /* Clear WakeUp flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Clear RTC Alarm flag */
  RTC_ClearFlag(RTC_FLAG_ALR);
  
  return ret;
}
  
void enterSTANDBYMode(void)
{    
  /* Peripheral Clock - This is done in the init function, but I did it again here */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
   
  /* Enable WKUP pin */
  PWR_WakeUpPinCmd(ENABLE);

  /* Clear RTC Alarm flag */
  RTC_ClearFlag(RTC_FLAG_ALR);

  /* Clear WakeUp flag */
  PWR_ClearFlag(PWR_FLAG_WU);
    
  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
  PWR_EnterSTANDBYMode();
      
}
   
/*******************************************************************************  
* Function Name  : enterSTOPMode  
* Description    : Enters MCU in STOP mode. The wake-up from STOP mode is   
*                  performed by an external interrupt.  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void enterSTOPMode(void)   
{     

    SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;        // systick IRQ off
    
    PWR_ClearFlag(PWR_FLAG_WU);

    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    
    SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;        // systick IRQ on

    /* Configures system clock after wake-up from STOP: 
     * enable HSE, PLL and select PLL as system clock source 
     * (HSE and PLL are disabled in STOP mode) */  
    SYSCLKConfig_STOP();	
}

void PWR_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}


void PWR_reset()
{
	NVIC_SystemReset();
}

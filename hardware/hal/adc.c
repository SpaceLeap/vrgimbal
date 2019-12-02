#include "hal_types.h"
#include "adc.h"

static adc_dev adc1 = {
    .ADCx     = ADC1,
    .clk      = RCC_APB2Periph_ADC1,
    .clkcmd   = RCC_APB2PeriphClockCmd,
};
/** ADC1 device. */
adc_dev* const _ADC1 = &adc1;

#if defined(STM32_HIGH_DENSITY)
adc_dev adc3 = {
    .ADCx     = ADC3,
    .clk      = RCC_APB2Periph_ADC3,
    .clkcmd   = RCC_APB2PeriphClockCmd,
};
/** ADC3 device. */
adc_dev* const _ADC3 = &adc3;
#endif

__IO uint16_t 	ADC_ConvertedValue, T_StartupTimeDelay;
__IO bool adc_data_ready;

/**
 * @brief Call a function on all ADC devices.
 * @param fn Function to call on each ADC device.
 */
void adc_foreach(void (*fn)(const adc_dev*)) 
{
    fn(_ADC1);
#if defined(STM32_HIGH_DENSITY)
    fn(_ADC3);
#endif
}

/**
 * @brief Initialize an ADC peripheral.
 *
 * Initializes the RCC clock line for the given peripheral.  Resets
 * ADC device registers.
 *
 * @param dev ADC peripheral to initialize
 */
void adc_init(const adc_dev *dev) {

	/* Enable The HSI */
	RCC_HSICmd(ENABLE);
	/* Check that HSI oscillator is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	dev->clkcmd(dev->clk, ENABLE);

	ADC_DeInit(dev->ADCx);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* ADCx Init ****************************************************************/
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(dev->ADCx, &ADC_InitStructure);
}

/**
 * @brief Perform a single synchronous software triggered conversion on a
 * channel.
 * @param dev ADC device to use for reading.
 * @param channel channel to convert
 * @return conversion result
 */
uint16_t adc_read(const adc_dev *dev, uint8_t channel)
{
  adc_data_ready = false;
  
#ifdef INTERNAL_VREF
  uint16_t vrefint = vref_read();
  float vref_V = 1.204 * 4095.0 / (float)vrefint;
#endif

  adc_disable(dev);
 
  /* ADC regular channel14 configuration */
  ADC_RegularChannelConfig(dev->ADCx, channel, 1, ADC_SampleTime_28Cycles5);
  adc_enable(dev);
      
  /* Start ADC Software Conversion */
  ADC_SoftwareStartConvCmd(dev->ADCx, ENABLE);
 
  /* Wait until ADC Channel end of conversion */  
  while (ADC_GetFlagStatus(dev->ADCx, ADC_FLAG_EOC) == RESET);
  
  /* Read ADC conversion result */
  uint16_t value = ADC_GetConversionValue(dev->ADCx);

#ifdef INTERNAL_VREF
  value = (uint16_t)((float)value * vref_V / 3.3);
#endif

  return value;
}

#ifdef INTERNAL_TEMPERATURE
uint16_t temp_read(void)
{
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Wait until ADC + Temp sensor start */
  uint16_t T_StartupTimeDelay = 1024;
  while (T_StartupTimeDelay--);
  
  /* Enable TempSensor channel: channel16 */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_28Cycles5);
                               
  /* initialize result */
  uint16_t res = 0;

  /* start ADC convertion by software */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  /* wait until end-of-covertion */
  while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0 );
  /* read ADC convertion result */
  res = ADC_GetConversionValue(ADC1);
	
  /* de-initialize ADC */
  ADC_TempSensorVrefintCmd(DISABLE);
  
  return (res);
}
#endif

#ifdef INTERNAL_VREF
uint16_t vref_read(void)
{
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Wait until ADC + Temp sensor start */
  uint16_t T_StartupTimeDelay = 1024;
  while (T_StartupTimeDelay--);

  /* Enable Vrefint channel: Channel17 */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_28Cycles5);

  /* initialize result */
  uint16_t res = 0;

  /* start ADC convertion by software */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  /* wait until end-of-covertion */
  while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0 );
  /* read ADC convertion result */
  res = ADC_GetConversionValue(ADC1);

  /* de-initialize ADC */
  ADC_TempSensorVrefintCmd(DISABLE);

  return (res);
}
#endif

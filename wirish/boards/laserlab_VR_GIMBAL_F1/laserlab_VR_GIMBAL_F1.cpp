#ifdef BOARD_laserlab_VR_GIMBAL_F1

#include "laserlab_VR_GIMBAL_F1.h"
#include "wirish_types.h"



void boardInit(void) {
	//mi serve la rimappatura parziale del timer3
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
}



extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {
#ifdef TEST_SCAMBIA_PORTA_PWM
	{_GPIOA,   TIMER2,  NULL,  0, 1, ADCx}, // D00/PA0  (ADC12_IN0/TIM2_CH1_ETR)			<--MOT3_CMDA
	{_GPIOA,   TIMER2,  NULL,  1, 2, ADCx}, // D01/PA1  (ADC12_IN1/TIM2_CH2)				<--MOT3_CMDB
	{_GPIOA,   TIMER2,  NULL,  2, 3, ADCx}, // D02/PA2  (ADC12_IN2/USART2_TX/TIM2_CH3)	<--MOT3_CMDC
	{_GPIOA,   NULL,  NULL,  3, 0, ADCx}, // D03/PA3  (ADC12_IN3/USART2_RX/TIM2_CH4)	<--IR_CMD
#else
	{_GPIOA,   NULL,  NULL,  0, 0, ADCx}, // D00/PA0  (ADC12_IN0/TIM2_CH1_ETR)			<--PWM_IN0
	{_GPIOA,   NULL,  NULL,  1, 0, ADCx}, // D01/PA1  (ADC12_IN1/TIM2_CH2)				<--PWM_IN1
	{_GPIOA,   NULL,  NULL,  2, 0, ADCx}, // D02/PA2  (ADC12_IN2/USART2_TX/TIM2_CH3)	<--PWM_IN2
	{_GPIOA,   NULL,  NULL,  3, 0, ADCx}, // D03/PA3  (ADC12_IN3/USART2_RX/TIM2_CH4)	<--PWM_IN3
#endif
	{_GPIOA,   NULL,  _ADC1,  4, 0, 4}, // D04/PA4  (ADC12_IN4)						<--INPUT_ANA1
	{_GPIOA,   NULL,  NULL,  5, 0, ADCx}, // D05/PA5  (ADC12_IN5/ SPI1_SCK)				<--INPUT_ANA2
	{_GPIOA,   NULL,  _ADC1,  6, 0, 6}, // D06/PA6  (ADC12_IN6/ SPI1_MISO/TIM3_CH1)	<--INPUT_ANA3
	{_GPIOA,   NULL,  _ADC1,  7, 0, 7}, // D07/PA7  (ADC12_IN7/ SPI1_MOSI/TIM3_CH2)	<--INPUT_ANA4
    {_GPIOA,   NULL,  NULL,  8, 0, ADCx}, // D08/PA8  (TIM1_CH1/MCO)					<--INPUT_ANA5
    {_GPIOA,   NULL,  NULL,  9, 0, ADCx}, // D09/PA9  (USART1_TX/TIM1_CH2)				-->UART1_TX
    {_GPIOA,   NULL,  NULL, 10, 0, ADCx}, // D10/PA10 (USART1_RX/TIM1_CH3)				<--UART1_RX
    {_GPIOA,   NULL,  NULL, 11, 0, ADCx}, // D11/PA11 (USBDM/TIM1_CH4)					-->USBM
    {_GPIOA,   NULL,  NULL, 12, 0, ADCx}, // D12/PA12 (USBDP/TIM1_ETR)					-->USBP
    {_GPIOA,   NULL,  NULL, 13, 0, ADCx}, // D13/PA13 (JTMS_SWDAT)						<--JTAG_TMS
    {_GPIOA,   NULL,  NULL, 14, 0, ADCx}, // D14/PA14 (JTCK_SWCLK)						<--JTAG_TCK
    {_GPIOA,   NULL,  NULL, 15, 0, ADCx}, // D15/PA15 (TIM2_CH1_ETR/JTDI)
    {_GPIOB,   TIMER3,  NULL,  0, 3, ADCx}, // D16/PB0  (ADC12_IN8/TIM3_CH3)				-->MOT1_CMDB
    {_GPIOB,   TIMER3,  NULL,  1, 4, ADCx}, // D17/PB1  (ADC12_IN9/TIM3_CH4)				-->MOT1_CMDC
    {_GPIOB,   NULL,  NULL,  2, 0, ADCx}, // D18/PB2  (BOOT1)
    {_GPIOB,   NULL,  NULL,  3, 0, ADCx}, // D19/PB3  (JTDO/TIM2_CH2)
    {_GPIOB,   TIMER3,  NULL,  4, 1, ADCx}, // D20/PB4  (JTRST/TIM3_CH1)
    {_GPIOB,   TIMER3,  NULL,  5, 2, ADCx}, // D21/PB5  (TIM3_CH2)						-->MOT1_CMDA
    {_GPIOB,   TIMER4,  NULL,  6, 1, ADCx}, // D22/PB6  (I2C1_SCL/TIM4_CH1)				-->MOT2_CMDA
    {_GPIOB,   TIMER4,  NULL,  7, 2, ADCx}, // D23/PB7  (I2C1_SDA/TIM4_CH2)				-->MOT2_CMDB
    {_GPIOB,   TIMER4,  NULL,  8, 3, ADCx}, // D24/PB8  (I2C1_SCL/CANRX/TIM4_CH3)		-->MOT2_CMDC
    {_GPIOB,   NULL,  NULL,  9, 0, ADCx}, // D25/PB9  (I2C1_SDA/CANTX/TIM4_CH4)			<--INT_IMU
	{_GPIOB,   NULL,  NULL, 10, 0, ADCx}, // D26/PB10 (I2C2_SCL/TIM2_CH3)				-->I2C2_SCL
    {_GPIOB,   NULL,  NULL, 11, 0, ADCx}, // D27/PB11 (I2C2_SDA/TIM2_CH4)				-->I2C2_SDA
    {_GPIOB,   NULL,  NULL, 12, 0, ADCx}, // D28/PB12 (TIM1_BKIN)						-->CS_EEPROM
    {_GPIOB,   NULL,  NULL, 13, 0, ADCx}, // D29/PB13 (SPI2_SCK/TIM1_CH1N)				-->SPI2_SCK
    {_GPIOB,   NULL,  NULL, 14, 0, ADCx}, // D30/PB14 (SPI2_MISO/TIM1_CH2N)				-->SPI2_MISO
    {_GPIOB,   NULL,  NULL, 15, 0, ADCx}, // D31/PB15 (SPI2_MOSI/TIM1_CH3N)				-->SPI2_MOSI
    {_GPIOC,   NULL,  _ADC1,  0, 0, 10}, // D32/PC0  (ADC12_IN10)						<--INPUT_ANA6
    {_GPIOC,   NULL,  _ADC1,  1, 0, 11}, // D33/PC1  (ADC12_IN11)						<--MOT1_ISENSE
    {_GPIOC,   NULL,  _ADC1,  2, 0, 12}, // D34/PC2  (ADC12_IN12)						<--MOT2_ISENSE
    {_GPIOC,   NULL,  _ADC1,  3, 0, 13}, // D35/PC3  (ADC12_IN13)						<--MOT3_ISENSE
    {_GPIOC,   NULL,  NULL,  4, 0, ADCx}, // D36/PC4  (ADC12_IN14)						-->USB_DISC
    {_GPIOC,   NULL,  NULL,  5, 0, ADCx}, // D37/PC5  (ADC12_IN15)
#ifdef TEST_SCAMBIA_PORTA_PWM
    {_GPIOC,   NULL,  NULL,  6, 0, ADCx}, // D38/PC6  (TIM8_CH1)						-->PWM_IN0
    {_GPIOC,   NULL,  NULL,  7, 0, ADCx}, // D39/PC7  (TIM8_CH2)						-->PWM_IN0
    {_GPIOC,   NULL,  NULL,  8, 0, ADCx}, // D40/PC8  (TIM8_CH3/SDIO_D0)				-->PWM_IN0
    {_GPIOC,   NULL,  NULL,  9, 0, ADCx}, // D41/PC9  (TIM8_CH4/SDIO_D1)				-->PWM_IN0

#else
    {_GPIOC,   TIMER8,  NULL,  6, 1, ADCx}, // D38/PC6  (TIM8_CH1)						-->MOT3_CMDA
    {_GPIOC,   TIMER8,  NULL,  7, 2, ADCx}, // D39/PC7  (TIM8_CH2)						-->MOT3_CMDB
    {_GPIOC,   TIMER8,  NULL,  8, 3, ADCx}, // D40/PC8  (TIM8_CH3/SDIO_D0)				-->MOT3_CMDC
    {_GPIOC,   NULL,  NULL,  9, 0, ADCx}, // D41/PC9  (TIM8_CH4/SDIO_D1)				-->IR_CMD

#endif
    {_GPIOC,   NULL,  NULL, 10, 0, ADCx}, // D42/PC10 (UART4_TX/SDIO_D2)				-->MOTOR_EN
    {_GPIOC,   NULL,  NULL, 11, 0, ADCx}, // D43/PC11 (UART4_RX/SDIO_D3)
    {_GPIOC,   NULL,  NULL, 12, 0, ADCx}, // D44/PC12 (UART5_TX/SDIO_CK)
    {_GPIOC,   NULL,  NULL, 13, 0, ADCx}, // D45/PC13 TAMP-RTC
    {_GPIOC,   NULL,  NULL, 14, 0, ADCx}, // D46/PC14 OSC32_IN							-->LED_RED
    {_GPIOC,   NULL,  NULL, 15, 0, ADCx}, // D47/PC15 OSC32_OUT							-->LED_GRE
    {_GPIOD,   NULL,  NULL,  0, 0, ADCx}, // D48/PD0  (OSC_IN/CAN_RX)
    {_GPIOD,   NULL,  NULL,  1, 0, ADCx}, // D49/PD1  (OSC_OUT/CAN_TX)
    {_GPIOD,   NULL,  NULL,  2, 0, ADCx}, // D50/PD2  (UART5_RX/SDIO_CMD)
};


/*
extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {

	{_GPIOA,   NULL,  NULL,  0, 0, ADCx}, // D00/PA0  (ADC12_IN0/TIM2_CH1_ETR)			<--PWM_IN0
	{_GPIOA,   NULL,  NULL,  1, 0, ADCx}, // D01/PA1  (ADC12_IN1/TIM2_CH2)				<--PWM_IN1
	{_GPIOA,   NULL,  NULL,  2, 0, ADCx}, // D02/PA2  (ADC12_IN2/USART2_TX/TIM2_CH3)	<--PWM_IN2
	{_GPIOA,   NULL,  NULL,  3, 0, ADCx}, // D03/PA3  (ADC12_IN3/USART2_RX/TIM2_CH4)	<--PWM_IN3
	{_GPIOA,   NULL,  NULL,  4, 0, ADCx}, // D04/PA4  (ADC12_IN4)						<--INPUT_ANA1
	{_GPIOA,   NULL,  NULL,  5, 0, ADCx}, // D05/PA5  (ADC12_IN5/ SPI1_SCK)				<--INPUT_ANA2
	{_GPIOA,   TIMER3,  NULL,  6, 1, ADCx}, // D06/PA6  (ADC12_IN6/ SPI1_MISO/TIM3_CH1)	<--INPUT_ANA3
	{_GPIOA,   TIMER3,  NULL,  7, 2, ADCx}, // D07/PA7  (ADC12_IN7/ SPI1_MOSI/TIM3_CH2)	<--INPUT_ANA4
    {_GPIOA,   NULL,  NULL,  8, 0, ADCx}, // D08/PA8  (TIM1_CH1/MCO)					<--INPUT_ANA5
    {_GPIOA,   NULL,  NULL,  9, 0, ADCx}, // D09/PA9  (USART1_TX/TIM1_CH2)				-->UART1_TX
    {_GPIOA,   NULL,  NULL, 10, 0, ADCx}, // D10/PA10 (USART1_RX/TIM1_CH3)				<--UART1_RX
    {_GPIOA,   NULL,  NULL, 11, 0, ADCx}, // D11/PA11 (USBDM/TIM1_CH4)					-->USBM
    {_GPIOA,   NULL,  NULL, 12, 0, ADCx}, // D12/PA12 (USBDP/TIM1_ETR)					-->USBP
    {_GPIOA,   NULL,  NULL, 13, 0, ADCx}, // D13/PA13 (JTMS_SWDAT)						<--JTAG_TMS
    {_GPIOA,   NULL,  NULL, 14, 0, ADCx}, // D14/PA14 (JTCK_SWCLK)						<--JTAG_TCK
    {_GPIOA,   NULL,  NULL, 15, 0, ADCx}, // D15/PA15 (TIM2_CH1_ETR/JTDI)
    {_GPIOB,   TIMER3,  NULL,  0, 3, ADCx}, // D16/PB0  (ADC12_IN8/TIM3_CH3)				-->MOT1_CMDB
    {_GPIOB,   TIMER3,  NULL,  1, 4, ADCx}, // D17/PB1  (ADC12_IN9/TIM3_CH4)				-->MOT1_CMDC
    {_GPIOB,   NULL,  NULL,  2, 0, ADCx}, // D18/PB2  (BOOT1)
    {_GPIOB,   NULL,  NULL,  3, 0, ADCx}, // D19/PB3  (JTDO/TIM2_CH2)
    {_GPIOB,   TIMER3,  NULL,  4, 1, ADCx}, // D20/PB4  (JTRST/TIM3_CH1)
    {_GPIOB,   TIMER3,  NULL,  5, 2, ADCx}, // D21/PB5  (TIM3_CH2)						-->MOT1_CMDA
    {_GPIOB,   TIMER4,  NULL,  6, 1, ADCx}, // D22/PB6  (I2C1_SCL/TIM4_CH1)				-->MOT2_CMDA
    {_GPIOB,   TIMER4,  NULL,  7, 2, ADCx}, // D23/PB7  (I2C1_SDA/TIM4_CH2)				-->MOT2_CMDB
    {_GPIOB,   TIMER4,  NULL,  8, 3, ADCx}, // D24/PB8  (I2C1_SCL/CANRX/TIM4_CH3)		-->MOT2_CMDC
    {_GPIOB,   NULL,  NULL,  9, 0, ADCx}, // D25/PB9  (I2C1_SDA/CANTX/TIM4_CH4)			<--INT_IMU
	{_GPIOB,   NULL,  NULL, 10, 0, ADCx}, // D26/PB10 (I2C2_SCL/TIM2_CH3)				-->I2C2_SCL
    {_GPIOB,   NULL,  NULL, 11, 0, ADCx}, // D27/PB11 (I2C2_SDA/TIM2_CH4)				-->I2C2_SDA
    {_GPIOB,   NULL,  NULL, 12, 0, ADCx}, // D28/PB12 (TIM1_BKIN)						-->CS_EEPROM
    {_GPIOB,   NULL,  NULL, 13, 0, ADCx}, // D29/PB13 (SPI2_SCK/TIM1_CH1N)				-->SPI2_SCK
    {_GPIOB,   NULL,  NULL, 14, 0, ADCx}, // D30/PB14 (SPI2_MISO/TIM1_CH2N)				-->SPI2_MISO
    {_GPIOB,   NULL,  NULL, 15, 0, ADCx}, // D31/PB15 (SPI2_MOSI/TIM1_CH3N)				-->SPI2_MOSI
    {_GPIOC,   NULL,  NULL,  0, 0, ADCx}, // D32/PC0  (ADC12_IN10)						<--INPUT_ANA6
    {_GPIOC,   NULL,  NULL,  1, 0, ADCx}, // D33/PC1  (ADC12_IN11)						<--MOT1_ISENSE
    {_GPIOC,   NULL,  NULL,  2, 0, ADCx}, // D34/PC2  (ADC12_IN12)						<--MOT2_ISENSE
    {_GPIOC,   NULL,  NULL,  3, 0, ADCx}, // D35/PC3  (ADC12_IN13)						<--MOT3_ISENSE
    {_GPIOC,   NULL,  NULL,  4, 0, ADCx}, // D36/PC4  (ADC12_IN14)						-->USB_DISC
    {_GPIOC,   NULL,  NULL,  5, 0, ADCx}, // D37/PC5  (ADC12_IN15)
    {_GPIOC,   TIMER3,  NULL,  6, 1, ADCx}, // D38/PC6  (TIM8_CH1)						-->MOT3_CMDA
    {_GPIOC,   TIMER3,  NULL,  7, 2, ADCx}, // D39/PC7  (TIM8_CH2)						-->MOT3_CMDB
    {_GPIOC,   TIMER3,  NULL,  8, 3, ADCx}, // D40/PC8  (TIM8_CH3/SDIO_D0)				-->MOT3_CMDC
    {_GPIOC,   TIMER3,  NULL,  9, 4, ADCx}, // D41/PC9  (TIM8_CH4/SDIO_D1)				-->IR_CMD
    {_GPIOC,   NULL,  NULL, 10, 0, ADCx}, // D42/PC10 (UART4_TX/SDIO_D2)				-->MOTOR_EN
    {_GPIOC,   NULL,  NULL, 11, 0, ADCx}, // D43/PC11 (UART4_RX/SDIO_D3)
    {_GPIOC,   NULL,  NULL, 12, 0, ADCx}, // D44/PC12 (UART5_TX/SDIO_CK)
    {_GPIOC,   NULL,  NULL, 13, 0, ADCx}, // D45/PC13 TAMP-RTC
    {_GPIOC,   NULL,  NULL, 14, 0, ADCx}, // D46/PC14 OSC32_IN							-->LED_RED
    {_GPIOC,   NULL,  NULL, 15, 0, ADCx}, // D47/PC15 OSC32_OUT							-->LED_GRE
    {_GPIOD,   NULL,  NULL,  0, 0, ADCx}, // D48/PD0  (OSC_IN/CAN_RX)
    {_GPIOD,   NULL,  NULL,  1, 0, ADCx}, // D49/PD1  (OSC_OUT/CAN_TX)
    {_GPIOD,   NULL,  NULL,  2, 0, ADCx}, // D50/PD2  (UART5_RX/SDIO_CMD)
};
*/
#endif

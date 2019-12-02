#include "gpio.h"
#include "hal_types.h"
#include "util.h"
		
/*
 * GPIO devices
 */

gpio_dev gpioa = {
    .GPIOx     = GPIOA,
    .clk       = RCC_APB2Periph_GPIOA,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PA,
};
/** GPIO port A device. */
gpio_dev* const _GPIOA = &gpioa;

gpio_dev gpiob = {
    .GPIOx      = GPIOB,
    .clk       = RCC_APB2Periph_GPIOB,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PB,
};
/** GPIO port B device. */
gpio_dev* const _GPIOB = &gpiob;

gpio_dev gpioc = {
    .GPIOx      = GPIOC,
    .clk       = RCC_APB2Periph_GPIOC,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PC,
};
/** GPIO port C device. */
gpio_dev* const _GPIOC = &gpioc;

gpio_dev gpiod = {
    .GPIOx      = GPIOD,
    .clk       = RCC_APB2Periph_GPIOD,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PD,
};
/** GPIO port D device. */
gpio_dev* const _GPIOD = &gpiod;

gpio_dev gpioe = {
    .GPIOx      = GPIOE,
    .clk       = RCC_APB2Periph_GPIOE,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PE,
};
/** GPIO port E device. */
gpio_dev* const _GPIOE = &gpioe;

gpio_dev gpiof = {
    .GPIOx      = GPIOF,
    .clk       = RCC_APB2Periph_GPIOF,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PF,
};
/** GPIO port F device. */
gpio_dev* const _GPIOF = &gpiof;

gpio_dev gpiog = {
    .GPIOx      = GPIOG,
    .clk       = RCC_APB2Periph_GPIOG,
    .clkcmd    = RCC_APB2PeriphClockCmd,
    .exti_port = AFIO_EXTI_PG,
};
/** GPIO port G device. */
gpio_dev* const _GPIOG = &gpiog;

void gpio_init(gpio_dev* dev) 
{
	/* Check the parameters */
	assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
	GPIO_DeInit(dev->GPIOx);
	/* Enable the GPIO Clock  */
	//LASER_PWM_PATCH
	dev->clkcmd(dev->clk | RCC_APB2Periph_AFIO, ENABLE);
}

void gpio_foreach(void (*fn)(gpio_dev*))
{
    fn(_GPIOA);
    fn(_GPIOB);
    fn(_GPIOC);
    fn(_GPIOD);
    fn(_GPIOE);
    fn(_GPIOF);
    fn(_GPIOG);
}

void gpio_init_all(void)
{
	gpio_foreach(gpio_init);
}

gpio_dev * gpio_get_gpio_dev(uint8_t port)
{
	// Check the parameters 
	assert_param(port >= 0 && port <= 7);
	
	gpio_dev *dev;
	  	
	switch(port) {
		case 0: dev = _GPIOA;
				break;
		case 1: dev = _GPIOB;
				break;
		case 2: dev = _GPIOC;
				break;
		case 3: dev = _GPIOD;
				break;
		case 4: dev = _GPIOE;
				break;
		case 5: dev = _GPIOF;
				break;
		case 6: dev = _GPIOG;
				break;
		default:
				assert_param(0);
				errno_r = EINVAL;
				dev = NULL;				
	}		
	return dev;
}


void gpio_set_mode(gpio_dev* dev, uint8_t pin, gpio_pin_mode mode)
{
	/* Check the parameters */
	assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
	assert_param(IS_GPIO_PIN_SOURCE(pin));
	
	GPIO_InitTypeDef config;
	
	/* Enable the GPIO Clock  */
	dev->clkcmd(dev->clk, ENABLE);
  
	/* Configure the pin */
	GPIO_StructInit(&config);
	config.GPIO_Speed = GPIO_DEFAULT_SPEED;
    switch(mode) 
    {
		case GPIO_OUTPUT_PP:
			config.GPIO_Mode = GPIO_Mode_Out_PP;
			break;
		case GPIO_OUTPUT_OD:
			config.GPIO_Mode = GPIO_Mode_Out_OD;
			break;
		case GPIO_INPUT_FLOATING:
			config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			break;
		case GPIO_INPUT_ANALOG:
			config.GPIO_Mode = GPIO_Mode_AIN;
			break;
		case GPIO_INPUT_PU:
			config.GPIO_Mode = GPIO_Mode_IPU;
			break;
		case GPIO_INPUT_PD:
			config.GPIO_Mode = GPIO_Mode_IPD;
			break;
		case GPIO_AF_OUTPUT_PP:
			config.GPIO_Mode = GPIO_Mode_AF_PP;
			break;
		case GPIO_AF_OUTPUT_OD:
			config.GPIO_Mode = GPIO_Mode_AF_OD;
			break;
		default:
			errno_r = EINVAL;
			return;
    }

	config.GPIO_Pin = BIT(pin);
	GPIO_Init(dev->GPIOx, &config);      	
}

void gpio_write_bit(gpio_dev* dev, uint8_t pin, uint8_t val)
{
	/* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
    assert_param(IS_GPIO_PIN_SOURCE(pin));
    
    if (val) 
    {
		dev->GPIOx->BSRR = BIT(pin);
	}
    else
    {
		dev->GPIOx->BRR = BIT(pin);
    }    
}

uint8_t gpio_read_bit(gpio_dev* dev, uint8_t pin)
{
	uint8_t bitstatus = 0x00;

	/* Check the parameters */
	assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
	assert_param(IS_GPIO_PIN_SOURCE(pin));
 
	if ((dev->GPIOx->IDR & BIT(pin)) != (uint32_t)Bit_RESET)
	{
		bitstatus = (uint8_t)Bit_SET;
	}
	else
	{
		bitstatus = (uint8_t)Bit_RESET;
	}
   return bitstatus;

	
}

void gpio_toggle_bit(gpio_dev* dev, uint8_t pin)
{
	/* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
    assert_param(IS_GPIO_PIN_SOURCE(pin));
    dev->GPIOx->ODR ^= BIT(pin);	
}

afio_exti_port gpio_exti_port(gpio_dev* dev)
{
	/* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(dev->GPIOx));
    return dev->exti_port;
}

void afio_exti_select(afio_exti_num exti, afio_exti_port gpio_port)
{
	/* Check the parameters */
	assert_param(IS_GPIO_PIN_SOURCE(exti));
	assert_param(IS_GPIO_EXTI_PORT_SOURCE(gpio_port));

	GPIO_EXTILineConfig(gpio_port, exti);
}


void afio_remap(afio_remap_peripheral remapping) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    if (remapping & AFIO_REMAP_USE_MAPR2) {
        remapping &= ~AFIO_REMAP_USE_MAPR2;
        AFIO->MAPR2 |= remapping;
    } else {
        AFIO->MAPR |= remapping;
    }
}

void afio_cfg_debug_ports(afio_debug_cfg config)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_DEFAULT_SPEED;
					
	switch(config)
	{
		case AFIO_DEBUG_NONE:
			/* Enable GPIOA and GPIOB clocks */
			_GPIOA->clkcmd(_GPIOA->clk, ENABLE);
			_GPIOB->clkcmd(_GPIOB->clk, ENABLE);
			
			/* Configure PA.13 (JTMS/SWDIO), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as output push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);

			/* Configure PB.03 (JTDO) and PB.04 (JTRST) as output push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
			GPIO_Init(GPIOB, &GPIO_InitStructure);			
		
			break;
		case AFIO_DEBUG_SW_ONLY:
			/* Enable GPIOA clocks */
			_GPIOA->clkcmd(_GPIOA->clk, ENABLE);
			
			/* Configure PA.13 (JTMS/SWDIO), PA.14 (JTCK/SWCLK) as output push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);		
			break;
		case AFIO_DEBUG_FULL_SWJ_NO_NJRST:
			/* Enable GPIOB clocks */
			_GPIOB->clkcmd(_GPIOB->clk, ENABLE);
					
			/* Configure PB.04 (JTRST) as output push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOB, &GPIO_InitStructure);				
			break;
		case AFIO_DEBUG_FULL_SWJ:
			break;
		default:
			errno_r = EINVAL;
			return;			
	}
}

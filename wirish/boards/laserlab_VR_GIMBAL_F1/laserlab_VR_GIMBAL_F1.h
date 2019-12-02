#ifndef _BOARD_LASERLAB_VR_GIMBAL_F1_H_
#define _BOARD_LASERLAB_VR_GIMBAL_F1_H_

#include <hal.h>

//#define TEST_SCAMBIA_PORTA_PWM


#define CYCLES_PER_MICROSECOND  (SystemCoreClock/1000000)
#define SYSTICK_RELOAD_VAL      (CYCLES_PER_MICROSECOND*1000-1)

void boardInit(void);

#undef  STM32_PCLK1
#undef  STM32_PCLK2
/* APB1 and APB2 Prescaler are set to 1 */
#define STM32_PCLK1   (SystemCoreClock)
#define STM32_PCLK2   (SystemCoreClock)

/* LEDs ********************************************/
#define BOARD_LED_RED_PIN       46  //PC14
#define BOARD_LED_GRE_PIN       47  //PC15

#ifdef TEST_SCAMBIA_PORTA_PWM
#define BOARD_IR_CMD	        3  //PA3
#else
#define BOARD_IR_CMD	        41  //PC9
#endif

/* MOTORs ********************************************/

#define BOARD_MOTOR_EN	        42  //PC10

#define BOARD_MOT1_CMD_A        21  //PB5
#define BOARD_MOT1_CMD_B        16  //PB0
#define BOARD_MOT1_CMD_C        17  //PB1
#define BOARD_MOT1_ISENSE       33  //PC1

#define BOARD_MOT2_CMD_A        22  //PB6
#define BOARD_MOT2_CMD_B        23  //PB7
#define BOARD_MOT2_CMD_C        24  //PB8
#define BOARD_MOT2_ISENSE       34  //PC2

#ifdef TEST_SCAMBIA_PORTA_PWM

#define BOARD_MOT3_CMD_A        0  //PA0
#define BOARD_MOT3_CMD_B        1  //PA1
#define BOARD_MOT3_CMD_C        2  //PA2
#define BOARD_MOT3_ISENSE       35  //PC3

#else
#define BOARD_MOT3_CMD_A        38  //PC6
#define BOARD_MOT3_CMD_B        39  //PC7
#define BOARD_MOT3_CMD_C        40  //PC8
#define BOARD_MOT3_ISENSE       35  //PC3

#endif

/* USB *********************************************/
#define BOARD_USB_DISC          36  //PC4
#define BOARD_USB_DISC_DEV      _GPIOC
#define BOARD_USB_DISC_BIT      4
#define BOARD_USBM  	    	11  //PA11
#define BOARD_USBP	    	    12  //PA12


/* ANALOG *********************************************/
#define BOARD_ANA1	             4  //PA4
#define BOARD_ANA2  	         5  //PA5
#define BOARD_ANA3      	     6  //PA6
#define BOARD_ANA4          	 7  //PA7
#define BOARD_ANA5          	 8  //PA8
#define BOARD_ANA6          	32  //PC0


/* REMOTE COMMAND INPUT ********************************/
#ifdef TEST_SCAMBIA_PORTA_PWM
#define BOARD_PWM_IN0            38  //PC6
#define BOARD_PWM_IN1  	         39  //PC7
#define BOARD_PWM_IN2      	     40  //PC8
#define BOARD_PWM_IN3          	 41  //PC9

#else

#define BOARD_PWM_IN0            0  //PA0
#define BOARD_PWM_IN1  	         1  //PA1
#define BOARD_PWM_IN2      	     2  //PA2
#define BOARD_PWM_IN3          	 3  //PA3
#endif

/* IMU *********************************************/
#define BOARD_INT_IMU	        25  //PB9
//#define BOARD_INERTIALSENSOR_INT  BOARD_INT_IMU


//IMU SPI
#define BOARD_CS_IMU_SPI  BOARD_ANA1
#define BOARD_INT_IMU_SPI BOARD_ANA5


//vedi config I2C sotto


/* EEPROM *********************************************/
#define BOARD_CS_EEPROM	        28  //PB12

#define BOARD_EEPROM_SIZE		512 //128
//vedi config SPI sotto

/* USARTs *********************************************/
//Serial Port for CLI
#define SERIAL_CLI_PORT			 0
#define SERIAL_CLI_BAUD			115200 //9600

//Serial Port for command port (RS485)
#define SERIAL_CMD_PORT			 1
#define SERIAL_CMD_BAUD			9600

#define BOARD_NR_USARTS          1

#define BOARD_USART1_TX_PIN      9  //PA9
#define BOARD_USART1_RX_PIN     10  //PA10


/* SPIs *****************************************************/
#define BOARD_NR_SPI             2

#define BOARD_SPI1_SCK_PIN       5
#define BOARD_SPI1_MISO_PIN      6
#define BOARD_SPI1_MOSI_PIN      7
#define BOARD_SPI2_SCK_PIN      29
#define BOARD_SPI2_MISO_PIN     30
#define BOARD_SPI2_MOSI_PIN     31

#define BOARD_SPI1_CS_DF_PIN    83
#define BOARD_SPI1_CS_BR_PIN    94

/* I2Cs *****************************************************/
#define BOARD_NR_I2C             2

#define BOARD_I2C1_SCL_PIN      24
#define BOARD_I2C1_SDA_PIN      25
#define BOARD_I2C2_SCL_PIN      26
#define BOARD_I2C2_SDA_PIN      27



/************************************************************/
#define BOARD_NR_GPIO_PINS      51





#endif

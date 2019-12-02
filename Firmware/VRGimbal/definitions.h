/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
    This file is part of VRGimbal by VirtualRobotix Italia s.c.a.r.l..

    VRGimbal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VRGimbal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VRGimbal.  If not, see <http://www.gnu.org/licenses/>.

    Please refer to http://vrgimbal.wordpress.com for more information
*/

/*************************/
/* Definitions           */
/*************************/
#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_


//inserisco qui le definzioni per mantenere la compatibilità
#define PGM_P const void *
#define F(x) x
#define cli dummy //noInterrupts
#define sei dummy //interrupts





#define VERSION_STATUS A // A = Alpha; B = Beta , N = Normal Release
#define VERSION 114
#define VERSION_EEPROM 13


#define MOTOR_COUNT 3
#define PROFILE_COUNT 0

#define ANGLE_PRECISION 1000  //millesimi di grado



// MPU Address Settings
#define MPU6050_ADDRESS_AD0_LOW     0x68 // default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0x69 // Drotek MPU breakout board
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_HIGH


typedef uint16_t pwmsin_t; //uint8_t


#define MOTORUPDATE_FREQ 1000 //500 //100 //1000                 // in Hz, 1000 is default // 1,2,4,8 for 32kHz, 1,2,4 for 4kHz
#define DT_INT_US (1000000/MOTORUPDATE_FREQ)
#define DT_INT_MS (1000/MOTORUPDATE_FREQ)


#ifdef BRUGI_USE_INTERRUPT_TIMER
#define LOOPUPDATE_FACTOR 5
#else
#define LOOPUPDATE_FACTOR 1
#endif

#define LOOPUPDATE_FREQ (MOTORUPDATE_FREQ/LOOPUPDATE_FACTOR)    // loop control sample rate equals motor update rate
#define DT_FLOAT (1.0/LOOPUPDATE_FREQ)      // loop controller sample period dT
#define DT_LOOP_MS (1000/LOOPUPDATE_FREQ)



#define POUT_FREQ 5 // 25     // rate of ACC print output in Hz, 25 Hz is default


#define IDLE_TIME_SEC 2  // gimbal fast lock time at startup
#define LOCK_TIME_SEC 5  // gimbal fast lock time at startup 

// LP filter coefficient
#define LOWPASS_K_FLOAT(TAU) (DT_FLOAT/(TAU+DT_FLOAT))

// Do not change for now
#define MPU6050_GYRO_FS MPU6050_GYRO_FS_250  // +-250,500,1000,2000 deg/s
#define MPU6050_DLPF_BW MPU6050_DLPF_BW_256  // 5,10,20,42,98,188,256 Hz

// Number of sinus values for full 360 deg.
// NOW FIXED TO 256 !!!
// Reason: Fast Motor Routine using uint8_t overflow for stepping
#define N_SIN 1024 // 512 //256
extern int32_t g_pwmSinMotor[N_SIN];

#define STEP_DOWNSCALE 1// 2 //3

#define SCALE_ACC 10000.0
#define SCALE_PID_PARAMS 1000.0f



#define RC_PWM_CHANNELS 4
#define MIN_RC 1000
#define MID_RC 1500
#define MAX_RC 2000
#define RC_DEADBAND 50
#define RC_TIMEOUT 100000


#define RC_DATA_SIZE 	6
#define RC_DATA_ROLL 	0
#define RC_DATA_PITCH 	1
#define RC_DATA_YAW 	2
#define RC_DATA_RESET_ROLL	 	3
#define RC_DATA_RESET_PITCH 	4
#define RC_DATA_RESET_YAW 		5


// PPM Decoder
#define RC_PPM_GUARD_TIME 4000
#define RC_PPM_RX_MAX_CHANNELS 32

// I2C Frequency
//#define I2C_SPEED 100000L     //100kHz normal mode
//#define I2C_SPEED 400000L   //400kHz fast mode
#define I2C_SPEED 800000L   //800kHz ultra fast mode

// Hardware Abstraction for Motor connectors,
// DO NOT CHANGE UNLES YOU KNOW WHAT YOU ARE DOING !!!
#define PWM_A_MOTOR1 OCR2A
#define PWM_B_MOTOR1 OCR1B
#define PWM_C_MOTOR1 OCR1A

#define PWM_A_MOTOR0 OCR0A
#define PWM_B_MOTOR0 OCR0B
#define PWM_C_MOTOR0 OCR2B


#ifdef DEBUG
    #define DEBUG_PRINT(x) cliSerial->print(x)
    #define DEBUG_PRINTF(x, y) cliSerial->print(x, y)
    #define DEBUG_PRINTLN(x) cliSerial->println(x)
    #define DEBUG_PRINTLNF(x, y) cliSerial->println(x, y)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTF(x, y)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTLNF(x, y)
#endif



#define CC_FACTOR 1


#define LEDPIN_PINMODE             pinMode (BOARD_LED_RED_PIN, OUTPUT);
//#define LEDPIN_SWITCH              digitalWrite(BOARD_LED_RED_PIN,!bitRead(PORTB,0));
#define LEDPIN_OFF                 digitalWrite(BOARD_LED_RED_PIN, LOW);
#define LEDPIN_ON                  digitalWrite(BOARD_LED_RED_PIN, HIGH);

#define LEDGREPIN_PINMODE             pinMode (BOARD_LED_GRE_PIN, OUTPUT);
//#define LEDPIN_SWITCH              digitalWrite(BOARD_LED_GRE_PIN,!bitRead(PORTB,0));
#define LEDGREPIN_OFF                 digitalWrite(BOARD_LED_GRE_PIN, LOW);
#define LEDGREPIN_ON                  digitalWrite(BOARD_LED_GRE_PIN, HIGH);

// enable stack and heapsize check (use just for debugging)
//#define STACKHEAPCHECK_ENABLE

#endif //_DEFINITIONS_H_

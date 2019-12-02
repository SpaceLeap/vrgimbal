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

#ifndef _MAIN_H
#define _MAIN_H

#define THISFIRMWARE "VRGimbal 1.14a "

#include <wirish.h>

// Configuration
#include "defines.h"
#include "config.h"

// Local modules

//#include "AP_Common.h"
#include "AP_Math.h"


// Global modules
#ifdef FUN_DATAFLASH
#include <DataFlash.h>                  // ArduPilot Mega Flash Memory Library
#endif
#ifdef FUN_EEPROM
#include <EEPROM.h>
#endif

#ifdef ENABLE_AP_PARAM
#include <AP_Menu.h>
#include "Parameters.h"
#endif

//BruGI
#include "definitions.h"
#include "variables.h"
#include "fastMathRoutines.h"
#include "SerialCommand.h"


/* MAIN *****************************************************************/
#if (defined ( FUN_DATAFLASH)) || (defined (GIMBAL_IMU_SPI))
extern HardwareSPI spi1;
#endif
extern HardwareSPI spi2;
#ifdef FUN_EEPROM
extern HardwareI2C i2c2;
#endif


/* MAIN ************************************************************/
extern void dummy(void);

extern void setup(void);
extern void loop(void);
//extern uint32 getMillis(void);
/************************************************************************/


/* DATAFLASH ************************************************************/
#ifdef FUN_DATAFLASH
extern DataFlash_MP32 DataFlash;
#endif
/************************************************************************/


/* PARAMETERS ***********************************************************/
#ifdef ENABLE_AP_PARAM
#ifdef FUN_EEPROM
extern AP_Param param_loader;
extern Parameters g;

extern void load_parameters(void);
extern void zero_eeprom(void);
#endif
#endif
/************************************************************************/


/* SERIALI **************************************************************/
extern FastSerial SerialDBG;
#ifdef GIMBAL_ENABLE_USB
extern USBSerial SerialUSB;
#endif
extern FastSerial *cliSerial; // = &SerialDBG;

/************************************************************************/


extern SerialCommand sCmd;     // Create SerialCommand object






/* SETUP ****************************************************************/
//extern int8 setup_mode(uint8 argc, const Menu::arg *argv);
/************************************************************************/

/* SYSTEM ***************************************************************/
//#ifdef FUN_EEPROM
//extern void init_eeprom(void);
//#endif
//extern void pinSetPwm(uint8_t pin, uint16_t freq, uint16_t dutyPerMille);
//extern void init_peripherals(void);
//extern void init_system(void);
//extern uint32 map_baudrate(int8 rate, uint32 default_baud);
//extern unsigned long freeRAM();
//extern void write_leds(uint8 Output);

/************************************************************************/

/* TEST *****************************************************************/
//extern int8 test_mode(uint8 argc, const Menu::arg *argv)
/************************************************************************/

/* TIMERS ***************************************************************/
//#define SUPER_FAST_MILLISECONDS 	1
//#define FAST_MILLISECONDS 			20
//#define MEDIUM_MILLISECONDS 		40
//#define SLOW_MILLISECONDS			100
//#define SUPER_SLOW_MILLISECONDS		1000
//
//extern uint32 super_fast_loop_Timer;	//    1 ms frequency - 1000 Hz
//extern uint32 fast_loop_Timer;			//   20 ms frequency -   50 Hz
//extern uint32 medium_loop_Timer;		//   40 ms frequency -   25 Hz
//extern uint32 slow_loop_Timer;			//  100 ms frequency -   10 Hz
//extern uint32 super_slow_loop_Timer;	// 1000 ms frequency -    1 Hz

//extern void initTimers(void);
/************************************************************************/

/* UTILS ****************************************************************/
//extern void print_hit_enter();
//extern void print_hit_quit();
//extern void print_blanks(int num);
//extern void print_divider(void);
//extern void print_enabled(boolean b);
//extern void print_done();
extern void flash_leds(bool on);
//
//
extern void setup_printf_P(const prog_char_t *fmt, ...);
extern void setup_wait_key(void);
//extern bool setup_wait_input(float * val);
/************************************************************************/



/* RCdecode *************************************************************/
#ifdef GIMBAL_ENABLE_RC
#ifdef RC_USE_LIBS
#include "APM_RC.h"
extern APM_RC_MP32V3 				APM_RC;
#endif

extern void initRC();
extern void checkRcTimeouts();
extern void initRCPins();
extern void evaluateRC();
#endif
/************************************************************************/

/* SerialCom ************************************************************/
extern void setSerialProtocol();
extern void gyroReadCalibration();
/************************************************************************/

/* IMU ******************************************************************/

#ifdef GIMBAL_ENABLE_COMPASS
#include <CompassHMC5843.h>
extern Compass_HMC5843    	compass;
#endif
#if defined ( IMU_BRUGI )
#include "MPU6050.h"
extern MPU6050 mpu;
//IMU aggiuntiva
extern MPU6050 mpu_yaw;
extern bool mpu_yaw_present;

#endif
#if defined( IMU_AP )



#ifdef GIMBAL_ENABLE_COMPASS
#include <AP_Compass.h>         // ArduPilot Mega Magnetometer Library
extern AP_Compass_HMC5843      			compass;
#endif

#include <Arduino_Mega_ISR_Registry.h>
#include <AP_PeriodicProcess.h>
#include <AP_InertialSensor.h>
//#include <AP_ADC.h>
#include <AP_GPS.h>
#include <AP_AHRS.h>
#include <AP_Math.h>
#include <AP_Common.h>

extern Arduino_Mega_ISR_Registry 		isr_registry;
extern AP_TimerProcess  scheduler;
#ifdef GIMBAL_IMU_SPI
extern AP_InertialSensor_MPU6000 	ins;
#else
extern AP_InertialSensor_MPU6000_I2C 	ins;
#endif

extern AP_AHRS_DCM						ahrs;

#endif



extern void initIMU();
extern void initSensorOrientation();
extern void readGyros();
extern void updateGyroAttitude();
extern void updateACCAttitude();
extern void getAttiduteAngles();
extern void readACC(axisDef axis);
extern void updateACC();

#ifdef GIMBAL_ENABLE_COMPASS
extern void readMAG(); //axisDef axis);
extern void updateMAG();
extern void updateMAGAttitude();
#endif

extern void setACCFastMode (bool fastMode);

/************************************************************************/

/* orientationRoutines **************************************************/
extern void initResolutionDevider();
#ifdef IMU_BRUGI
extern void gyroOffsetCalibration(MPU6050 * p_mpu, int16_t * p_offsets);
extern bool accelCalibration(MPU6050 * p_mpu, float * offsets, float * scales);
extern bool calibrate_accel(MPU6050 * mpu, Vector3f & p_accel_offset, Vector3f & p_accel_scale,
		void (*delay_cb)(unsigned long t), void (*flash_leds_cb)(bool on),
                                        void (*send_msg)(const prog_char_t *, ...),
                                        void (*wait_key)(void));
#endif

#ifdef IMU_EVV
void EVV_Init_Orientation();
#endif
/************************************************************************/

/* BLcontroller *********************************************************/
extern void initBlController();
extern void motorTest();
extern void motorMove(uint8_t motorNum, int steps);
extern void switchOffAndMoveToPosition(uint8_t motorNumber, uint8_t maxPWM, int position, uint32_t total_delay);
extern void setPositionAndPower(uint8_t motorNumber, uint8_t pwm, int position);
extern void switchOffMotors();
extern void motorInterrupt();
/************************************************************************/

/* BruGi ****************************************************************/
void initIMU_LPF();
#ifdef IMU_BRUGI
void initMPUlpf(MPU6050 * p_mpu);
#endif
bool read_config();
void write_config();
/************************************************************************/

/* TIMERS ***************************************************************/
#define SUPER_FAST_MICROSECONDS 	500
#define FAST_MICROSECONDS 			5000

extern uint32 superfast_loopTimer;	//    1 ms frequency - 1000 Hz
extern uint32 fast_loopTimer;			//   20 ms frequency -   50 Hz


/************************************************************************/

/* Joystick *************************************************************/
extern void initManualControllers();
extern void ManCmdAxisCalibration();
extern uint16_t getManCmdAxisRC(uint8 nAxis);
extern void getManCmdAxisValue(uint8 nAxis, uint16_t* val);
/************************************************************************/

extern void print_vector(Vector3i v);
extern void print_vector(Vector3f v);

extern bool checkEsc();

#endif // _MAIN_H

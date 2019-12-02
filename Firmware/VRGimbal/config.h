// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "defines.h"

//////////////////////////////////////////////////////////////////////////////
//DEFINIZIONE DELLE FUNZIONALITA' ATTIVE
//////////////////////////////////////////////////////////////////////////////

#define RC_USE_LIBS

//TIPO GESTIONE IMU!!!!
#define IMU_BRUGI
#define IMU_EVV
//#define IMU_AP

#define MAX_IMU_COUNT 2
//#define IMU_SECONDARY_ON_ROLL

//#define MOTOR_COUNT 1

#define BRUGI_USE_OPENBG_PID
#define BRUGI_USE_INTERVAL
//#define BRUGI_USE_INTERRUPT_TIMER
//#define PWM_USE_OFFSET


#define GIMBAL_DTERM_LOWPASS


#define GIMBAL_TEST_MOTORS
#define GIMBAL_TEST_SENSORS


#define GIMBAL_PWM_FREQ  8000// 6400 //32000
#define GIMBAL_MOTOR_RESOLUTION  255

//IMU
#if defined ( IMU_BRUGI )

// Do not change for now
#define MPU6050_GYRO_FS MPU6050_GYRO_FS_250  // +-250,500,1000,2000 deg/s
#define MPU6050_DLPF_BW MPU6050_DLPF_BW_256  // 5,10,20,42,98,188,256 Hz

//#define GIMBAL_ENABLE_COMPASS

#elif defined( IMU_AP )

#define ENABLE_AP_PARAM

//#define GIMBAL_IMU_SPI
//#define GIMBAL_ENABLE_COMPASS

#ifndef MAG_ORIENTATION
	#define MAG_ORIENTATION AP_COMPASS_COMPONENTS_DOWN_PINS_FORWARD
#endif

#ifndef HIL_MODE
 #define HIL_MODE        HIL_MODE_DISABLED
#endif


#else

#error "No IMU defined"

#endif

/*********************** EEPROM *********************************************/
#define FUN_EEPROM
/****************************************************************************/

/*********************** DATA FLASH *****************************************/
//#define FUN_DATAFLASH
/****************************************************************************/


/*********************** JOYSTICK *******************************************/
#define MANUAL_INPUT_COUNT 6
#define MANUAL_INPUT_UPDATE_INTERVAL 25 //ms
/****************************************************************************/


#define GIMBAL_ENABLE_USB
#define GIMBAL_ENABLE_RC


#endif // __CONFIG_H__

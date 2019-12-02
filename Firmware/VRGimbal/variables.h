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
/* Config Structure      */
/*************************/
#ifndef _VARIABLES_H_
#define _VARIABLES_H_


#include "realtimeStatistics.h"

enum MAN_CMD_AXIS_Mode
{
	joyNone,
	joyAnalog,
	joyDigital //(PullUp)
	//DigitalPullDown
};

typedef struct MAN_CMD_AXIS_config{
	uint16_t	Mode;
	uint16_t	Min;
	uint16_t	Mid;
	uint16_t	Max;
} MAN_CMD_AXIS_config_t;

typedef struct calibrationVector
{
	int16_t X;
	int16_t Y;
	int16_t Z;
} calibrationVector_t;



typedef struct rcChannelConfig
{
	int8_t  channel;
	bool    absolute;
	int16_t minOutput;  //valore corrispondente a 1000
	int16_t maxOutput;  //valore corrispondente a 2000
	int16_t gain;       //gain in relative mode
	int16_t LPF;        //low pass filter in absoulte mode

	int8_t resetChannel;

} rcChannelConfig_t;


typedef struct motorAxisConfig
{
	int8_t  	mode;
	int8_t  	motorNumber;
	int8_t  	motorDirection;
	int32_t 	Kp;
	int32_t 	Ki;
	int32_t 	Kd;
	uint8_t 	maxPWM;
	int16_t 	angleOffset;
	int16_t 	offsetMotor;
	uint16_t 	stepsMotor;
	uint16_t 	stepsLimit;

	//follow me params
	int16_t driveLimit1Angle;
	int16_t driveLimit2Angle;
	int16_t maxGyroDrive;

} motorAxisConfig_t;


#define CONFIG_FLOAT_SCALING 1000.0f
//typedef struct calibrationVectorF
//{
//	float X;
//	float Y;
//	float Z;
//} calibrationVectorF_t;



struct configProfile
{

	//impostazioni assi
	motorAxisConfig axisConfig[MOTOR_COUNT];
	rcChannelConfig rcConfig[MOTOR_COUNT];


	//parametri IMU
	bool enableGyro;           // enable gyro attitude update
	bool enableACC;            // enable acc attitude update
	bool enableMAG;
	int16_t accTimeConstant;
	int8_t  mpuLPF;				// mpu LPF 0..6, 0=fastest(256Hz) 6=slowest(5Hz)


	//frequenza Motori
	uint8_t pwmFrequency;
	uint8_t pwmMode;
/*
	int16_t pwmMin;
	int16_t pwmMax;
	int16_t pwmCenter;

	int16_t pwmPhaseA;
	int16_t pwmPhaseB;
	int16_t pwmPhaseC;
*/
	uint8_t pwmFormula;

	//parametri RC
	bool rcModePPM;            // RC mode, true=common RC PPM channel, false=separate RC channels
	int16_t rcMid;             // rc channel center ms


};


struct config_t
{
	uint8_t vers;
	uint8_t versEEPROM;


	//IMU calibration information
	calibrationVector gyroOffset[MAX_IMU_COUNT];
	calibrationVector accOffset[MAX_IMU_COUNT];
	calibrationVector accScale[MAX_IMU_COUNT];
	calibrationVector magMin;
	calibrationVector magMax;


	//parametri IMU
	bool recalibrateOnStartup;
	bool axisReverseZ;
	bool axisSwapXY;


	//parametri Joystick
	MAN_CMD_AXIS_config_t manCmdAxisParam[MANUAL_INPUT_COUNT];


	uint8_t profilesCount;
	uint8_t currentProfile;


	configProfile profiles[PROFILE_COUNT + 1];

};


#ifdef ENABLE_AP_PARAM
//#define BRUGI_CONFIG_EEPROM_POS BOARD_EEPROM_SIZE - sizeof(config_t) //44
#define BRUGI_CONFIG_EEPROM_POS  BOARD_EEPROM_SIZE/2  //0
#else
#define BRUGI_CONFIG_EEPROM_POS  0
#endif

extern config_t config;

void recalcMotorStuff();
void resetMotorFreq();

void initPIDs();

void setDefaultParameters();


typedef struct PIDdata {
  int32_t   Kp, Ki, Kd;
} PIDdata_t;

extern PIDdata_t pitchPIDpar;
extern PIDdata_t rollPIDpar;
extern PIDdata_t yawPIDpar;

void initPIDs(void);




/*************************/
/* Variables             */
/*************************/



// motor drive

extern pwmsin_t pwmSinMotorPitch[N_SIN];
extern pwmsin_t pwmSinMotorRoll[N_SIN];
extern pwmsin_t pwmSinMotorYaw[N_SIN];

extern int currentStepMotor0;
extern int currentStepMotor1;
extern int currentStepMotor2;
extern bool motorUpdate;

extern int8_t pitchDirection;
extern int8_t rollDirection;
extern int8_t yawDirection;

extern int freqCounter; // TODO: back to char later ...

extern int pitchMotorDrive;
extern int rollMotorDrive;
extern int yawMotorDrive;

extern int pitchMotorDrive_PREV;
extern int rollMotorDrive_PREV;
extern int yawMotorDrive_PREV;
extern int pitchMotorDrive_INT_step;
extern int rollMotorDrive_INT_step;
extern int yawMotorDrive_INT_step;
extern bool motor_update_values;

// control motor update in ISR
extern bool enableMotorUpdates;


// Variables for MPU6050
extern float gyroPitch;
extern float gyroRoll; //in deg/s
extern float gyroYaw; //in deg/s

extern float resolutionDevider;
extern int16_t x_val;
extern int16_t y_val;
extern int16_t z_val;

extern float PitchPhiSet;
extern float RollPhiSet;
extern float YawPhiSet;
extern float pitchAngleSet;
extern float rollAngleSet;
extern float yawAngleSet;

extern bool PitchResetting;
extern bool RollResetting;
extern bool YawResetting;

extern int count;

// RC control

struct rcData_t
{
 uint32_t microsRisingEdge;
 uint32_t microsLastUpdate;
 uint16_t rx;
 bool     update;
 bool     valid;
 float    rcSpeed;
 float    setpoint;
};

extern rcData_t rcData[RC_DATA_SIZE];


struct rcConfig_t
{
	int16 minOut;
	int16 maxOut;

	int16 Mid;
	int16 DeadBand;
	int16 Gain;
	int16 LPF;

};

extern rcConfig_t rcConfig[RC_DATA_SIZE];

extern float rcLPF_tc[RC_DATA_SIZE];

// Gimbal State
enum gimStateType {
 GIM_IDLE=0,      // no PID
 GIM_UNLOCKED,    // PID on, fast ACC
 GIM_LOCKED       // PID on, slow ACC
};

extern gimStateType gimState;
//extern int stateCount;
extern uint32_t stateStart;


//*************************************
//
//  IMU
//
//*************************************
struct flags_struct {
  uint8_t SMALL_ANGLES_25 : 1;
};

extern flags_struct flags;

enum axisDef {
	axisROLL,
	axisPITCH,
	axisYAW
};


enum cart_axisDef {
	axisX,
	axisY,
	axisZ
};


typedef struct fp_vector {
  float X;
  float Y;
  float Z;
} t_fp_vector_def;

typedef union {
  float   A[3];
  t_fp_vector_def V;
} t_fp_vector;



//********************
// sensor orientation
//********************
typedef struct sensorAxisDef {
  char idx;
  int  dir;
} t_sensorAxisDef;

typedef struct sensorOrientationDef {
  t_sensorAxisDef Gyro[3];
  t_sensorAxisDef Acc[3];
  t_sensorAxisDef Mag[3];
} t_sensorOrientationDef;

extern t_sensorOrientationDef sensorDef;

// gyro calibration value
extern int16_t gyroOffset[3];

extern float gyroScale;

extern int32_t accSmooth[3];
extern int16_t gyroADC[3];
extern float accADC[3];

//IMU aggiuntiva
extern int16_t gyroOffset2[3];
extern int16_t gyroADC2[3];


//accel calibration
extern float accOffset[3];
extern float accScale[3];
extern float accOffset2[3];
extern float accScale2[3];


extern t_fp_vector EstG;

extern float accLPF[3];
extern float accMag;

extern float AccComplFilterConst;  // filter constant for complementary filter

extern int16_t acc_25deg;      //** TODO: check

extern float angle[3];  // absolute angle inclination of MOTOR AXIS in multiple of 0.01 degree    180 deg = 18000

//extern float estimAngle[3];
//extern float angleIMU[3];  // absolute angle inclination of IMU in multiple of 0.01 degree    180 deg = 18000


// DEBUG only
extern uint32_t stackTop;
extern uint32_t stackBottom;

extern uint32_t heapTop;
extern uint32_t heapBottom;



extern bool g_accOutput;
extern bool g_bSendDebugOutput;
extern bool g_bSendRCOutput;
extern bool g_bSendYawOutput;
extern bool g_bSendJoyOutput;



//PROVA: parametri per regolazione stabilizzazione manuale e follow
//extern float driveLimit1Angle;
//extern float driveLimit2Angle;
//extern float maxGyroDrive;


//debug variables
extern realtimeStatistics interrupt_mean_duration;
extern realtimeStatistics interrupt_mean_lap;

extern realtimeStatistics loop_mean_lap;

#define GIMBAL_TEST_COUNT 10
extern bool g_bTest[GIMBAL_TEST_COUNT];

extern bool g_bTestYawMotor;
extern float g_fTestYawMotorValue;
extern float g_fTestYawMotorSpeed;


//stima della prua del supporto --> per il follow
extern float estimSupportYaw;
extern float estimSupportYawSmooth;


extern int g_driveAlert[3];

#endif //_VARIABLES_H_

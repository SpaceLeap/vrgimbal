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

#include "main.h"
#include "variables.h"


config_t config;
PIDdata_t pitchPIDpar;
PIDdata_t rollPIDpar;
PIDdata_t yawPIDpar;

void setDefaultParameters()
{
	config.vers = VERSION;
	config.versEEPROM = VERSION_EEPROM;


	for (int i = 0; i < MAX_IMU_COUNT; i++)
	{
		config.gyroOffset[i].X = 0;
		config.gyroOffset[i].Y = 0;
		config.gyroOffset[i].Z = 0;

		config.accOffset[i].X = (int16_t) (CONFIG_FLOAT_SCALING * 0.0f);
		config.accOffset[i].Y = (int16_t) (CONFIG_FLOAT_SCALING * 0.0f);
		config.accOffset[i].Z = (int16_t) (CONFIG_FLOAT_SCALING * 0.0f);

		config.accScale[i].X = (int16_t) (CONFIG_FLOAT_SCALING * 1.0f);
		config.accScale[i].Y = (int16_t) (CONFIG_FLOAT_SCALING * 1.0f);
		config.accScale[i].Z = (int16_t) (CONFIG_FLOAT_SCALING * 1.0f);

	}
	config.magMin.X = 0;
	config.magMin.Y = 0;
	config.magMin.Z = 0;
	config.magMax.X = 1;
	config.magMax.Y = 1;
	config.magMax.Z = 1;

	config.profilesCount = PROFILE_COUNT;
	config.currentProfile = 0;



  config.profiles[0].axisConfig[axisPITCH].mode = 0; //0 = PID, 1 = ABSOLUTE
  config.profiles[0].axisConfig[axisROLL].mode = 0; //0 = PID, 1 = ABSOLUTE
  config.profiles[0].axisConfig[axisYAW].mode = 0; //0 = PID, 1 = ABSOLUTE
  config.profiles[0].axisConfig[axisPITCH].Kp = 6000; //20000;
  config.profiles[0].axisConfig[axisPITCH].Ki = 1000; //25000;
  config.profiles[0].axisConfig[axisPITCH].Kd = 12000; ///40000;
  config.profiles[0].axisConfig[axisROLL].Kp = 12000; // 20000;
  config.profiles[0].axisConfig[axisROLL].Ki = 8000; //25000;
  config.profiles[0].axisConfig[axisROLL].Kd = 20000; //30000;
  config.profiles[0].axisConfig[axisYAW].Kp = 7000;
  config.profiles[0].axisConfig[axisYAW].Ki = 2000;
  config.profiles[0].axisConfig[axisYAW].Kd = 35000;
  config.profiles[0].accTimeConstant = 1;
  config.profiles[0].mpuLPF = 0;
  config.profiles[0].axisConfig[axisPITCH].angleOffset = 0;
  config.profiles[0].axisConfig[axisROLL].angleOffset = 0;
  config.profiles[0].axisConfig[axisYAW].angleOffset = 0;
  /*
  config.profiles[0].nPolesMotorPitch = 14;
  config.profiles[0].nPolesMotorRoll = 14;
  config.profiles[0].nPolesMotorYaw = 14;
  */
  config.profiles[0].axisConfig[axisPITCH].motorDirection= 1;
  config.profiles[0].axisConfig[axisROLL].motorDirection = 1;
  config.profiles[0].axisConfig[axisYAW].motorDirection = 1;
  config.profiles[0].axisConfig[axisPITCH].stepsMotor = 8000;  //steps for 360°
  config.profiles[0].axisConfig[axisROLL].stepsMotor = 8000;//steps for 360°
  config.profiles[0].axisConfig[axisYAW].stepsMotor = 8000;//steps for 360°

  config.profiles[0].axisConfig[axisPITCH].offsetMotor = 0;
  config.profiles[0].axisConfig[axisROLL].offsetMotor = 0;
  config.profiles[0].axisConfig[axisYAW].offsetMotor = 0;
  config.profiles[0].axisConfig[axisPITCH].stepsLimit = 0;
  config.profiles[0].axisConfig[axisROLL].stepsLimit = 0;
  config.profiles[0].axisConfig[axisYAW].stepsLimit = 0;


  config.profiles[0].axisConfig[axisPITCH].motorNumber = 0; //1;
  config.profiles[0].axisConfig[axisROLL].motorNumber = 1; //0;
  config.profiles[0].axisConfig[axisYAW].motorNumber = 2; //2;
  config.profiles[0].axisConfig[axisPITCH].maxPWM = 40;
  config.profiles[0].axisConfig[axisROLL].maxPWM = 60;
  config.profiles[0].axisConfig[axisYAW].maxPWM = 80; //127;

  config.profiles[0].pwmFrequency = 8;
  config.profiles[0].pwmMode = 0;

  /*
  config.profiles[0].pwmMin = 0;
  config.profiles[0].pwmMax = 10000;
  config.profiles[0].pwmCenter = 5000;

  config.profiles[0].pwmPhaseA = 0;
  config.profiles[0].pwmPhaseB = 120;
  config.profiles[0].pwmPhaseC = 240;
*/
  config.profiles[0].pwmFormula = 0;



  config.profiles[0].rcConfig[axisPITCH].minOutput = -30;
  config.profiles[0].rcConfig[axisPITCH].maxOutput = 30;
  config.profiles[0].rcConfig[axisROLL].minOutput = -30;
  config.profiles[0].rcConfig[axisROLL].maxOutput = 30;
  config.profiles[0].rcConfig[axisYAW].minOutput = -120;
  config.profiles[0].rcConfig[axisYAW].maxOutput = 120;
  config.profiles[0].rcConfig[axisPITCH].gain = 5;
  config.profiles[0].rcConfig[axisPITCH].LPF = 2;
  config.profiles[0].rcConfig[axisROLL].gain = 5;
  config.profiles[0].rcConfig[axisROLL].LPF = 2;
  config.profiles[0].rcConfig[axisYAW].gain = 5;
  config.profiles[0].rcConfig[axisYAW].LPF = 2;

  config.profiles[0].axisConfig[axisPITCH].driveLimit1Angle = 5;
  config.profiles[0].axisConfig[axisPITCH].driveLimit2Angle = 30;
  config.profiles[0].axisConfig[axisPITCH].maxGyroDrive = 30;
  config.profiles[0].axisConfig[axisROLL].driveLimit1Angle = 5;
  config.profiles[0].axisConfig[axisROLL].driveLimit2Angle = 30;
  config.profiles[0].axisConfig[axisROLL].maxGyroDrive = 30;
  config.profiles[0].axisConfig[axisYAW].driveLimit1Angle = 5;
  config.profiles[0].axisConfig[axisYAW].driveLimit2Angle = 30;
  config.profiles[0].axisConfig[axisYAW].maxGyroDrive = 30;


  config.profiles[0].rcModePPM = false;

  config.profiles[0].rcConfig[axisROLL].channel = 3;
  config.profiles[0].rcConfig[axisPITCH].channel = 1;
  config.profiles[0].rcConfig[axisYAW].channel = 0;

  config.profiles[0].rcConfig[axisPITCH].resetChannel = 2;
  config.profiles[0].rcConfig[axisROLL].resetChannel = 2;
  config.profiles[0].rcConfig[axisYAW].resetChannel = -1;

  config.profiles[0].rcConfig[axisPITCH].absolute = false;
  config.profiles[0].rcConfig[axisROLL].absolute = false;
  config.profiles[0].rcConfig[axisYAW].absolute = false;

  config.profiles[0].rcMid = MID_RC;


  config.profiles[0].enableGyro=true;
  config.profiles[0].enableACC=true;
  config.axisReverseZ=false;
  config.axisSwapXY=false;




  for(uint8 n_axis = 0; n_axis < MANUAL_INPUT_COUNT; n_axis++)
  {
	  if ((n_axis == 1) || (n_axis == 4))
		  config.manCmdAxisParam[n_axis].Mode = joyDigital;
	  else
		  config.manCmdAxisParam[n_axis].Mode = joyAnalog;
  	  config.manCmdAxisParam[n_axis].Min = 0;
  	  config.manCmdAxisParam[n_axis].Mid = 2048;
  	  config.manCmdAxisParam[n_axis].Max = 4096;
  }
  config.profiles[0].enableMAG = true;
  config.recalibrateOnStartup = true;




  //config.crc8 = 0;

}


void initPIDs(void)
{
  rollPIDpar.Kp = config.profiles[0].axisConfig[axisROLL].Kp/10;
  rollPIDpar.Ki = config.profiles[0].axisConfig[axisROLL].Ki; // /1000;
  rollPIDpar.Kd = config.profiles[0].axisConfig[axisROLL].Kd/10;

  pitchPIDpar.Kp = config.profiles[0].axisConfig[axisPITCH].Kp/10;
  pitchPIDpar.Ki = config.profiles[0].axisConfig[axisPITCH].Ki; // /1000;
  pitchPIDpar.Kd = config.profiles[0].axisConfig[axisPITCH].Kd/10;

  yawPIDpar.Kp = config.profiles[0].axisConfig[axisYAW].Kp/10;
  yawPIDpar.Ki = config.profiles[0].axisConfig[axisYAW].Ki;	// /1000;
  yawPIDpar.Kd = config.profiles[0].axisConfig[axisYAW].Kd/10;
}



/*************************/
/* Variables             */
/*************************/



// motor drive

pwmsin_t pwmSinMotorPitch[N_SIN];
pwmsin_t pwmSinMotorRoll[N_SIN];
pwmsin_t pwmSinMotorYaw[N_SIN];

int currentStepMotor0 = 0;
int currentStepMotor1 = 0;
int currentStepMotor2 = 0;
bool motorUpdate = false;

int8_t pitchDirection = 1;
int8_t rollDirection = 1;
int8_t yawDirection = 1;

int freqCounter=0; // TODO: back to char later ...


int pitchMotorDrive = 0;
int rollMotorDrive = 0;
int yawMotorDrive = 0;

int pitchMotorDrive_PREV = 0;
int rollMotorDrive_PREV = 0;
int yawMotorDrive_PREV = 0;
int pitchMotorDrive_INT_step = 0;
int rollMotorDrive_INT_step = 0;
int yawMotorDrive_INT_step = 0;

bool motor_update_values = false;


// control motor update in ISR
bool enableMotorUpdates = false;


// Variables for MPU6050
float gyroPitch;
float gyroRoll; //in deg/s
float gyroYaw; //in deg/s

float resolutionDevider;
int16_t x_val;
int16_t y_val;
int16_t z_val;

float PitchPhiSet = 0;
float RollPhiSet = 0;
float YawPhiSet = 0;
float pitchAngleSet=0;
float rollAngleSet=0;
float yawAngleSet=0;

bool PitchResetting = false;
bool RollResetting = false;
bool YawResetting = false;




int count=0;

// RC control
rcData_t rcData[RC_DATA_SIZE];

float rcLPF_tc[RC_DATA_SIZE] = { 1.0, 1.0, 1.0,
								 1.0, 1.0, 1.0 };

// Gimbal State
gimStateType gimState = GIM_IDLE;
//int stateCount = 0;
uint32_t stateStart = 0;


//*************************************
//
//  IMU
//
//*************************************
flags_struct flags;


//********************
// sensor orientation
//********************

t_sensorOrientationDef sensorDef = {
    {{0, 1}, {1, 1}, {2, 1}},    // Gyro
    {{0, 1}, {1, 1}, {2, 1}},     // Acc
    {{0, 1}, {1, 1}, {2, 1}}     // Mag
  };

// gyro calibration value
int16_t gyroOffset[3] = {0, 0, 0};



float gyroScale=0;

int32_t accSmooth[3];
int16_t gyroADC[3];
float accADC[3];


int16_t gyroOffset2[3] = {0, 0, 0};
int16_t gyroADC2[3];



//accel calibration
float accOffset[3] = {0.0f, 0.0f, 0.0f};
float accScale[3] = {1.0f, 1.0f, 1.0f};

float accOffset2[3] = {0.0f, 0.0f, 0.0f};
float accScale2[3] = {1.0f, 1.0f, 1.0f};


t_fp_vector EstG;

float accLPF[3];
float accMag = 0;

float AccComplFilterConst = 0;  // filter constant for complementary filter

int16_t acc_25deg = 25;      //** TODO: check

float angle[3]    = {0,0, 0};  // absolute angle inclination of MOTOR AXIS in multiple of 0.01 degree    180 deg = 18000

//float estimAngle[3];
//float angleIMU[3]    = {0,0, 0};  // absolute angle inclination of IMU in multiple of 0.01 degree    180 deg = 18000

// DEBUG only
uint32_t stackTop = 0xffffffff;
uint32_t stackBottom = 0;

uint32_t heapTop = 0;
uint32_t heapBottom = 0xffffffff;


bool g_accOutput = false;
bool g_bSendDebugOutput = false;
bool g_bSendRCOutput = false;
bool g_bSendYawOutput = false;
bool g_bSendJoyOutput = false;

//float driveLimit1Angle = 5.0f;
//float driveLimit2Angle = 30.0f;
//float maxGyroDrive = 30.0f;


//debug variables
realtimeStatistics interrupt_mean_duration;
realtimeStatistics interrupt_mean_lap;
realtimeStatistics loop_mean_lap;

bool g_bTest[GIMBAL_TEST_COUNT] = { false };

bool g_bTestYawMotor = false;
float g_fTestYawMotorValue = 0.0f;
float g_fTestYawMotorSpeed = 0.0f;

int g_driveAlert[3] = { -1, -1, -1 };

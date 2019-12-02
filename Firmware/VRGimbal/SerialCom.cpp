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

//************************************************************************************
// general config parameter access routines
//************************************************************************************

#include "main.h"

// types of config parameters
enum confType {
  BOOL,
  INT8,
  INT16,
  INT32,
  UINT8,
  UINT16,
  UINT32
};

#define CONFIGNAME_MAX_LEN 17
typedef struct configDef {
  char name[CONFIGNAME_MAX_LEN];  // name of config parameter
  confType type;                  // type of config parameters
  void * address;                 // address of config parameter
  void (* updateFunction)(void);  // function is called when parameter update happens
} t_configDef;

t_configDef configDef;

// access decriptor as arry of bytes as well
typedef union {
  t_configDef   c;
  char          bytes[sizeof(t_configDef)];
} t_configUnion;

t_configUnion configUnion;


void resetTests()
{
	interrupt_mean_duration.clear();
	interrupt_mean_lap.clear();

	loop_mean_lap.clear();
}


void copyRCGain()
{
	config.profiles[0].rcConfig[axisPITCH].gain = config.profiles[0].rcConfig[axisROLL].gain;
	config.profiles[0].rcConfig[axisYAW].gain = config.profiles[0].rcConfig[axisROLL].gain;
}

void copyRCLPF()
{
	config.profiles[0].rcConfig[axisPITCH].LPF = config.profiles[0].rcConfig[axisROLL].LPF;
	config.profiles[0].rcConfig[axisYAW].LPF = config.profiles[0].rcConfig[axisROLL].LPF;

	initRC();
}

void copyRCAbsolute()
{
	config.profiles[0].rcConfig[axisPITCH].absolute = config.profiles[0].rcConfig[axisROLL].absolute;
	config.profiles[0].rcConfig[axisYAW].absolute = config.profiles[0].rcConfig[axisROLL].absolute;
}

void copyFollow()
{
	config.profiles[0].axisConfig[axisPITCH].driveLimit1Angle = config.profiles[0].axisConfig[axisROLL].driveLimit1Angle;
	config.profiles[0].axisConfig[axisPITCH].driveLimit2Angle = config.profiles[0].axisConfig[axisROLL].driveLimit2Angle;
	config.profiles[0].axisConfig[axisPITCH].maxGyroDrive = config.profiles[0].axisConfig[axisROLL].maxGyroDrive;
	config.profiles[0].axisConfig[axisYAW].driveLimit1Angle = config.profiles[0].axisConfig[axisROLL].driveLimit1Angle;
	config.profiles[0].axisConfig[axisYAW].driveLimit2Angle = config.profiles[0].axisConfig[axisROLL].driveLimit2Angle;
	config.profiles[0].axisConfig[axisYAW].maxGyroDrive = config.profiles[0].axisConfig[axisROLL].maxGyroDrive;
}

//
// list of all config parameters
// to be accessed by par command
//
// descriptor is stored in PROGMEN to preserve RAM space
// see http://www.arduino.cc/en/Reference/PROGMEM
// and http://jeelabs.org/2011/05/23/saving-ram-space/
const t_configDef PROGMEM configListPGM[] = {

	{"vers",			UINT8, &config.vers,             NULL},

	{"modePitch",		INT8,  &config.profiles[0].axisConfig[axisPITCH].mode,    	NULL},
	{"modeRoll",     	INT8,  &config.profiles[0].axisConfig[axisROLL].mode,     	NULL},
	{"modeYaw",      	INT8,  &config.profiles[0].axisConfig[axisYAW].mode,      	NULL},

	{"gyroPitchKp",     INT32, &config.profiles[0].axisConfig[axisPITCH].Kp,      &initPIDs},
	{"gyroPitchKi",     INT32, &config.profiles[0].axisConfig[axisPITCH].Ki,      &initPIDs},
	{"gyroPitchKd",     INT32, &config.profiles[0].axisConfig[axisPITCH].Kd,      &initPIDs},
	{"gyroRollKp",      INT32, &config.profiles[0].axisConfig[axisROLL].Kp,       &initPIDs},
	{"gyroRollKi",      INT32, &config.profiles[0].axisConfig[axisROLL].Ki,       &initPIDs},
	{"gyroRollKd",      INT32, &config.profiles[0].axisConfig[axisROLL].Kd,       &initPIDs},
	{"gyroYawKp",       INT32, &config.profiles[0].axisConfig[axisYAW].Kp,        &initPIDs},
	{"gyroYawKi",       INT32, &config.profiles[0].axisConfig[axisYAW].Ki,        &initPIDs},
	{"gyroYawKd",       INT32, &config.profiles[0].axisConfig[axisYAW].Kd,        &initPIDs},
	#ifdef IMU_BRUGI
	{"accTimeConstant", INT16, &config.profiles[0].accTimeConstant,  &initIMU},
	{"mpuLPF",          INT8,  &config.profiles[0].mpuLPF,           &initIMU_LPF},
	#endif
	{"angleOffsetPitch",INT16, &config.profiles[0].axisConfig[axisPITCH].angleOffset, NULL},
	{"angleOffsetRoll", INT16, &config.profiles[0].axisConfig[axisROLL].angleOffset,  NULL},
	{"angleOffsetYaw",  INT16, &config.profiles[0].axisConfig[axisYAW].angleOffset,  NULL},

	{"dirMotorPitch",   INT8,  &config.profiles[0].axisConfig[axisPITCH].motorDirection,    NULL},
	{"dirMotorRoll",    INT8,  &config.profiles[0].axisConfig[axisROLL].motorDirection,     NULL},
	{"dirMotorYaw",     INT8,  &config.profiles[0].axisConfig[axisYAW].motorDirection,      NULL},

	{"stepsMotorPitch", INT16,  &config.profiles[0].axisConfig[axisPITCH].stepsMotor,    NULL},
	{"stepsMotorRoll",  INT16,  &config.profiles[0].axisConfig[axisROLL].stepsMotor,     NULL},
	{"stepsMotorYaw",   INT16,  &config.profiles[0].axisConfig[axisYAW].stepsMotor,      NULL},

	{"offsetMotorPitch",INT16,  &config.profiles[0].axisConfig[axisPITCH].offsetMotor,    NULL},
	{"offsetMotorRoll", INT16,  &config.profiles[0].axisConfig[axisROLL].offsetMotor,     NULL},
	{"offsetMotorYaw",  INT16,  &config.profiles[0].axisConfig[axisYAW].offsetMotor,      NULL},


	{"limitMotorPitch",INT16,  &config.profiles[0].axisConfig[axisPITCH].stepsLimit,    NULL},
	{"limitMotorRoll", INT16,  &config.profiles[0].axisConfig[axisROLL].stepsLimit,     NULL},
	{"limitMotorYaw",  INT16,  &config.profiles[0].axisConfig[axisYAW].stepsLimit,      NULL},


	{"motorNumberPitch",UINT8, &config.profiles[0].axisConfig[axisPITCH].motorNumber, NULL},
	{"motorNumberRoll", UINT8, &config.profiles[0].axisConfig[axisROLL].motorNumber,  NULL},
	{"motorNumberYaw",  UINT8, &config.profiles[0].axisConfig[axisYAW].motorNumber,   NULL},
	{"maxPWMmotorPitch",UINT8, &config.profiles[0].axisConfig[axisPITCH].maxPWM,  NULL}, //&recalcMotorStuff},
	{"maxPWMmotorRoll", UINT8, &config.profiles[0].axisConfig[axisROLL].maxPWM,   NULL}, //&recalcMotorStuff},
	{"maxPWMmotorYaw",  UINT8, &config.profiles[0].axisConfig[axisYAW].maxPWM,    NULL}, //&recalcMotorStuff},

	{"pwmFrequency",   	UINT8, &config.profiles[0].pwmFrequency,  &resetMotorFreq},
	{"pwmMode",   	UINT8, &config.profiles[0].pwmMode,  &resetMotorFreq},
//	{"pwmMin",   		INT16, &config.profiles[0].pwmMin,  &recalcMotorStuff},
//	{"pwmMax",   		INT16, &config.profiles[0].pwmMax,  &recalcMotorStuff},
//	{"pwmCenter",   	INT16, &config.profiles[0].pwmCenter,  &recalcMotorStuff},
//
//	{"pwmPhaseA",   	INT16, &config.profiles[0].pwmPhaseA,  &recalcMotorStuff},
//	{"pwmPhaseB",   	INT16, &config.profiles[0].pwmPhaseB,  &recalcMotorStuff},
//	{"pwmPhaseC",   	INT16, &config.profiles[0].pwmPhaseC,  &recalcMotorStuff},

	{"pwmFormula",   	INT16, &config.profiles[0].pwmFormula,  &recalcMotorStuff},


	{"minRCPitch",      INT16,  &config.profiles[0].rcConfig[axisPITCH].minOutput,        NULL},
	{"maxRCPitch",      INT16,  &config.profiles[0].rcConfig[axisPITCH].maxOutput,        NULL},
	{"minRCRoll",       INT16,  &config.profiles[0].rcConfig[axisROLL].minOutput,         NULL},
	{"maxRCRoll",       INT16,  &config.profiles[0].rcConfig[axisROLL].maxOutput,         NULL},
	{"minRCYaw",        INT16,  &config.profiles[0].rcConfig[axisYAW].minOutput,          NULL},
	{"maxRCYaw",        INT16,  &config.profiles[0].rcConfig[axisYAW].maxOutput,          NULL},
	//{"rcGain",          INT16, &config.profiles[0].rcConfig[axisROLL].gain,            &copyRCGain},
	{"rcGainRoll",      INT16, &config.profiles[0].rcConfig[axisROLL].gain,            NULL},
	{"rcGainPitch",     INT16, &config.profiles[0].rcConfig[axisPITCH].gain,            NULL},
	{"rcGainYaw",       INT16, &config.profiles[0].rcConfig[axisYAW].gain,            NULL},
	#ifdef GIMBAL_ENABLE_RC
	//{"rcLPF",           INT16, &config.profiles[0].rcConfig[axisROLL].LPF,             &copyRCLPF},
	{"rcLPFRoll",		INT16, &config.profiles[0].rcConfig[axisROLL].LPF,             &initRC},
	{"rcLPFPitch",		INT16, &config.profiles[0].rcConfig[axisPITCH].LPF,             &initRC},
	{"rcLPFYaw",		INT16, &config.profiles[0].rcConfig[axisYAW].LPF,             &initRC},
	{"rcModePPM",       BOOL,  &config.profiles[0].rcModePPM,         NULL},
	#else
	{"rcLPF",           INT16, &config.profiles[0].rcConfig[axisPITCH].LPF,             &copyRCLPF},
	{"rcLPFRoll",		INT16, &config.profiles[0].rcConfig[axisROLL].LPF,             NULL},
	{"rcLPFPitch",		INT16, &config.profiles[0].rcConfig[axisPITCH].LPF,            NULL},
	{"rcLPFYaw",		INT16, &config.profiles[0].rcConfig[axisYAW].LPF,             NULL},
	{"rcModePPM",       BOOL,  &config.profiles[0].rcModePPM,         NULL},
	#endif
	{"rcChannelPitch",  INT8,  &config.profiles[0].rcConfig[axisPITCH].channel,    NULL},
	{"rcChannelRoll",   INT8,  &config.profiles[0].rcConfig[axisROLL].channel,     NULL},
	{"rcChannelYaw",    INT8,  &config.profiles[0].rcConfig[axisYAW].channel,      NULL},

	{"rcChResetPitch",  INT8,  &config.profiles[0].rcConfig[axisPITCH].resetChannel,    NULL},
	{"rcChResetRoll",   INT8,  &config.profiles[0].rcConfig[axisROLL].resetChannel,     NULL},
	{"rcChResetYaw",    INT8,  &config.profiles[0].rcConfig[axisYAW].resetChannel,      NULL},

	{"rcMid",           INT16, &config.profiles[0].rcMid,             NULL},
	//{"rcAbsolute",      BOOL,  &config.profiles[0].rcConfig[axisROLL].absolute,        &copyRCAbsolute},
	{"rcAbsoluteRoll",      BOOL,  &config.profiles[0].rcConfig[axisROLL].absolute,        NULL},
	{"rcAbsolutePitch",      BOOL,  &config.profiles[0].rcConfig[axisPITCH].absolute,      NULL},
	{"rcAbsoluteYaw",      BOOL,  &config.profiles[0].rcConfig[axisYAW].absolute,        NULL},

	{"accOutput",       BOOL,  &g_accOutput,         NULL},

	{"enableGyro",      BOOL,  &config.profiles[0].enableGyro,        NULL},
	{"enableACC",       BOOL,  &config.profiles[0].enableACC,         NULL},

	{"axisReverseZ",    BOOL,  &config.axisReverseZ,      &initSensorOrientation},
	{"axisSwapXY",      BOOL,  &config.axisSwapXY,        &initSensorOrientation},


	{"driveLimit1Angle",INT16,  &config.profiles[0].axisConfig[axisROLL].driveLimit1Angle,    &copyFollow},
	{"driveLimit2Angle",INT16,  &config.profiles[0].axisConfig[axisROLL].driveLimit2Angle,    &copyFollow},
	{"maxGyroDrive",    INT16,  &config.profiles[0].axisConfig[axisROLL].maxGyroDrive,        &copyFollow},


	{"joy0Mode",       	UINT16,  &(config.manCmdAxisParam[0].Mode),        &initManualControllers},
	{"joy0Min",        	UINT16,  &(config.manCmdAxisParam[0].Min),         &initManualControllers},
	{"joy0Mid",        	UINT16,  &(config.manCmdAxisParam[0].Mid),         &initManualControllers},
	{"joy0Max",        	UINT16,  &(config.manCmdAxisParam[0].Max),         &initManualControllers},

	{"joy1Mode",       	UINT16,  &(config.manCmdAxisParam[1].Mode),         &initManualControllers},
	{"joy1Min",        	UINT16,  &(config.manCmdAxisParam[1].Min),         &initManualControllers},
	{"joy1Mid",        	UINT16,  &(config.manCmdAxisParam[1].Mid),         &initManualControllers},
	{"joy1Max",        	UINT16,  &(config.manCmdAxisParam[1].Max),         &initManualControllers},

	{"joy2Mode",       	UINT16,  &(config.manCmdAxisParam[2].Mode),        &initManualControllers},
	{"joy2Min",        	UINT16,  &(config.manCmdAxisParam[2].Min),         &initManualControllers},
	{"joy2Mid",        	UINT16,  &(config.manCmdAxisParam[2].Mid),         &initManualControllers},
	{"joy2Max",        	UINT16,  &(config.manCmdAxisParam[2].Max),         &initManualControllers},

	{"joy3Mode",       	UINT16,  &(config.manCmdAxisParam[3].Mode),         &initManualControllers},
	{"joy3Min",        	UINT16,  &(config.manCmdAxisParam[3].Min),         &initManualControllers},
	{"joy3Mid",        	UINT16,  &(config.manCmdAxisParam[3].Mid),         &initManualControllers},
	{"joy3Max",        	UINT16,  &(config.manCmdAxisParam[3].Max),         &initManualControllers},

	{"joy4Mode",       	UINT16,  &(config.manCmdAxisParam[4].Mode),        &initManualControllers},
	{"joy4Min",        	UINT16,  &(config.manCmdAxisParam[4].Min),         &initManualControllers},
	{"joy4Mid",        	UINT16,  &(config.manCmdAxisParam[4].Mid),         &initManualControllers},
	{"joy4Max",        	UINT16,  &(config.manCmdAxisParam[4].Max),         &initManualControllers},

	{"joy5Mode",       	UINT16,  &(config.manCmdAxisParam[5].Mode),         &initManualControllers},
	{"joy5Min",        	UINT16,  &(config.manCmdAxisParam[5].Min),         &initManualControllers},
	{"joy5Mid",        	UINT16,  &(config.manCmdAxisParam[5].Mid),         &initManualControllers},
	{"joy5Max",        	UINT16,  &(config.manCmdAxisParam[5].Max),         &initManualControllers},


	{"recalibrate",    	BOOL,  &(config.recalibrateOnStartup),         NULL},

	//12345678901234567
	//	{"t_readGyros",    		BOOL,  &(g_bTest[0]),         &resetTests},
	//	{"t_getAttiduteAng",    BOOL,  &(g_bTest[1]),         &resetTests},
	//	{"t_changedrive",    	BOOL,  &(g_bTest[2]),         &resetTests},
	//	{"t_readACC",		    BOOL,  &(g_bTest[3]),         &resetTests},
	//	{"t_updateACC",		    BOOL,  &(g_bTest[4]),         &resetTests},
	//	{"test5",    BOOL,  &(g_bTest[5]),         &resetTests},
	//	{"test6",    BOOL,  &(g_bTest[6]),         &resetTests},
		{"t_dbgDrive",    BOOL,  &(g_bTest[7]),         NULL},
	//	{"t_printPWMsin",    BOOL,  &(g_bTest[8]),         &resetTests},
	{"t_reversePWM",    BOOL,  &(g_bTest[9]),         &resetTests},

	//12345678901234567
	{"t_drvAlertRoll",    INT32,  &(g_driveAlert[axisROLL]),         NULL},
	{"t_drvAlertPitch",    INT32,  &(g_driveAlert[axisPITCH]),         NULL},
	{"t_drvAlertYaw",    INT32,  &(g_driveAlert[axisYAW]),         NULL},



	{"gyro1offsetX",  	INT16,  &(config.gyroOffset[0].X), NULL},
	{"gyro1offsetY",  	INT16,  &(config.gyroOffset[0].Y), NULL},
	{"gyro1offsetZ",  	INT16,  &(config.gyroOffset[0].Z), NULL},

	{"gyro2offsetX",  	INT16,  &(config.gyroOffset[1].X), NULL},
	{"gyro2offsetY",  	INT16,  &(config.gyroOffset[1].Y), NULL},
	{"gyro2offsetZ",  	INT16,  &(config.gyroOffset[1].Z), NULL},

	{"acc1offsetX",  	INT16,  &(config.accOffset[0].X), NULL},
	{"acc1offsetY",  	INT16,  &(config.accOffset[0].Y), NULL},
	{"acc1offsetZ",  	INT16,  &(config.accOffset[0].Z), NULL},

	{"acc1scaleX",  	INT16,  &(config.accScale[0].X), NULL},
	{"acc1scaleY",  	INT16,  &(config.accScale[0].Y), NULL},
	{"acc1scaleZ",  	INT16,  &(config.accScale[0].Z), NULL},

	{"acc2offsetX",  	INT16,  &(config.accOffset[1].X), NULL},
	{"acc2offsetY",  	INT16,  &(config.accOffset[1].Y), NULL},
	{"acc2offsetZ",  	INT16,  &(config.accOffset[1].Z), NULL},

	{"acc2scaleX",  	INT16,  &(config.accScale[1].X), NULL},
	{"acc2scaleY",  	INT16,  &(config.accScale[1].Y), NULL},
	{"acc2scaleZ",  	INT16,  &(config.accScale[1].Z), NULL},


	{NULL, BOOL, NULL, NULL} // terminating NULL required !!
};

// read bytes from program memory
void getPGMstring (PGM_P s, char * d, int numBytes) {
  char c;
  for (int i=0; i<numBytes; i++) {
    *d++ = pgm_read_byte(s++);
  }
}

// find Config Definition for named parameter
t_configDef * getConfigDef(char * name) {

  void * addr = NULL;
  bool found = false;  
  t_configDef * p = (t_configDef *)configListPGM;

  while (true) {
    getPGMstring ((PGM_P)p, configUnion.bytes, sizeof(configDef)); // read structure from program memory
    if (configUnion.c.address == NULL) break;
    if (strncmp(configUnion.c.name, name, CONFIGNAME_MAX_LEN) == 0) {
      addr = configUnion.c.address;
      found = true;
      break;
   }
   p++; 
  }
  if (found) 
      return &configUnion.c;
  else 
      return NULL;
}


// print single parameter value
void printConfig(t_configDef * def) {
  if (def != NULL) {
    cliSerial->print(def->name);
    cliSerial->print(F(" "));
    switch (def->type) {
      case BOOL   : cliSerial->print(*(bool *)(def->address)); break;
      case UINT8  : cliSerial->print(*(uint8_t *)(def->address)); break;
      case UINT16 : cliSerial->print(*(uint16_t *)(def->address)); break;
      case UINT32 : cliSerial->print(*(uint32_t *)(def->address)); break;
      case INT8   : cliSerial->print(*(int8_t *)(def->address)); break;
      case INT16  : cliSerial->print(*(int16_t *)(def->address)); break;
      case INT32  : cliSerial->print(*(int32_t *)(def->address)); break;
    }
    cliSerial->println("");
  } else {
    cliSerial->println(F("ERROR: illegal parameter"));
  }
}

// write single parameter with value
void writeConfig(t_configDef * def, int32_t val) {
  if (def != NULL) {
    switch (def->type) {
      case BOOL   : *(bool *)(def->address)     = val; break;
      case UINT8  : *(uint8_t *)(def->address)  = val; break;
      case UINT16 : *(uint16_t *)(def->address) = val; break;
      case UINT32 : *(uint32_t *)(def->address) = val; break;
      case INT8   : *(int8_t *)(def->address)   = val; break;
      case INT16  : *(int16_t *)(def->address)  = val; break;
      case INT32  : *(int32_t *)(def->address)  = val; break;
    }
    // call update function
    if (def->updateFunction != NULL) def->updateFunction();
  } else {
    cliSerial->println(F("ERROR: illegal parameter"));
  }
}


// print all parameters
void printConfigAll(t_configDef * p) {
  while (true) {
    getPGMstring ((PGM_P)p, configUnion.bytes, sizeof(configDef)); // read structure from program memory
    if (configUnion.c.address == NULL) break;
    printConfig(&configUnion.c);
    p++; 
  }
  cliSerial->println(F("done."));
}

//******************************************************************************
// general parameter modification function
//      par                           print all parameters
//      par <parameter_name>          print parameter <parameter_name>
//      par <parameter_name> <value>  set parameter <parameter_name>=<value>
//*****************************************************************************
void parameterMod() {

  char * paraName = NULL;
  char * paraValue = NULL;
  
  int32_t val = 0;

  if ((paraName = sCmd.next()) == NULL) {
    // no command parameter, print all config parameters
    printConfigAll((t_configDef *)configListPGM);
  } else if ((paraValue = sCmd.next()) == NULL) {
    // one parameter, print single parameter
    printConfig(getConfigDef(paraName));
  } else {
    // two parameters, set specified parameter
    val = atol(paraValue);
    writeConfig(getConfigDef(paraName), val);
  }
}
//************************************************************************************


void updateAllParameters() {
  recalcMotorStuff();
  initPIDs();
  initIMU();

#ifdef IMU_BRUGI
  initMPUlpf(&mpu);
  if (mpu_yaw_present)
	  initMPUlpf(&mpu_yaw);
#endif

  initSensorOrientation();
#ifdef GIMBAL_ENABLE_RC
  initRCPins();
  initRC();
#endif
}

void setDefaultParametersAndUpdate() {
  setDefaultParameters();
  updateAllParameters();
}

void moveMotor()
{
  int motor = atoi(sCmd.next());
  int steps = 10;
  char * str = sCmd.next();
  if (str)
	  steps = atoi(str);
  motorMove((uint8_t) motor, steps);

}

void toggleJoyOutput()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
    g_bSendJoyOutput = true;
  else
	g_bSendJoyOutput = false;
}


void toggleDebugOutput()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
	  g_bSendDebugOutput = true;
  else
	  g_bSendDebugOutput = false;
}

void toggleRCOutput()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
	  g_bSendRCOutput = true;
  else
	  g_bSendRCOutput = false;
}

void toggleYAWOutput()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
	  g_bSendYawOutput = true;
  else
	  g_bSendYawOutput = false;
}

void transmitUseACC()  // TODO: remove obsolete command
{
   cliSerial->println(1);  // dummy for bl_tool compatibility ;-)
}


void toggleACCOutput()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
    g_accOutput = true;
  else
    g_accOutput = false;
}

void toggleDMPOutput()
{
  int temp = atoi(sCmd.next());
}

void toggleCompassLearn()
{
#ifdef GIMBAL_ENABLE_COMPASS
	char * arg = sCmd.next();
	if (arg)
	{
		int temp = atoi(sCmd.next());
		if (temp==1)
			compass.set_learn(true);
		else {
			compass.set_learn(false);
#ifdef COMPASS_AP
			compass.save_offsets();
#endif
		}
	} else {
		cliSerial->printf("Compass Learn %d\r\n", compass.get_learn());
	}
#endif
}


void yawRotateRight()
{
	float tmp = 1.0f;
	char * param = sCmd.next();
	if (param)
		tmp = atof(param);

	YawPhiSet += tmp;
}

void yawRotateLeft()
{
	float tmp = 1.0f;
	char * param = sCmd.next();
	if (param)
		tmp = atof(param);

	YawPhiSet -= tmp;
}

void setUseACC() // TODO: remove obsolete command
{
  int temp = atoi(sCmd.next());
}
/*
void transmitRCConfig()
{
  cliSerial->println(config.profiles[0].rcConfig[axisPITCH].minOutput);
  cliSerial->println(config.profiles[0].rcConfig[axisPITCH].maxOutput);
  cliSerial->println(config.profiles[0].rcConfig[axisROLL].minOutput);
  cliSerial->println(config.profiles[0].rcConfig[axisROLL].maxOutput);
}

void transmitRCAbsolute()
{
  cliSerial->println(config.profiles[0].rcAbsolute);
}

void setRCGain()
{
    config.profiles[0].rcGain = atoi(sCmd.next());
}

void transmitRCGain()
{
  cliSerial->println(config.profiles[0].rcGain);
}

void setRcMode()
{
    config.profiles[0].rcModePPM = atoi(sCmd.next());
    config.profiles[0].rcChannelPitch = atoi(sCmd.next());
    config.profiles[0].rcChannelRoll = atoi(sCmd.next());
#ifdef GIMBAL_ENABLE_RC
    initRCPins();
#endif
}

void transmitRcMode()
{
  cliSerial->println(config.profiles[0].rcModePPM);
  cliSerial->println(config.profiles[0].rcChannelPitch);
  cliSerial->println(config.profiles[0].rcChannelRoll);
}

void setRCAbsolute()
{
  int temp = atoi(sCmd.next());
  if(temp==1)
  {
    config.profiles[0].rcAbsolute = true;
  }
  else
  {
    config.profiles[0].rcAbsolute = false;
  }
  rcData[RC_DATA_PITCH].setpoint = 0.0;
  rcData[RC_DATA_ROLL].setpoint  = 0.0;
  rcData[RC_DATA_PITCH].rcSpeed  = 0.0;
  rcData[RC_DATA_ROLL].rcSpeed   = 0.0;
}

void setRCConfig()
{
  config.profiles[0].rcConfig[axisPITCH].minOutput = atoi(sCmd.next());
  config.profiles[0].rcConfig[axisPITCH].maxOutput = atoi(sCmd.next());
  config.profiles[0].rcConfig[axisROLL].minOutput = atoi(sCmd.next());
  config.profiles[0].rcConfig[axisROLL].maxOutput = atoi(sCmd.next());
}

void transmitSensorOrientation()
{
  cliSerial->println(config.profiles[0].axisReverseZ);
  cliSerial->println(config.profiles[0].axisSwapXY);
}


*/
void writeEEPROM()
{
	write_config();
}

void readEEPROM()
{
	if (read_config())
		updateAllParameters();
}


void test_eeprom()
{
	int i = 0;
	int j = 0;
	cliSerial->printf("Test EEPROM\r\n");

	cliSerial->printf("Read status\r\n");
	byte st = EEPROM.readstatus();

	cliSerial->printf("Status %x \r\n", st);


	cliSerial->print("HEX:\r\n");
	for (j = 0; j < EEPROM_PAGE_SIZE / 32; j++ )
	{
		for (i = 0; i < 32; i++)
		{
			byte b =  EEPROM.read((uint32_t) (i + 32*j));
			cliSerial->printf("%02x ", b);
		}
		cliSerial->print("\r\n");
	}

	cliSerial->print("Bytes:\r\n");

	for (j = 0; j < EEPROM_PAGE_SIZE / 32; j++ )
	{
		for (i = 0; i < 32; i++)
		{
			char c = (char) EEPROM.read((uint32_t) (i + 32*j));
			cliSerial->print(c);
		}
		cliSerial->print("\r\n");
	}

	cliSerial->print("Test EEPROM End\r\n");
}

#ifdef ENABLE_AP_PARAM
void test_params()
{
	cliSerial->print("Test params\r\n");
	AP_Param::show_all(cliSerial);

	cliSerial->print("Test params 2\r\n");
	AP_Param::ParamToken token;
    AP_Param *ap;
    enum ap_var_type type;

    for (ap=AP_Param::first(&token, &type);
         ap;
         ap=AP_Param::next_scalar(&token, &type)) {
    	print_param(type, ap);
    }

    cliSerial->print("Test params End\r\n");

}

void save_params()
{
	cliSerial->print("Save params\r\n");
	//AP_Param::show_all(cliSerial);

	AP_Param::ParamToken token;
    AP_Param *ap;
    enum ap_var_type type;

    for (ap=AP_Param::first(&token, &type);
         ap;
         ap=AP_Param::next_scalar(&token, &type)) {
    	ap->save();
    }

    cliSerial->print("Save params End\r\n");

}

void change_param()
{
	cliSerial->print("Change param\r\n");
	char * nm = sCmd.next();
	char * strval = sCmd.next();

	if ((nm) && (strval))
	{
		AP_Param::ParamToken token;
		AP_Param *ap;
		enum ap_var_type type;

		for (ap=AP_Param::first(&token, &type);
			 ap;
			 ap=AP_Param::next_scalar(&token, &type)) {

			char s[AP_MAX_NAME_SIZE+1];
			ap->copy_name(s, sizeof(s), true);
			s[AP_MAX_NAME_SIZE] = 0;

			if (strcmp(s, nm) == 0)
			{
				float f =  atof(strval);
				switch (type) {
				case AP_PARAM_INT8:
					((AP_Int8 *)ap)->set(f);
					break;
				case AP_PARAM_INT16:
					((AP_Int16 *)ap)->set(f);
					break;
				case AP_PARAM_INT32:
					((AP_Int32 *)ap)->set(f);
					break;
				case AP_PARAM_FLOAT:
					((AP_Float *)ap)->set(f);
					break;
				default:
					break;
				}


				print_param(type, ap);
			}
		}
	}
}
#endif

/*
void transmitActiveConfig()
{
  cliSerial->println(config.profiles[0].vers);
  cliSerial->println(config.profiles[0].axisConfig[axisPITCH].Kp);
  cliSerial->println(config.profiles[0].axisConfig[axisPITCH].Ki);
  cliSerial->println(config.profiles[0].axisConfig[axisPITCH].Kd);
  cliSerial->println(config.profiles[0].axisConfig[axisROLL].Kp);
  cliSerial->println(config.profiles[0].axisConfig[axisROLL].Ki);
  cliSerial->println(config.profiles[0].axisConfig[axisROLL].Kd);
  cliSerial->println(config.profiles[0].accTimeConstant);

  //cliSerial->println(config.profiles[0].nPolesMotorPitch);
  //cliSerial->println(config.profiles[0].nPolesMotorRoll);
  //cliSerial->println(config.profiles[0].axisConfig[axisPITCH].motorDirection);

  cliSerial->println(config.profiles[0].axisConfig[axisROLL].motorDirection);
  cliSerial->println(config.profiles[0].axisConfig[axisPITCH].motorNumber);
  cliSerial->println(config.profiles[0].axisConfig[axisROLL].motorNumber);
  cliSerial->println(config.profiles[0].axisConfig[axisPITCH].maxPWM);
  cliSerial->println(config.profiles[0].axisConfig[axisROLL].maxPWM);
}


void setPitchPID()
{
  config.profiles[0].axisConfig[axisPITCH].Kp = atol(sCmd.next());
  config.profiles[0].axisConfig[axisPITCH].Ki = atol(sCmd.next());
  config.profiles[0].axisConfig[axisPITCH].Kd = atol(sCmd.next());
  initPIDs();
}

void setRollPID()
{
  config.profiles[0].axisConfig[axisROLL].Kp = atol(sCmd.next());
  config.profiles[0].axisConfig[axisROLL].Ki = atol(sCmd.next());
  config.profiles[0].axisConfig[axisROLL].Kd = atol(sCmd.next());
  initPIDs();
}

void setMotorPWM()
{
  config.profiles[0].axisConfig[axisPITCH].maxPWM = atoi(sCmd.next());
  config.profiles[0].axisConfig[axisROLL].maxPWM = atoi(sCmd.next());
  recalcMotorStuff();
}*/

void gyroRecalibrate()
{
#if defined ( IMU_BRUGI )
  mpu.setDLPFMode(MPU6050_DLPF_BW_5);  // experimental AHa: set to slow mode during calibration
  gyroOffsetCalibration(&mpu, gyroOffset);
  initMPUlpf(&mpu);

  if (mpu_yaw_present)
  {
	  mpu_yaw.setDLPFMode(MPU6050_DLPF_BW_5);  // experimental AHa: set to slow mode during calibration
	  gyroOffsetCalibration(&mpu_yaw, gyroOffset2);
	  initMPUlpf(&mpu_yaw);
  }
  cliSerial->println(F("recalibration: done"));
#endif
}


void gyroSaveCalibration()
{
	config.gyroOffset[0].X = gyroOffset[0];
	config.gyroOffset[0].Y = gyroOffset[1];
	config.gyroOffset[0].Z = gyroOffset[2];
	config.gyroOffset[1].X = gyroOffset2[0];
	config.gyroOffset[1].Y = gyroOffset2[1];
	config.gyroOffset[1].Z = gyroOffset2[2];
	config.recalibrateOnStartup = false;
}

void gyroReadCalibration()
{
	gyroOffset[0] = config.gyroOffset[0].X;
	gyroOffset[1] = config.gyroOffset[0].Y;
	gyroOffset[2] = config.gyroOffset[0].Z;
	gyroOffset2[0] = config.gyroOffset[1].X;
	gyroOffset2[1] = config.gyroOffset[1].Y;
	gyroOffset2[2] = config.gyroOffset[1].Z;
}


void accRecalibrate()
{
#if defined ( IMU_BRUGI )
	accelCalibration(&mpu, accOffset, accScale);
	cliSerial->println(F("Accel. recalibration: done"));

	if (mpu_yaw_present)
	{
		cliSerial->println(F("AUX Accel. recalibration..."));
		accelCalibration(&mpu, accOffset2, accScale2);
		cliSerial->println(F("AUX Accel. recalibration: done"));
	}

#else
	ins.calibrate_accel(delay, flash_leds, setup_printf_P, setup_wait_key);
	cliSerial->println("Place gimbal level and press any key.");
	setup_wait_key();
#endif
}


void accSaveCalibration()
{
#if defined ( IMU_BRUGI )
	config.accOffset[0].X = (int16_t) (CONFIG_FLOAT_SCALING * accOffset[0]);
	config.accOffset[0].Y = (int16_t) (CONFIG_FLOAT_SCALING * accOffset[1]);
	config.accOffset[0].Z = (int16_t) (CONFIG_FLOAT_SCALING * accOffset[2]);
	config.accOffset[1].X = (int16_t) (CONFIG_FLOAT_SCALING * accOffset2[0]);
	config.accOffset[1].Y = (int16_t) (CONFIG_FLOAT_SCALING * accOffset2[1]);
	config.accOffset[1].Z = (int16_t) (CONFIG_FLOAT_SCALING * accOffset2[2]);

	config.accScale[0].X = (int16_t) (CONFIG_FLOAT_SCALING * accScale[0]);
	config.accScale[0].Y = (int16_t) (CONFIG_FLOAT_SCALING * accScale[1]);
	config.accScale[0].Z = (int16_t) (CONFIG_FLOAT_SCALING * accScale[2]);
	config.accScale[1].X = (int16_t) (CONFIG_FLOAT_SCALING * accScale2[0]);
	config.accScale[1].Y = (int16_t) (CONFIG_FLOAT_SCALING * accScale2[1]);
	config.accScale[1].Z = (int16_t) (CONFIG_FLOAT_SCALING * accScale2[2]);

#else
	ins.save_params();
#endif
}

void accReadCalibration()
{
	accOffset[0] = ((float) config.accOffset[0].X) / CONFIG_FLOAT_SCALING;
	accOffset[1] = ((float) config.accOffset[0].Y) / CONFIG_FLOAT_SCALING;
	accOffset[2] = ((float) config.accOffset[0].Z) / CONFIG_FLOAT_SCALING;
	accOffset2[0] = ((float) config.accOffset[1].X) / CONFIG_FLOAT_SCALING;
	accOffset2[1] = ((float) config.accOffset[1].Y) / CONFIG_FLOAT_SCALING;
	accOffset2[2] = ((float) config.accOffset[1].Z) / CONFIG_FLOAT_SCALING;


	accScale[0] = ((float) config.accScale[0].X) / CONFIG_FLOAT_SCALING;
	accScale[1] = ((float) config.accScale[0].Y) / CONFIG_FLOAT_SCALING;
	accScale[2] = ((float) config.accScale[0].Z) / CONFIG_FLOAT_SCALING;
	accScale2[0] = ((float) config.accScale[1].X) / CONFIG_FLOAT_SCALING;
	accScale2[1] = ((float) config.accScale[1].Y) / CONFIG_FLOAT_SCALING;
	accScale2[2] = ((float) config.accScale[1].Z) / CONFIG_FLOAT_SCALING;

}
/*
void setMotorDirNo()
{
  config.profiles[0].axisConfig[axisPITCH].motorDirection = atoi(sCmd.next());
  config.profiles[0].axisConfig[axisROLL].motorDirection = atoi(sCmd.next());
  config.profiles[0].axisConfig[axisPITCH].motorNumber = atoi(sCmd.next());
  config.profiles[0].axisConfig[axisROLL].motorNumber = atoi(sCmd.next());
}*/

void setMotorOffsets()
{
	char * tmp = sCmd.next();
	if (tmp)
	{
		config.profiles[0].axisConfig[axisROLL].offsetMotor = atoi(sCmd.next());
		config.profiles[0].axisConfig[axisPITCH].offsetMotor = atoi(sCmd.next());
		config.profiles[0].axisConfig[axisYAW].offsetMotor = atoi(sCmd.next());
	} else {
		config.profiles[0].axisConfig[axisROLL].offsetMotor = rollMotorDrive;
		config.profiles[0].axisConfig[axisPITCH].offsetMotor = pitchMotorDrive;
		config.profiles[0].axisConfig[axisYAW].offsetMotor = yawMotorDrive;
	}

}


uint32 imu_loopTimer = 0;

bool imu_loop()
{

  //motorInterrupt();



//  if (motorUpdate) // loop runs with motor ISR update rate (1000Hz)
	 uint32_t now = millis();
	 uint32_t pid_lap = now - imu_loopTimer;

	 if ( pid_lap >= DT_LOOP_MS)
	 {
		 if (pid_lap > 10 * DT_LOOP_MS)
			 pid_lap = DT_LOOP_MS;

		 imu_loopTimer = now;
    motorUpdate = false;

    readGyros();

    if (config.profiles[0].enableGyro) updateGyroAttitude();
    if (config.profiles[0].enableACC) updateACCAttitude();

    getAttiduteAngles();

    //****************************
    // slow rate actions
    //****************************


    switch (count) {
    case 1:
      readACC(axisROLL); break;
    case 2:
      readACC(axisPITCH); break;
    case 3:
      readACC(axisYAW); break;
    case 4:
      updateACC(); break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      count=0;
      break;
    default:
      break;
    }
    count++;


    //****************************
    // Evaluate Serial inputs
    //****************************
    //verifico se devo cambiare seriale da UART a USB
#ifdef GIMBAL_ENABLE_USB
	if (SerialUSB.isConnected())
		cliSerial = (FastSerial*)&SerialUSB;
	else
		cliSerial = &SerialDBG;
	sCmd.setSerialPort(cliSerial);
#endif
	return true;

  }
	 return false;

}
 /*
void setMotorSteps()
{
	bool sendResponse = false;
	int axis = -1;
	int steps = -1;
	char * tmp = sCmd.next();
	if (tmp)
	{
		axis = atoi(tmp);
	}
	tmp = sCmd.next();
	if (tmp)
	{
		steps = atoi(tmp);
	}

	if (axis >= 0)
	{
		if (steps <= 0)
		{
			sendResponse = true;

			uint8_t pwmRoll = config.profiles[0].axisConfig[axisROLL].maxPWM;
			uint8_t pwmPitch = config.profiles[0].axisConfig[axisPITCH].maxPWM;
			uint8_t pwmYaw = config.profiles[0].axisConfig[axisYAW].maxPWM;

			if (config.profiles[0].axisConfig[axisROLL].maxPWM < 200)
				config.profiles[0].axisConfig[axisROLL].maxPWM = 200;
			if (config.profiles[0].axisConfig[axisPITCH].maxPWM < 200)
				config.profiles[0].axisConfig[axisPITCH].maxPWM = 200;
			if (config.profiles[0].axisConfig[axisYAW].maxPWM < 200)
				config.profiles[0].axisConfig[axisYAW].maxPWM = 200;

			recalcMotorStuff();

			//calcolo in modo automatico
			int32_t init_angle = angle[axis];

			int32_t drive_0 = 0;

			int32_t init_drive = 0;
			int32_t currDrive = 0;

			int16_t minAngle = 0;
			int16_t maxAngle = 0;
			int driveDir = 1;

			switch (axis)
			{
			case axisROLL:
				drive_0 = rollMotorDrive;
				minAngle = config.profiles[0].rcConfig[axisROLL].minOutput * 1000;
				maxAngle = config.profiles[0].rcConfig[axisROLL].maxOutput * 1000;
				driveDir = config.profiles[0].axisConfig[axisROLL].motorDirection;
				break;
			case axisPITCH:
				drive_0 = pitchMotorDrive;
				minAngle = config.profiles[0].rcConfig[axisPITCH].minOutput * 1000;
				maxAngle = config.profiles[0].rcConfig[axisPITCH].maxOutput * 1000;
				driveDir = config.profiles[0].axisConfig[axisPITCH].motorDirection;
				break;
			case axisYAW:
				drive_0 = yawMotorDrive;
				minAngle = config.profiles[0].rcConfig[axisYAW].minOutput * 1000;
				maxAngle = config.profiles[0].rcConfig[axisYAW].maxOutput * 1000;
				driveDir = config.profiles[0].axisConfig[axisYAW].motorDirection;
				break;
			}

			init_drive = drive_0;
			currDrive = drive_0;

			if (minAngle == maxAngle)
			{
				minAngle = angle[axis];
				maxAngle = minAngle + 90000;
			}

			//riporto al MIN
			int driveStep = driveDir * 10;
			if (angle[axis] < minAngle)
				driveStep = -driveStep;

			uint32_t output_tm = 0;
			while (abs(inormalize_yaw(angle[axis] - minAngle)) > 5000)
			{
				uint32_t now = millis();


				if ((now - output_tm) > (1000 / POUT_FREQ))
				{
					output_tm = now;
					cliSerial->printf(F("positioning s: %d a: %d\r\n"), currDrive, angle[axis]);
				}

				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = currDrive;
					break;
				case axisPITCH:
					pitchMotorDrive = currDrive;
					break;
				case axisYAW:
					yawMotorDrive = currDrive;
					break;
				}

				motorInterrupt();
				if (imu_loop())
					currDrive = currDrive - driveStep;
			}

			//piccola pausa...
			output_tm = millis();
			while (millis() - output_tm < 1000)
			{
				imu_loop();
			}


			cliSerial->println(F("Minimum reached. Starting measure."));

			init_drive = currDrive;
			init_angle = angle[axis];

			cliSerial->printf(F("BEGIN s: %d a: %d\r\n"), currDrive, angle[axis]);
			output_tm = 0;
			//while ( ((currDrive - init_drive) < 10000) && (abs(inormalize_yaw(angle[axis] - init_angle)) < 90000))
			while (abs(inormalize_yaw(maxAngle - angle[axis])) > 5000)
			{

			    uint32_t now = millis();

			    if ((now - output_tm) > (1000 / POUT_FREQ))
				{
			    	output_tm = now;
			    	cliSerial->printf(F("setMotorSteps s: %d a: %d\r\n"), currDrive, angle[axis]);
				}

				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = currDrive;
					break;
				case axisPITCH:
					pitchMotorDrive = currDrive;
					break;
				case axisYAW:
					yawMotorDrive = currDrive;
					break;
				}

				motorInterrupt();
				if (imu_loop())
					currDrive = currDrive + driveDir;
			}

			cliSerial->printf(F("END s: %d a: %d\r\n"), currDrive, angle[axis]);
			int32_t totsteps = currDrive - init_drive;
			int32_t totangle = angle[axis] - init_angle;
			steps = (int32_t) fabs( (float) totsteps * 360000.0f / (float) totangle );


			driveStep = -10;
			if (drive_0 > currDrive)
				driveStep = 10;

			//rimetto a posto
			while (abs(currDrive - drive_0) <= 10)
			{

			    uint32_t now = millis();
			    static uint32_t output_tm = 0;

			    if ((now - output_tm) > (1000 / POUT_FREQ))
				{
			    	output_tm = now;
			    	cliSerial->printf(F("rewind s: %d a: %d\r\n"), currDrive, angle[axis]);
				}

				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = currDrive;
					break;
				case axisPITCH:
					pitchMotorDrive = currDrive;
					break;
				case axisYAW:
					yawMotorDrive = currDrive;
					break;
				}

				motorInterrupt();
				if (imu_loop())
					currDrive = currDrive + driveDir;

			}

			//finito
			config.profiles[0].axisConfig[axisROLL].maxPWM = pwmRoll;
			config.profiles[0].axisConfig[axisPITCH].maxPWM = pwmPitch;
			config.profiles[0].axisConfig[axisYAW].maxPWM = pwmYaw;
			recalcMotorStuff();

		}

		//imposto
		if (steps > 0)
		{
			switch (axis)
			{
			case axisROLL:
				config.profiles[0].axisConfig[axisROLL].stepsMotor = steps;
				break;
			case axisPITCH:
				config.profiles[0].axisConfig[axisPITCH].stepsMotor = steps;
				break;
			case axisYAW:
				config.profiles[0].axisConfig[axisYAW].stepsMotor = steps;
				break;
			}
		}
		if (sendResponse)
		{
			switch (axis)
			{
			case axisROLL:
				cliSerial->printf(F("stepsMotorRoll %d\r\n"), config.profiles[0].axisConfig[axisROLL].stepsMotor);
				break;
			case axisPITCH:
				cliSerial->printf(F("stepsMotorPitch %d\r\n"), config.profiles[0].axisConfig[axisPITCH].stepsMotor);
				break;
			case axisYAW:
				cliSerial->printf(F("stepsMotorYaw %d\r\n"), config.profiles[0].axisConfig[axisYAW].stepsMotor);
				break;
			}
		}
	}
}*/

//#define SMS_USE_REALTIME_SIN

void setMotorSteps()
{
	bool sendResponse = false;
	int axis = -1;
	int steps = -1;
	char * tmp = sCmd.next();
	if (tmp)
	{
		axis = atoi(tmp);
	}
	tmp = sCmd.next();
	if (tmp)
	{
		steps = atoi(tmp);
	}

	if (axis >= 0)
	{
		if (steps <= 0)
		{
			bool bAbort = false;

			sendResponse = true;

			uint8_t initPower = 0;
			int32_t drive_0 = 0;
			uint8_t maxPower = 200;
			uint8_t currPower = 0;
			uint8_t motorNum = 0;

			switch (axis)
			{
			case axisROLL:
				drive_0 = rollMotorDrive;
				initPower = config.profiles[0].axisConfig[axisROLL].maxPWM;
				//driveDir = config.profiles[0].axisConfig[axisROLL].motorDirection;
				motorNum = config.profiles[0].axisConfig[axisROLL].motorNumber;
				break;
			case axisPITCH:
				drive_0 = pitchMotorDrive;
				initPower = config.profiles[0].axisConfig[axisPITCH].maxPWM;
				//driveDir = config.profiles[0].axisConfig[axisPITCH].motorDirection;
				motorNum = config.profiles[0].axisConfig[axisPITCH].motorNumber;
				break;
			case axisYAW:
				drive_0 = yawMotorDrive;
				initPower = config.profiles[0].axisConfig[axisYAW].maxPWM;
				//driveDir = config.profiles[0].axisConfig[axisYAW].motorDirection;
				motorNum = config.profiles[0].axisConfig[axisYAW].motorNumber;
				break;
			}

			int32_t currDrive = drive_0;
			//blocco la posizione iniziale
			currPower = initPower;

			uint32_t output_tm = 0;
			for (uint8_t pwr = initPower; pwr <= maxPower; pwr++)
			{
				int cnt = 0;
				//while (cnt < 2)
				{
					if (imu_loop())
						cnt++;
				}
				uint32_t now = millis();
				if ((now - output_tm) > (1000 / POUT_FREQ))
				{
					output_tm = now;
					cliSerial->printf(F("positioning s: %d a: %d pwr: %d\r\n"), (int) drive_0, (int) angle[axis], (int) pwr);
				}
#ifdef SMS_USE_REALTIME_SIN
				setPositionAndPower(motorNum, pwr, drive_0);
#else
				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = drive_0;
					config.profiles[0].axisConfig[axisROLL].maxPWM = pwr;
					break;
				case axisPITCH:
					pitchMotorDrive = drive_0;
					config.profiles[0].axisConfig[axisPITCH].maxPWM = pwr;
					break;
				case axisYAW:
					yawMotorDrive = drive_0;
					config.profiles[0].axisConfig[axisYAW].maxPWM = pwr;
					break;
				}
				motorInterrupt();
#endif
				currPower = pwr;

				if (checkEsc())
				{
					bAbort = true;
					break;
				}

			}


			int32_t init_angle = angle[axis];


			if (!bAbort)
			{

#ifdef SMS_USE_REALTIME_SIN
#else
				switch (axis)
				{
				case axisROLL:
					config.profiles[0].axisConfig[axisROLL].maxPWM = currPower;
					break;
				case axisPITCH:
					config.profiles[0].axisConfig[axisPITCH].maxPWM = currPower;
					break;
				case axisYAW:
					config.profiles[0].axisConfig[axisYAW].maxPWM = currPower;
					break;
				}
#endif

				//mi muovo di 10 gradi per calcolare gli step
				output_tm = 0;
				while (abs(inormalize_yaw(angle[axis] - init_angle)) < 10000)
				{
					uint32_t now = millis();
					if ((now - output_tm) > (1000 / POUT_FREQ))
					{
						output_tm = now;
						cliSerial->printf(F("measuring s: %d a: %d\r\n"), (int) currDrive, (int) angle[axis]);
					}
#ifdef SMS_USE_REALTIME_SIN
					setPositionAndPower(motorNum, currPower, currDrive);
#else
					switch (axis)
					{
					case axisROLL:
						rollMotorDrive = currDrive;
						break;
					case axisPITCH:
						pitchMotorDrive = currDrive;
						break;
					case axisYAW:
						yawMotorDrive = currDrive;
						break;
					}

					motorInterrupt();
#endif
					if (checkEsc())
					{
						bAbort = true;
						break;
					}

					int cnt = 0;
					while (cnt < 2)
					{
						if (imu_loop())
							cnt++;
					}
					currDrive++;
				}
				if (!bAbort)
				{
					cliSerial->printf(F("END s: %d a: %d\r\n"), (int) currDrive, (int) angle[axis]);
					int32_t totsteps = currDrive - drive_0;
					int32_t totangle = angle[axis] - init_angle;
					steps = (int32_t) ( (float) totsteps * 360000.0f / (float) totangle );

					if (steps < 0)
						cliSerial->println("Detected NEGATIVE motor direction.");
					else
						cliSerial->println("Detected POSITIVE motor direction.");

					steps = (int32_t) fabs(steps);
				}
			}

			if (bAbort)
			{
				cliSerial->println("Requested stop: exiting.");
			}

			//riposiziono in posizione iniziale
			output_tm = 0;
			while (abs(inormalize_yaw(angle[axis] - init_angle)) > 1000)
			{
				uint32_t now = millis();
				if ((now - output_tm) > (1000 / POUT_FREQ))
				{
					output_tm = now;
					cliSerial->printf(F("repositioning s: %d a: %d\r\n"), (int) currDrive, (int) angle[axis]);
				}
#ifdef SMS_USE_REALTIME_SIN
				setPositionAndPower(motorNum, currPower, currDrive);
#else
				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = currDrive;
					break;
				case axisPITCH:
					pitchMotorDrive = currDrive;
					break;
				case axisYAW:
					yawMotorDrive = currDrive;
					break;
				}

				motorInterrupt();
#endif
				if (checkEsc())
				{
					bAbort = true;
					break;
				}

				int cnt = 0;
				//while (cnt < 2)
				//{
					if (imu_loop())
						cnt++;
				//}
				currDrive--;
			}

			//riabbasso la potenza
			output_tm = 0;
			for (uint8_t pwr = currPower; pwr >= initPower; pwr--)
			{
				int cnt = 0;
				//while (cnt < 2)
				{
					if (imu_loop())
						cnt++;
				}
				uint32_t now = millis();
				if ((now - output_tm) > (1000 / POUT_FREQ))
				{
					output_tm = now;
					cliSerial->printf(F("repositioning s: %d a: %d pwr: %d\r\n"), (int) currDrive, (int)  angle[axis], (int) pwr);
				}
#ifdef SMS_USE_REALTIME_SIN
				setPositionAndPower(motorNum, pwr, currDrive);
#else
				switch (axis)
				{
				case axisROLL:
					rollMotorDrive = currDrive;
					config.profiles[0].axisConfig[axisROLL].maxPWM = pwr;
					break;
				case axisPITCH:
					pitchMotorDrive = currDrive;
					config.profiles[0].axisConfig[axisPITCH].maxPWM = pwr;
					break;
				case axisYAW:
					yawMotorDrive = currDrive;
					config.profiles[0].axisConfig[axisYAW].maxPWM = pwr;
					break;
				}
				motorInterrupt();
#endif

			}

			switch (axis)
			{
			case axisROLL:
				config.profiles[0].axisConfig[axisROLL].maxPWM = initPower;
				break;
			case axisPITCH:
				config.profiles[0].axisConfig[axisPITCH].maxPWM = initPower;
				break;
			case axisYAW:
				config.profiles[0].axisConfig[axisYAW].maxPWM = initPower;
				break;
			}

		}

		//imposto
		if (steps > 0)
		{
			switch (axis)
			{
			case axisROLL:
				config.profiles[0].axisConfig[axisROLL].stepsMotor = steps;
				break;
			case axisPITCH:
				config.profiles[0].axisConfig[axisPITCH].stepsMotor = steps;
				break;
			case axisYAW:
				config.profiles[0].axisConfig[axisYAW].stepsMotor = steps;
				break;
			}
		}
		if (sendResponse)
		{
			switch (axis)
			{
			case axisROLL:
				cliSerial->printf(F("stepsMotorRoll %d\r\n"), config.profiles[0].axisConfig[axisROLL].stepsMotor);
				break;
			case axisPITCH:
				cliSerial->printf(F("stepsMotorPitch %d\r\n"), config.profiles[0].axisConfig[axisPITCH].stepsMotor);
				break;
			case axisYAW:
				cliSerial->printf(F("stepsMotorYaw %d\r\n"), config.profiles[0].axisConfig[axisYAW].stepsMotor);
				break;
			}
		}
	}
}


void setDriveParams()
{
	char * tmp = sCmd.next();
	if (tmp == NULL)
	{
		//stampo i parametri
		cliSerial->print(F("driveLimit1Angle "));
		cliSerial->print(config.profiles[0].axisConfig[axisROLL].driveLimit1Angle);
		cliSerial->println();
		cliSerial->print(F("driveLimit2Angle "));
		cliSerial->print(config.profiles[0].axisConfig[axisROLL].driveLimit2Angle);
		cliSerial->println();
		cliSerial->print(F("maxGyroDrive "));
		cliSerial->print(config.profiles[0].axisConfig[axisROLL].maxGyroDrive);
		cliSerial->println();
	} else {
		//imposto i parametri
		config.profiles[0].axisConfig[axisROLL].driveLimit1Angle = atoi(tmp);
		config.profiles[0].axisConfig[axisROLL].driveLimit2Angle = atoi(sCmd.next());
		config.profiles[0].axisConfig[axisROLL].maxGyroDrive = atoi(sCmd.next());
		copyFollow();
	}
}

void setSensorOrientation()
{
  config.axisReverseZ = atoi(sCmd.next());
  config.axisSwapXY = atoi(sCmd.next());

  initSensorOrientation();
  
}

void outputWaveForm()
{
	cliSerial->print("Motor Drive Waveform\r\n");
	for (int i = 0; i < N_SIN; i++)
	{
		cliSerial->print(i);
		cliSerial->print(" ");
		cliSerial->print(g_pwmSinMotor[i]);
		cliSerial->print("\r\n");
	}
	cliSerial->print("Done.\r\n");
}

void testMotorMovement()
{

	interrupt_mean_lap.clear();

	int mtr = atoi(sCmd.next());
	cliSerial->print("mtr ");
	int spd = atoi(sCmd.next());
	cliSerial->print("spd ");
	int steps = atoi(sCmd.next());
	cliSerial->print("stp ");
	int slow = atoi(sCmd.next());
	cliSerial->print("ms ");
	int slowus = atoi(sCmd.next());
	cliSerial->print("us ");
	bool bDebugMotor = false;
	int d = atoi(sCmd.next());
	if (d != 0)
		bDebugMotor = true;

	float looplap = (float) slow + (float) slowus / 1000.0;

	if (spd == 0)
		spd = 1;


	cliSerial->printf("Starting test on motor %d totsteps %d speed %d / ", mtr, steps, spd );
	cliSerial->print(looplap);
	cliSerial->println(" ms");

	steps = steps / spd;


	float lap_mean = 0;
	uint32_t output_tm = 0;

	int i = 0;
	if ((mtr >= 0) && (mtr < 3))
	{
		uint32_t u_last_motor_update = 0;
		while (i < steps)
		{
			uint32_t unow = micros();
			//int pid_lap = (int) measure_micro_delay( u_last_motor_update, unow);
			//if (( pid_lap >= slowus) || (u_last_motor_update == 0))


			float pid_lap = ((float) measure_micro_delay( u_last_motor_update, unow) / 1000.0f);
			if ( pid_lap >= looplap)//DT_LOOP_MS * slow)
			{
				u_last_motor_update = unow;
				switch (mtr)
				{
				case axisROLL:
					rollMotorDrive = rollMotorDrive + spd;
					break;
				case axisPITCH:
					pitchMotorDrive = pitchMotorDrive + spd;
					break;
				case axisYAW:
					yawMotorDrive = yawMotorDrive + spd;
					break;
				}
				motorInterrupt();

				if (i == 0)
					lap_mean = pid_lap;
				else
				{
					lap_mean = (lap_mean * (float) i + pid_lap) / (float)(i + 1);
				}

				i++;
			}

			if (bDebugMotor)
			{
				imu_loop();
				uint32_t now = millis();
				if ((now - output_tm) > (1000 / POUT_FREQ))
				{
					output_tm = now;
					cliSerial->print(angle[axisPITCH]);
					cliSerial->print(F(" ACC "));cliSerial->print(angle[axisROLL]);
					cliSerial->print(F(" "));cliSerial->print(angle[axisYAW]);
					cliSerial->printf(F(" MTR %d %d %d\r\n"), pitchMotorDrive, rollMotorDrive, yawMotorDrive);
				}
			}

		}
	}

	cliSerial->print("Done. Lap mean: ");
	cliSerial->print(lap_mean);
	cliSerial->println(" ms.");
	cliSerial->print(interrupt_mean_lap.mean());
	cliSerial->println(" ms.");


}

void printHelpUsage()
{
  cliSerial->println(F("This gives you a list of all commands with usage:"));
  cliSerial->println(F("Explanations are in brackets(), use integer values only !"));
  cliSerial->println(F(""));
  cliSerial->println(F("these are the preferred commands, use them for new GUIs !!"));
  cliSerial->println(F(""));
  cliSerial->println(F("SD    (Set Defaults)"));
  cliSerial->println(F("WE    (Writes active config to eeprom)"));
  cliSerial->println(F("RE    (Restores values from eeprom to active config)"));
  cliSerial->println(F("GC    (Recalibrates the Gyro Offsets)"));
  cliSerial->println(F("par <parName> <parValue>   (general parameter read/set command)"));
  cliSerial->println(F("    example usage:"));
  cliSerial->println(F("       par                     ... list all config parameters"));
  cliSerial->println(F("       par gyroPitchKi         ... list gyroPitchKi"));
  cliSerial->println(F("       par gyroPitchKi 12000   ... set gyroPitchKi to 12000"));
  cliSerial->println(F(""));
  cliSerial->println(F("these commands are intendend for commandline users and compatibilty with 049 GUI"));
  cliSerial->println(F("TC    (transmits all config values in eeprom save order)"));
  cliSerial->println(F("SP gyroPitchKp gyroPitchKi gyroPitchKd    (Set PID for Pitch)"));
  cliSerial->println(F("SR gyroRollKp gyroRollKi gyroRollKd    (Set PID for Roll)"));
  cliSerial->println(F("SE maxPWMmotorPitch maxPWMmotorRoll     (Used for Power limitiation on each motor 255=high, 1=low)"));
  cliSerial->println(F("SM dirMotorPitch dirMotorRoll motorNumberPitch motorNumberRoll"));
  cliSerial->println(F("SMO offsetMotorRoll offsetMotorPitch offsetMotorYaw (No param to use current position)"));
  cliSerial->println(F("SSO reverseZ swapXY (set sensor orientation)"));
  cliSerial->println(F("TSO   (Transmit sensor orientation)"));
  cliSerial->println(F("TRC   (transmitts RC Config)"));
  cliSerial->println(F("SRC minRCPitch maxRCPitch minRCRoll maxRCRoll (angles -90..90)"));
  cliSerial->println(F("SCA rcAbsolute (1 = true, RC control is absolute; 0 = false, RC control is proportional)"));
  cliSerial->println(F("SRG rcGain (set RC gain)"));
  cliSerial->println(F("SRM modePPM channelPitch channelRoll (set RC mode: modePPM 1=PPM 0=single channels, channelPitch/Roll = channel assignment 0..7)"));
  cliSerial->println(F("TCA   (Transmit RC control absolute or not)"));
  cliSerial->println(F("TRG   (Transmit RC gain)"));
  cliSerial->println(F("TRM   (Transmit RC mode"));
  cliSerial->println(F("UAC useACC (1 = true, ACC; 0 = false, DMP)"));
  cliSerial->println(F("TAC   (Transmit ACC status)"));
  cliSerial->println(F("OAC accOutput (Toggle Angle output in ACC mode: 1 = true, 0 = false)"));
  cliSerial->println(F("ODM dmpOutput (Toggle Angle output in DMP mode: 1 = true, 0 = false)"));
  cliSerial->println(F("MM motorNum Steps (force move motor)  "));
  cliSerial->println(F("ODB debugOutput (Toggle Debug output: 1 = true, 0 = false)"));
  cliSerial->println(F("ORC RCOutput (Toggle RC Debug output: 1 = true, 0 = false)"));
  cliSerial->println(F("HE     (print this output)"));
  cliSerial->println(F("HE par (print config paramter description)"));
  cliSerial->println(F(""));
  cliSerial->println(F("Note: command input is case-insensitive, commands are accepted in both upper/lower case"));
}

void printHelpParameters () {
  cliSerial->println(F("description of config paramters:"));
  cliSerial->println(F(""));
  cliSerial->println(F("vers"));
  cliSerial->println(F("  firmware version"));
  cliSerial->println(F(""));
  cliSerial->println(F("gyroPitchKp/gyroRollKp"));
  cliSerial->println(F("  pid controller P-value"));
  cliSerial->println(F("gyroPitchKi/gyroRollKi"));
  cliSerial->println(F("  pid controller I-value"));
  cliSerial->println(F("gyroPitchKd/gyroRollKd"));
  cliSerial->println(F("  pid controller D-value"));
  cliSerial->println(F("accTimeConstant"));
  cliSerial->println(F("  time constant of ACC complementary filter."));
  cliSerial->println(F("  controls how fast the gimbal follows ACC."));
  cliSerial->println(F("  unit = 1 sec, e.g. 7 = 7 seconds"));
  cliSerial->println(F("mpuLPF"));
  cliSerial->println(F("  low pass filter of gyro (DLPFMode)"));
  cliSerial->println(F("  legal values are 0...6, 0=fastest 6=slowest"));
  cliSerial->println(F("  use slow values if high frequency oscillations occur (still experimental)"));
  cliSerial->println(F(""));
  cliSerial->println(F("angleOffsetPitch/angleOffsetRoll"));
  cliSerial->println(F("  offset of gimbal zero position"));
  cliSerial->println(F("  unit = 0.01 deg, e.g. 500 = 5.00 deg"));
  cliSerial->println(F(""));
  cliSerial->println(F("dirMotorPitch/dirMotorRoll"));
  cliSerial->println(F("  motor direction"));
  cliSerial->println(F("  1 = normal, -1 = reverse direction"));
  cliSerial->println(F("motorNumberPitch/motorNumberRoll"));
  cliSerial->println(F("  assign motor output for pitch and roll, legal values are 0 or 1"));
  cliSerial->println(F("maxPWMmotorPitch/maxPWMmotorRoll"));
  cliSerial->println(F("  motor power, legal range 0 to 255"));
  cliSerial->println(F(""));
  cliSerial->println(F("minRCPitch/minRCRoll"));
  cliSerial->println(F("  RC minimum set point angle, unit = 1 deg"));
  cliSerial->println(F("maxRCPitch/maxRCRoll"));
  cliSerial->println(F("  RC maximum set point angle, unit = 1 deg"));
  cliSerial->println(F("rcGain"));
  cliSerial->println(F("  RC gain in Relative mode, specifies speed of gimbal movement"));
  cliSerial->println(F("rcLPF"));
  cliSerial->println(F("  RC low pass filter in Absolute mode, specified speed of gimbal movement"));
  cliSerial->println(F("  unit = 0.1 sec, e.g. 20 = 2.0 seconds"));
  cliSerial->println(F(""));
  cliSerial->println(F("rcModePPM"));
  cliSerial->println(F("  0 ... use two RC PWM inputs on A0 and A1"));
  cliSerial->println(F("  1 ... use PPM sum input on A0"));
  cliSerial->println(F(""));
  cliSerial->println(F("rcChannelPitch"));
  cliSerial->println(F("   RC channel assignment for RC pitch, legal values are 0 to 7 in PPM mode"));
  cliSerial->println(F("rcChannelRoll"));
  cliSerial->println(F("   RC channel assignment for RC roll, legal values are 0 to 7 in PPM mode"));
  cliSerial->println(F(""));
  cliSerial->println(F("rcMid"));
  cliSerial->println(F("   RC center position, unit = 1 msec, default=1500"));
  cliSerial->println(F("rcAbsolute"));
  cliSerial->println(F("   0 ... RC Relative Mode, gimbal position is incremented/decremented by RC"));
  cliSerial->println(F("   1 ... RC Absolute Mode, RC controls gimbal directly"));
  cliSerial->println(F(""));
  cliSerial->println(F("accOutput"));
  cliSerial->println(F("   1 ... enable ACC printout for GUI chart display"));
  cliSerial->println(F("enableGyro/enableACC"));
  cliSerial->println(F("   1 ... enable Gyro/ACC update in control loop"));
  cliSerial->println(F("         just for test and adjustment purposes"));
  cliSerial->println(F(""));
  cliSerial->println(F("axisReverseZ"));
  cliSerial->println(F("   0 ... sensor is mounted with component side up"));
  cliSerial->println(F("   1 ... sensor is mounted with component side down"));
  cliSerial->println(F("axisSwapXY"));
  cliSerial->println(F("   0 ... standard X/Y sensor orientation"));
  cliSerial->println(F("   1 ... swap X/Y, exchange roll/pitch function, when sensor is rotated 90 degrees"));
  cliSerial->println(F(""));
}

void helpMe() {
  char * paraName = NULL;
  if ((paraName = sCmd.next()) == NULL) {
    // no command parameter, print usage
    printHelpUsage();
  } else if (strncmp(paraName, "par", CONFIGNAME_MAX_LEN) == 0) {
    // print parameter help
    printHelpParameters();
  } else {
    printHelpUsage();
  }
}

void printVersion()
{
	cliSerial->printf("\r\nVERSION %s | %s %s\r\n", THISFIRMWARE, __DATE__, __TIME__);
}

void unrecognized(const char *command) 
{
	const char * buf = sCmd.getBuffer();

  //cliSerial->println(F("What? type in HE for Help ..."));
  cliSerial->printf(F("What[%s]? #%s# type in HE for Help ..."), command, buf);
  cliSerial->println();
}


void setSerialProtocol()
{
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("sd", setDefaultParametersAndUpdate);   
  sCmd.addCommand("we", writeEEPROM);   
  sCmd.addCommand("re", readEEPROM); 
  sCmd.addCommand("par", parameterMod);
  sCmd.addCommand("gc", gyroRecalibrate);

//  sCmd.addCommand("tc", transmitActiveConfig);
//  sCmd.addCommand("sp", setPitchPID);
//  sCmd.addCommand("sr", setRollPID);
//  sCmd.addCommand("se", setMotorPWM);
//  sCmd.addCommand("sm", setMotorDirNo);

  sCmd.addCommand("smo", setMotorOffsets);
  sCmd.addCommand("sms", setMotorSteps);

  sCmd.addCommand("sdp", setDriveParams);


//  sCmd.addCommand("sso", setSensorOrientation);
//  sCmd.addCommand("tso", transmitSensorOrientation);
//  sCmd.addCommand("trc", transmitRCConfig);
//  sCmd.addCommand("src", setRCConfig);
//  sCmd.addCommand("srg", setRCGain);
//  sCmd.addCommand("srm", setRcMode);
//  sCmd.addCommand("trm", transmitRcMode);
//  sCmd.addCommand("sca", setRCAbsolute);
//  sCmd.addCommand("tca", transmitRCAbsolute);
//  sCmd.addCommand("trg", transmitRCGain);
  sCmd.addCommand("uac", setUseACC);
  sCmd.addCommand("tac", transmitUseACC);
  sCmd.addCommand("oac", toggleACCOutput);
  sCmd.addCommand("odm", toggleDMPOutput);
  sCmd.addCommand("mm", moveMotor);
  sCmd.addCommand("odb", toggleDebugOutput);
  sCmd.addCommand("orc", toggleRCOutput);
  sCmd.addCommand("oyw", toggleYAWOutput);

  sCmd.addCommand("te", test_eeprom);

#ifdef ENABLE_AP_PARAM
  sCmd.addCommand("apt", test_params);
  sCmd.addCommand("aps", save_params);
  sCmd.addCommand("apc", change_param);
#endif

  sCmd.addCommand("cl", toggleCompassLearn);

  sCmd.addCommand("vrs", printVersion);

  sCmd.addCommand("jyc", ManCmdAxisCalibration);
  sCmd.addCommand("jyo", toggleJoyOutput);


  sCmd.addCommand("gsc", gyroSaveCalibration);
  sCmd.addCommand("grc", gyroReadCalibration);


  sCmd.addCommand("ac", accRecalibrate);
  sCmd.addCommand("asc", accSaveCalibration);
  sCmd.addCommand("arc", accReadCalibration);

  sCmd.addCommand("+", yawRotateRight);
  sCmd.addCommand("-", yawRotateLeft);

  sCmd.addCommand("tmm", testMotorMovement);
  sCmd.addCommand("owf", outputWaveForm);

  sCmd.addCommand("he", helpMe);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
}

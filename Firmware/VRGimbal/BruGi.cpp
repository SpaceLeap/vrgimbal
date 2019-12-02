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
*/
/*
 * VRGimbal code is based on the project:
 *        Brushless Gimbal Controller Software by Christian Winkler and Alois Hahn (C) 2013
 *        and
 *        APM:Copter HTTP:///www.ardupilot.com
 *
 * 	Project lead developer and creator: Matteo Murtas - LaserNavigation s.r.l.
 *
 * 	Developers:
 * 	Roberto Navoni
 * 	Emile Castelnuovo
 *
 * 	Please refer to http://vrgimbal.wordpress.com for more information
 *
 * 	Thanks to:
 * 	LeafLabs (www.leaflabs.com) for Wirish Libraries
 * 	APM dev team
 *
*/


//#define DEBUG_IMU_DELAY

/*************************/
/* Include Header Files  */
/*************************/

#include <main.h>
#include "EEPROMAnything.h"

// this sets up the parameter table, and sets the default values. This
// must be the first AP_Param variable declared to ensure its
// constructor runs before the constructors of the other AP_Param
// variables
AP_Param param_loader; //(var_info, BOARD_EEPROM_SIZE, &EEPROM);

FastSerial  SerialDBG;
#ifdef GIMBAL_ENABLE_USB
USBSerial SerialUSB;
#endif
FastSerial *cliSerial = &SerialDBG;

HardwareSPI spi2(2);
HardwareI2C i2c2(2);
SerialCommand sCmd(cliSerial);     // Create SerialCommand object

#ifdef GIMBAL_ENABLE_COMPASS
Compass_HMC5843      	compass(&i2c2, cliSerial);
#endif
#if defined ( IMU_BRUGI )
MPU6050 mpu(&i2c2);            // Create MPU object
MPU6050 mpu_yaw(&i2c2);        // Create MPU object
bool mpu_yaw_present = false;


void initIMU_LPF()
{
	initMPUlpf(&mpu);
	if (mpu_yaw_present)
		initMPUlpf(&mpu_yaw);
}

void initMPUlpf(MPU6050 * p_mpu) {
  // Set Gyro Low Pass Filter(0..6, 0=fastest, 6=slowest)
  switch (config.profiles[0].mpuLPF) {
    case 0:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_256);  break;
    case 1:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_188);  break;
    case 2:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_98);   break;
    case 3:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_42);   break;
    case 4:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_20);   break;
    case 5:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_10);   break;
    case 6:  p_mpu->setDLPFMode(MPU6050_DLPF_BW_5);    break;
    default: p_mpu->setDLPFMode(MPU6050_DLPF_BW_256);  break;
  }
}
#endif

#if defined ( IMU_AP )


#ifdef GIMBAL_ENABLE_COMPASS
AP_Compass_HMC5843      	compass(&i2c2, cliSerial);
#endif

Arduino_Mega_ISR_Registry 	isr_registry;
AP_TimerProcess  scheduler;
static GPS							*g_gps_null = NULL;
#ifdef GIMBAL_IMU_SPI
AP_InertialSensor_MPU6000 	ins(  &spi1,BOARD_CS_IMU_SPI, false, BOARD_INT_IMU_SPI, &Serial);
#else
AP_InertialSensor_MPU6000_I2C	ins(&i2c2, BOARD_INT_IMU, cliSerial, MPU6050_ADDRESS_AD0_LOW);
#endif
AP_AHRS_DCM					ahrs(&ins, g_gps_null);





void initMPUlpf() {

}
#endif


#ifdef GIMBAL_ENABLE_RC
#ifdef RC_USE_LIBS
APM_RC_MP32V3 				APM_RC;
#endif
#endif

/**********************************************/
/* Initialization                             */
/**********************************************/
void clearEEPROM()
{
	int ee = BRUGI_CONFIG_EEPROM_POS;
    unsigned int i;
    char b = 0;
    for (i = BRUGI_CONFIG_EEPROM_POS; i < EEPROM_PAGE_SIZE; i++)
    {
          EEPROM.write(ee++, b);
    }

}

bool read_config()
{
	//EEPROM_readAnything(BRUGI_CONFIG_EEPROM_POS, config);
	byte crc_read = 0;
	EEPROM_readAnything(BRUGI_CONFIG_EEPROM_POS, config, &crc_read);

	crc calculated = crcSlow((crc *)&config, sizeof(config));
	cliSerial->printf(F("Save Config. CRC: %02x RD: %02x CALC: %02x  Size: %d\r\n"), 0, crc_read, calculated, sizeof(config));

	//if (config.crc8 == calculated)
	if (crc_read == calculated)
	{
		return true;
	} else {
		// crc failed intialize directly here, as readEEPROM is void
		cliSerial->println(F("EEPROM CRC failed, initialize EEPROM"));
		setDefaultParameters();
		write_config();
	}
	return false;
}
void write_config()
{
	//EEPROM_writeAnything(BRUGI_CONFIG_EEPROM_POS, config);

	bool old = g_accOutput;
	g_accOutput = false; // do not save enabled OAC output mode
	crc new_crc = crcSlow((crc *)&config, sizeof(config)); // set proper CRC

	cliSerial->printf(F("Save Config. CRC: %02x\r\n"), new_crc);

	clearEEPROM();

	EEPROM_writeAnything(BRUGI_CONFIG_EEPROM_POS, config, &new_crc);
	g_accOutput = old;

}

void dummy(void)
{}

void setup() 
{

	// just for debugging
	#ifdef STACKHEAPCHECK_ENABLE
	stackCheck();
	heapCheck();
	#endif

	LEDPIN_PINMODE
	LEDGREPIN_PINMODE

	LEDGREPIN_ON


	//CH2_PINMODE
	//CH3_PINMODE

	// Start Serial Port
	SerialDBG.configure(SERIAL_CLI_PORT);
	SerialDBG.begin(SERIAL_CLI_BAUD, 0x01, 0x07);

#ifdef GIMBAL_ENABLE_USB
	SerialUSB.begin();

	if (SerialUSB.isConnected())
		cliSerial = (FastSerial*)&SerialUSB;
#endif

	cliSerial->print("\r\n\r\nLASER NAVIGATION SRL\r\n\r\n");
	cliSerial->printf("\r\nInit %s|%s %s\r\n", THISFIRMWARE, __DATE__, __TIME__);
	cliSerial->printf("System Clock: %lu MHz\r\n", SystemCoreClock/1000000);

	// Set Serial Protocol Commands
	setSerialProtocol();

	// Start I2C and Configure Frequency
	cliSerial->print("Init I2C\r\n");
	i2c2.begin();


	cliSerial->print("Init SPI2\r\n");
	spi2.begin(SPI_1_125MHZ, MSBFIRST, 0);
	EEPROM.init(&spi2, BOARD_CS_EEPROM); //, true, 9, 512, 512);

#ifdef ENABLE_AP_PARAM

	param_loader.setup(var_info, BRUGI_CONFIG_EEPROM_POS, &EEPROM);
	param_loader.load_defaults();
	load_parameters();
#endif

  // Read Config, fill with default settings if versions do not match or CRC fails
	  read_config();
  //if ((config.vers != VERSION) || (config.versEEPROM != VERSION_EEPROM))
  if  (config.versEEPROM != VERSION_EEPROM)
  {
    cliSerial->print(F("EEPROM version mismatch, initialize EEPROM"));
    setDefaultParameters();
    write_config();
  }
  
  // Init Sinus Arrays and Motor Stuff
  //superfluo, lo faccio in initmotors
  //recalcMotorStuff();
  
  // Init PIDs to reduce floating point operations.
  initPIDs();


#ifdef GIMBAL_ENABLE_RC
  // init RC variables
  initRC();
#endif

 
   // Init BL Controller
  //initBlController();
  // Initialize MPU 
  initResolutionDevider();
  
  // Init IMU variables
  initIMU();
  
#if defined ( IMU_BRUGI )


//  // Auto detect MPU address
//  mpu.setAddr(MPU6050_ADDRESS_AD0_HIGH);
//  mpu.initialize();
//  if(mpu.testConnection()) {
//	  cliSerial->println(F("MPU6050 ok (HIGH)"));
//  } else {
//    mpu.setAddr(MPU6050_ADDRESS_AD0_LOW);
//    mpu.initialize();
//    if(mpu.testConnection()) {
//    	cliSerial->println(F("MPU6050 ok (LOW)"));
//    } else {
//    	cliSerial->println(F("MPU6050 failed"));
//    }
//  }

  bool bHighOk = false;
  bool bLowOk = false;
  mpu.setAddr(MPU6050_ADDRESS_AD0_HIGH);
  mpu.initialize();
  if(mpu.testConnection())
	  bHighOk = true;

  mpu.setAddr(MPU6050_ADDRESS_AD0_LOW);
    mpu.initialize();
    if(mpu.testConnection())
  	  bLowOk = true;

    if (bHighOk && bLowOk)
    {
    	mpu.setAddr(MPU6050_ADDRESS_AD0_LOW);
    	mpu.initialize();

    	mpu_yaw.setAddr(MPU6050_ADDRESS_AD0_HIGH);
    	mpu_yaw.initialize();
    	mpu_yaw_present = true;

    	cliSerial->println(F("MPU6050 ok (DOUBLE SENSOR)"));

    } else if (bHighOk) {
    	mpu.setAddr(MPU6050_ADDRESS_AD0_HIGH);
    	mpu.initialize();
    	cliSerial->println(F("MPU6050 ok (HIGH)"));
    } else if (bLowOk) {
		mpu.setAddr(MPU6050_ADDRESS_AD0_LOW);
		mpu.initialize();
		cliSerial->println(F("MPU6050 ok (LOW)"));
	} else {
		cliSerial->println(F("MPU6050 failed"));
	}

#endif

  //CH2_ON
  
  // set sensor orientation (from config)
  initSensorOrientation();
  
  gyroReadCalibration();

#if defined ( IMU_BRUGI )
  // Init MPU Stuff
  mpu.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);          // Set Clock to ZGyro
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS);           // Set Gyro Sensitivity to config.h
  //mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);       //+- 2G
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);       //+- 8G
  initMPUlpf(&mpu);                                         // Set Gyro Low Pass Filter
  mpu.setRate(0);                                       // 0=1kHz, 1=500Hz, 2=333Hz, 3=250Hz, 4=200Hz
  mpu.setSleepEnabled(false); 
  
  //mpu.setIntDataReadyEnabled()

  if (config.recalibrateOnStartup)
  {
	  // Gyro Offset calibration
	  cliSerial->println(F("Gyro calibration: do not move"));
	  mpu.setDLPFMode(MPU6050_DLPF_BW_5);  // experimental AHa: set to slow mode during calibration
	  gyroOffsetCalibration(&mpu, gyroOffset);
	  initMPUlpf(&mpu);
	  cliSerial->println(F("Gyro calibration: done"));
  }
  if (mpu_yaw_present)
  {
	mpu_yaw.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);          // Set Clock to ZGyro
	mpu_yaw.setFullScaleGyroRange(MPU6050_GYRO_FS);           // Set Gyro Sensitivity to config.h
	//mpu_yaw.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);       //+- 2G
	mpu_yaw.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);       //+- 8G
	initMPUlpf(&mpu_yaw);                                         // Set Gyro Low Pass Filter
	mpu_yaw.setRate(0);                                       // 0=1kHz, 1=500Hz, 2=333Hz, 3=250Hz, 4=200Hz
	mpu_yaw.setSleepEnabled(false);

	  if (config.recalibrateOnStartup)
	  {
		// Gyro Offset calibration
		cliSerial->println(F("Second Gyro calibration: do not move"));
		mpu_yaw.setDLPFMode(MPU6050_DLPF_BW_5);  // experimental AHa: set to slow mode during calibration
		gyroOffsetCalibration(&mpu_yaw, gyroOffset2);
		initMPUlpf(&mpu_yaw);
		cliSerial->println(F("Second Gyro calibration: done"));
	  }
  }

#endif
  
#ifdef IMU_EVV
  cliSerial->println(F("Initing position: do not move."));
  EVV_Init_Orientation();
#endif

  LEDPIN_ON
  
   // Init BL Controller
  initBlController();

  if (config.profiles[0].axisConfig[axisYAW].mode != 0)
  {
	  cliSerial->println(F("Positioning Yaw"));
	  switchOffAndMoveToPosition(config.profiles[0].axisConfig[axisYAW].motorNumber , config.profiles[0].axisConfig[axisYAW].maxPWM, config.profiles[0].axisConfig[axisYAW].offsetMotor, 3000);
  }

  if (config.profiles[0].axisConfig[axisROLL].mode != 0)
  {
	  cliSerial->println(F("Positioning Roll"));
	  switchOffAndMoveToPosition(config.profiles[0].axisConfig[axisROLL].motorNumber , config.profiles[0].axisConfig[axisROLL].maxPWM, config.profiles[0].axisConfig[axisROLL].offsetMotor, 3000);
  }

  if (config.profiles[0].axisConfig[axisPITCH].mode != 0)
  {
	  cliSerial->println(F("Positioning Pitch"));
	  switchOffAndMoveToPosition(config.profiles[0].axisConfig[axisPITCH].motorNumber , config.profiles[0].axisConfig[axisPITCH].maxPWM, config.profiles[0].axisConfig[axisPITCH].offsetMotor, 3000);
  }

  // motorTest();

#ifdef GIMBAL_ENABLE_RC
  // Init RC-Input
  initRCPins();
#endif

  initManualControllers();

  LEDGREPIN_OFF
  
  cliSerial->println(F("GO! Type HE for help, activate NL in Arduino Terminal!"));

  //CH2_OFF
  //CH3_OFF

  //TEO 20130607
  gimState = GIM_IDLE;
  stateStart = millis();
 
}

/************************/
/* PID Controller       */
/************************/
int32_t ComputePID(int32_t DTms, int32_t in, int32_t setPoint, int32_t *errorSum, int32_t *errorOld, int32_t Kp, int16_t Ki, int32_t Kd, int32_t * Ddelta1, int32_t * Ddelta2)
{
  int32_t error = setPoint - in;
  int32_t Ierr;
   
  Ierr = error * Ki * DTms / 1000; //TEO 20131002 divido Ki qui anzich in lettura/scrittura dalla flash
  Ierr = constrain_int32(Ierr, -(int32_t)1000*100, (int32_t)1000*100);
  *errorSum += Ierr;


#ifdef GIMBAL_DTERM_LOWPASS
#ifdef pt1cut
    float dt = DTms / 1000;
#endif
    // D Term
    int32_t delta = error - *errorOld;
    *errorOld = error;
    int32_t deltaSum = *Ddelta1 + *Ddelta2 + delta;
    *Ddelta2   = *Ddelta1;
    *Ddelta1   = delta;
#ifdef pt1cut
    deltaSum        = LastDterm[axis] + (dt / (pt1cut + dt)) * (deltaSum - LastDterm[axis]); // pt1 element like shown by BRM here: http://www.multiwii.com/forum/viewtopic.php?f=23&t=2624
    LastDterm[axis] = deltaSum;
#endif
    int32_t Dterm = (deltaSum * Kd) / DTms;
#else
    int32_t Dterm = Kd * (error - *errorOld) * DTms;
#endif

  /*Compute PID Output*/
  int32_t out = (Kp * error) + *errorSum + Dterm;
  *errorOld = error;

  return out / 4096;
}

float ComputePID_float(float DTms, float in, float setPoint, float *errorSum, float *errorOld, float Kp, float Ki, float Kd, float * Ddelta1, float * Ddelta2)
{
	float error = setPoint - in;
	float Ierr;

  Ierr = error * Ki * DTms;
  Ierr = constrain(Ierr, -100000.0f, 100000.0f);
  *errorSum += Ierr;


#ifdef GIMBAL_DTERM_LOWPASS
#ifdef pt1cut
    float dt = DTms / 1000;
#endif
    // D Term
    float delta = error - *errorOld;
    *errorOld = error;
    float deltaSum = *Ddelta1 + *Ddelta2 + delta;
    *Ddelta2   = *Ddelta1;
    *Ddelta1   = delta;
#ifdef pt1cut
    deltaSum        = LastDterm[axis] + (dt / (pt1cut + dt)) * (deltaSum - LastDterm[axis]); // pt1 element like shown by BRM here: http://www.multiwii.com/forum/viewtopic.php?f=23&t=2624
    LastDterm[axis] = deltaSum;
#endif
    float Dterm = (deltaSum * Kd) / DTms;
#else
    float Dterm = Kd * (error - *errorOld) * DTms;
#endif

  /*Compute PID Output*/
  float out = (Kp * error) + *errorSum + Dterm;
  *errorOld = error;

  return out / 4096;
}

float fComputePID(float DTs, float in, float setPoint, float *errorSum, float *errorOld, float Kp, float Ki, float Kd, float * Ddelta1, float * Ddelta2)
{

	Kp = Kp / 1000.0f;
	//Ki = Ki / 1000.0f;
	Kd = Kd / 1000.0f;

	float error = setPoint - in;
	float Ierr;

  Ierr = error * Ki * DTs;
  Ierr = constrain(Ierr, -100.0f, 100.0f);
  *errorSum += Ierr;


#ifdef GIMBAL_DTERM_LOWPASS

  	float delta = error - *errorOld;
    *errorOld = error;
    float deltaSum = *Ddelta1 + *Ddelta2 + delta;
    *Ddelta2   = *Ddelta1;
    *Ddelta1   = delta;
#ifdef pt1cut
    deltaSum        = LastDterm[axis] + (DTs / (pt1cut + DTs)) * (deltaSum - LastDterm[axis]); // pt1 element like shown by BRM here: http://www.multiwii.com/forum/viewtopic.php?f=23&t=2624
    LastDterm[axis] = deltaSum;
#endif
    float Dterm = (deltaSum * Kd) / DTs;
#else
    float Dterm = Kd * (error - *errorOld) * DTs;
#endif

  /*Compute PID Output*/
    float out = (Kp * error) + *errorSum + Dterm;
  *errorOld = error;

  return out / 4096.0f;
}


/**********************************************/
/* Main Loop                                  */
/**********************************************/

int getMotorDriveSet(uint8_t axis)
{
	switch (axis)
	{
	case axisROLL:
		return  (int) ( (rollAngleSet / 360.0f) * (float) config.profiles[0].axisConfig[axisROLL].stepsMotor ) + config.profiles[0].axisConfig[axisROLL].offsetMotor;
		break;
	case axisPITCH:
		return  (int) ( (pitchAngleSet / 360.0f) * (float) config.profiles[0].axisConfig[axisPITCH].stepsMotor ) + config.profiles[0].axisConfig[axisPITCH].offsetMotor;
		break;
	case axisYAW:
		return  (int) ( (yawAngleSet / 360.0f) * (float) config.profiles[0].axisConfig[axisYAW].stepsMotor ) + config.profiles[0].axisConfig[axisYAW].offsetMotor;
		break;
	}
	return 0;
}

bool checkEsc()
{
	if (cliSerial->available())
	{
		int ch = cliSerial->read();
		if (ch == 0x1B)
			return true;
	}
	return false;
}

void print_vector(Vector3i v)
{
	cliSerial->print(v.x);cliSerial->print(F(" "));
	cliSerial->print(v.y);cliSerial->print(F(" "));
	cliSerial->print(v.z);cliSerial->print(F(" "));
}


void print_vector(Vector3f v)
{
	cliSerial->print(v.x);cliSerial->print(F(" "));
	cliSerial->print(v.y);cliSerial->print(F(" "));
	cliSerial->print(v.z);cliSerial->print(F(" "));
}

uint32 superfast_loopTimer = 0;
uint32 fast_loopTimer = 0;
uint32 superfastloop_speed = 1; // SUPER_FAST_MICROSECONDS;
uint32 fastloop_speed = 1; // 5; //FAST_MICROSECONDS;


uint32 motor_loopTimer = 0;

#ifdef DEBUG_IMU_DELAY
//debug velocit di lettura i2c
uint32_t lap_gyros = 0;
uint32_t cnt_gyros = 0;
#endif


int32_t rollAngleSet_OLD = 0;
int32_t pitchAngleSet_OLD = 0;
int32_t yawAngleSet_OLD = 0;


void measure_loop(bool bAppend = true)
{
	//misuro intervallo tra le chiamate
	uint32_t unewloop = micros();
	static uint32_t ulast_motor_loop = 0;
	if (ulast_motor_loop != 0)
	{
		uint32_t lap2 = measure_micro_delay(ulast_motor_loop, unewloop);
		if (bAppend)
			loop_mean_lap.append((float) lap2);
	}
	ulast_motor_loop = unewloop;
}

int filterMotorDrive(int oldDrive, int newDrive, int limit)
{


	/*
	float n = (float) newDrive;
	float o = (float) oldDrive;
	//utilLP_float(&o, n, 1.0f / (float) (g_outputFilter + 1));

	float coeff = 1.0f / (float) (g_outputFilter + 1);

	 n = o * (1.0f-coeff) + n * coeff;

	return (int) n;*/

	int n = newDrive;

	if (limit > 0)
	{
		if (newDrive > (oldDrive + limit))
		{
			n = oldDrive + limit;
		} else if (newDrive < (oldDrive - limit))
		{
			n = oldDrive - limit;
		}
	}
	return n;
}


realtimeStatistics statistic_pitch;
realtimeStatistics statistic_roll;
realtimeStatistics statistic_yaw;

void loop()
{

//	//misuro intervallo tra le chiamate
//	uint32_t unewloop = micros();
//	static uint32_t ulast_motor_loop = 0;
//	if (ulast_motor_loop != 0)
//	{
//		uint32_t lap2 = measure_micro_delay(ulast_motor_loop, unewloop);
//		loop_mean_lap.append((float) lap2);
//	}
//	ulast_motor_loop = unewloop;

	//measure_loop();

	int32_t pitchPIDVal;
	int32_t rollPIDVal;
	int32_t yawPIDVal;
	static int32_t pitchErrorSum;
	static int32_t rollErrorSum;
	static int32_t yawErrorSum;
	static int32_t pitchErrorOld;
	static int32_t rollErrorOld;
	static int32_t yawErrorOld;


	static int32_t pitchDelta1 = 0;
	static int32_t rollDelta1 = 0;
	static int32_t yawDelta1 = 0;
	static int32_t pitchDelta2 = 0;
	static int32_t rollDelta2 = 0;
	static int32_t yawDelta2 = 0;

	int deltaDrive[3] = {0, 0, 0};


	int yawDiffDrive = 0;
	float gyroDesiredYaw = 0;
	float diffDriveAngleYaw = 0;
	int driveSetYaw = 0;


	static char pOutCnt = 0;
	static uint32_t output_time = 0;

	//static int stateCount = 0;
  


#ifdef IMU_AP
//	//mi affido all'IMU_INT per evitare troppe letture
//	ins.read();
//	if (ins.num_samples_available() >= 1)
//		ahrs.update();


	static uint32_t superfast_loopTimer = 0;
	const uint32_t superfastloop_speed = 400;
	static uint32_t fast_loopTimer = 0;
	const uint32_t fastloop_speed = 2000; //4000;

    uint32_t micr = micros();

    if (micr - superfast_loopTimer >= superfastloop_speed) // 250 MPU6000 500 VRIMU
    {
        superfast_loopTimer = micr;
        //insert here all routines called by timer_scheduler
        //superfast_loop();
        ins.read();
    }

    uint32_t timer	= micros();

	// We want this to execute fast
	// ----------------------------
    if ((timer - fast_loopTimer) >= fastloop_speed && (ins.num_samples_available() >= 1)) {
    	fast_loopTimer          = timer;
    	ahrs.update();
    }

#endif


#ifdef BRUGI_USE_INTERRUPT_TIMER
	if (motorUpdate) // loop runs with motor ISR update rate (1000Hz)
	{
#endif
		uint32_t now = millis();
		//uint32_t pid_lap = now - motor_loopTimer;

		uint32_t unow = micros();
		static uint32_t u_last_motor_update = 0;

		float pid_lap = ((float) measure_micro_delay( u_last_motor_update, unow) / 1000.0f);
		u_last_motor_update = unow;

		bool bEnterPID = false;
		bool bOutputDebug = false;

		if ( pid_lap >= DT_LOOP_MS)
		{
			//measure_loop();

			bEnterPID = true;

			float pid_lap_sec = pid_lap / 1000.0f;

			if (pid_lap > 10 * DT_LOOP_MS)
				pid_lap = DT_LOOP_MS;

			motor_loopTimer = now;
			motorUpdate = false;
    
   
			// update IMU data
#ifdef DEBUG_IMU_DELAY
			uint32_t t1 = micros();
#endif
			readGyros();

#ifdef DEBUG_IMU_DELAY
			lap_gyros += (micros() - t1);
			cnt_gyros++;
#endif
    
			if (config.profiles[0].enableGyro) updateGyroAttitude();
			if (config.profiles[0].enableACC) updateACCAttitude();
#ifdef GIMBAL_ENABLE_COMPASS
			if (config.profiles[0].enableMAG) updateMAGAttitude();
#endif

			getAttiduteAngles();
    
#ifdef GIMBAL_ENABLE_RC
			// Evaluate RC-Signals
			evaluateRC();

			if (config.profiles[0].rcConfig[axisPITCH].absolute) {
				utilLP_float(&pitchAngleSet, PitchPhiSet, rcLPF_tc[axisPITCH]);
			} else if (PitchResetting) {
				utilLP_float(&pitchAngleSet, 0, rcLPF_tc[axisPITCH]);
				PitchPhiSet = pitchAngleSet;
			} else {
				utilLP_float(&pitchAngleSet, PitchPhiSet, 0.01);

			}

//			cliSerial->print(rollAngleSet);
//			cliSerial->print(" ");
			if (config.profiles[0].rcConfig[axisROLL].absolute) {
//				cliSerial->print("A ");
				utilLP_float(&rollAngleSet, RollPhiSet,  rcLPF_tc[axisROLL]);
			} else if  (RollResetting) {
				utilLP_float(&rollAngleSet, 0, rcLPF_tc[axisROLL]);
				RollPhiSet = rollAngleSet;
//				cliSerial->print("B ");
			} else {
				utilLP_float(&rollAngleSet, RollPhiSet, 0.01f);
//				cliSerial->print("C ");
			}
//			cliSerial->print(rollAngleSet);
//			cliSerial->print(" ");
//			cliSerial->print(RollPhiSet);
//			cliSerial->println();

			if (config.profiles[0].rcConfig[axisYAW].absolute) {
				utilLP_float(&yawAngleSet, YawPhiSet, rcLPF_tc[axisYAW]);
			} else if (YawResetting) {
				utilLP_float(&yawAngleSet, 0, rcLPF_tc[axisYAW]);
				YawPhiSet = yawAngleSet;
			} else {
				utilLP_float(&yawAngleSet, YawPhiSet, 0.01);
			}


#endif

			int new_pitchMotorDrive = pitchMotorDrive;
			int new_rollMotorDrive = rollMotorDrive;
			int new_yawMotorDrive = yawMotorDrive;
			//****************************
			// pitch PID
			//****************************
			float gyroDesiredPitch = 0;
			if (config.profiles[0].axisConfig[axisPITCH].mode == 0)
			{
				//pitchPIDVal = ComputePID(DT_LOOP_MS, angle[axisPITCH], pitchAngleSet*1000, &pitchErrorSum, &pitchErrorOld, pitchPIDpar.Kp, pitchPIDpar.Ki, pitchPIDpar.Kd, &pitchDelta1, &pitchDelta2);
				//pitchPIDVal = fComputePID(pid_lap_sec, angle[axisPITCH3] / 1000.0f, pitchAngleSet, &pitchErrorSum, &pitchErrorOld, pitchPIDpar.Kp, pitchPIDpar.Ki, pitchPIDpar.Kd, &pitchDelta1, &pitchDelta2);
				pitchPIDVal = ComputePID(pid_lap, angle[axisPITCH], pitchAngleSet*1000, &pitchErrorSum, &pitchErrorOld, pitchPIDpar.Kp, pitchPIDpar.Ki, pitchPIDpar.Kd, &pitchDelta1, &pitchDelta2);
				// motor control
				new_pitchMotorDrive = pitchPIDVal * config.profiles[0].axisConfig[axisPITCH].motorDirection;
			}
			else if (config.profiles[0].axisConfig[axisPITCH].mode == 1)
			{
				float dAngle = normalize_yaw( pitchAngleSet - pitchAngleSet_OLD );
				gyroDesiredPitch =  dAngle / ((float) pid_lap / 1000.0);  // /s

				pitchPIDVal = ComputePID(pid_lap,  (float) gyroADC[axisPITCH] / resolutionDevider, gyroDesiredPitch , &pitchErrorSum, &pitchErrorOld, pitchPIDpar.Kp, pitchPIDpar.Ki, pitchPIDpar.Kd, &pitchDelta1, &pitchDelta2);

				// motor control
				int drive = pitchPIDVal * pid_lap * config.profiles[0].axisConfig[axisPITCH].motorDirection;
				new_pitchMotorDrive = pitchMotorDrive + drive;
				if (drive != 0)
					pitchAngleSet_OLD = normalize_yaw(pitchAngleSet_OLD + (gyroDesiredPitch * pid_lap / 1000.0) );
			} else {
				new_pitchMotorDrive = (int) ((float) config.profiles[0].axisConfig[axisPITCH].stepsMotor * ((float)pitchAngleSet) / 360.0  + config.profiles[0].axisConfig[axisPITCH].offsetMotor);
			}

			//****************************
			// roll PID
			//****************************
			float gyroDesiredRoll= 0;

			if (config.profiles[0].axisConfig[axisROLL].mode == 0)
			{
				//rollPIDVal = ComputePID(DT_LOOP_MS, angle[axisROLL], rollAngleSet*1000, &rollErrorSum, &rollErrorOld, rollPIDpar.Kp, rollPIDpar.Ki, rollPIDpar.Kd, &rollDelta1, &rollDelta2);
				rollPIDVal = ComputePID(pid_lap, angle[axisROLL], rollAngleSet*1000, &rollErrorSum, &rollErrorOld, rollPIDpar.Kp, rollPIDpar.Ki, rollPIDpar.Kd, &rollDelta1, &rollDelta2);

				// motor control
				new_rollMotorDrive = rollPIDVal * config.profiles[0].axisConfig[axisROLL].motorDirection;
			}
			else if (config.profiles[0].axisConfig[axisROLL].mode == 1)
			{
				float dAngle = normalize_yaw( rollAngleSet - rollAngleSet_OLD );
				gyroDesiredRoll =  dAngle / ((float) pid_lap / 1000.0);  // /s

				rollPIDVal = ComputePID(pid_lap,  (float) gyroADC[axisROLL] / resolutionDevider, gyroDesiredRoll , &rollErrorSum, &rollErrorOld, rollPIDpar.Kp, rollPIDpar.Ki, rollPIDpar.Kd, &rollDelta1, &rollDelta2);

				// motor control
				int drive = rollPIDVal * pid_lap * config.profiles[0].axisConfig[axisROLL].motorDirection;
				new_rollMotorDrive = rollMotorDrive + drive;
				if (drive != 0)
					rollAngleSet_OLD = normalize_yaw(rollAngleSet_OLD + (gyroDesiredRoll * pid_lap / 1000.0) );
			} else {
				new_rollMotorDrive = (int) ((float) config.profiles[0].axisConfig[axisROLL].stepsMotor * ((float)rollAngleSet ) / 360.0 + config.profiles[0].axisConfig[axisROLL].offsetMotor);
			}

			//****************************
			// yaw PID
			//****************************
			//gestisco il cambio di modo in runtime azzerando lo storico degli errori
			static int8_t modeYaw_old = 0;
			if (modeYaw_old != config.profiles[0].axisConfig[axisYAW].mode)
			{
				yawErrorOld = 0;
				yawErrorSum = 0;
				yawDelta1 = 0;
				yawDelta2 = 0;

				modeYaw_old = config.profiles[0].axisConfig[axisYAW].mode;
			}


			if (g_bTestYawMotor)
			{
				g_fTestYawMotorValue += g_fTestYawMotorSpeed * pid_lap;
				new_yawMotorDrive = g_fTestYawMotorValue * config.profiles[0].axisConfig[axisYAW].motorDirection;
			} else if (config.profiles[0].axisConfig[axisYAW].mode == 0)
			{
				yawPIDVal = ComputePID(pid_lap, angle[axisYAW], yawAngleSet * 1000 , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);

				int yawMotorDrive_OLD = yawMotorDrive;
				// motor control
				new_yawMotorDrive = yawPIDVal * config.profiles[0].axisConfig[axisYAW].motorDirection;

				yawDiffDrive = new_yawMotorDrive - yawMotorDrive_OLD;


//				//test: limito la velocit di attuazione
//				float spd = (float) yawDiffDrive / pid_lap;
//				const float spdLimit = 1;  //1 step/ms
//				if (fabs(spd) > spdLimit)
//				{
//					new_yawMotorDrive = yawMotorDrive_OLD + (int)sgn(yawDiffDrive) * (int) (spdLimit * pid_lap);
//				}

				yawAngleSet_OLD = yawAngleSet;
			}
			else if (config.profiles[0].axisConfig[axisYAW].mode == 1)
			{

//				float dAngle = normalize_yaw( yawAngleSet - yawAngleSet_OLD ); // estimAngle[axisYAW]; //;
//				gyroDesiredYaw =  dAngle / ((float) pid_lap / 1000.0);  // /s
//
//				yawPIDVal = ComputePID(pid_lap,  (float) gyroADC[axisYAW] / resolutionDevider, gyroDesiredYaw , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);
//
//				// motor control
//				int drive = yawPIDVal * pid_lap * config.profiles[0].dirMotorYaw;
//				new_yawMotorDrive = yawMotorDrive + drive;
//				if (drive != 0)
//					yawAngleSet_OLD = normalize_yaw(yawAngleSet_OLD + (gyroDesiredYaw * pid_lap / 1000.0) );

//				//esperimento di stabilizzazione con controllo della velocit di rotazione (una specie di doppia retroazione)
//				diffDriveAngleYaw = yawAngleSet - angle[axisYAW] / 1000.0f;
//				diffDriveAngleYaw = normalize_yaw(diffDriveAngleYaw);
//
//				float driveLimit1 = 0.0f; //(float) config.profiles[0].driveLimit1Angle;
//				float driveLimit2 = (float) config.profiles[0].driveLimit2Angle;
//				float maxGyro = (float) config.profiles[0].maxGyroDrive;
//
//				float newGyro = 0;
//
//				if (diffDriveAngleYaw > driveLimit2)
//					newGyro = maxGyro ;
//				else if (diffDriveAngleYaw > driveLimit1)
//					newGyro = maxGyro * (float)(diffDriveAngleYaw - driveLimit1) / (float) (driveLimit2 - driveLimit1);
//				else if (diffDriveAngleYaw < -driveLimit2)
//					newGyro = - maxGyro;
//				else if (diffDriveAngleYaw < -driveLimit1)
//					newGyro = - maxGyro * (float)(abs(diffDriveAngleYaw) - driveLimit1) / (float) (driveLimit2 - driveLimit1);
//				else
//					newGyro = 0;
//
//				utilLP_float(&gyroDesiredYaw, newGyro, 0.01);

				gyroDesiredYaw = yawAngleSet / 10;


				yawPIDVal = ComputePID(pid_lap, 1000.0f * (float) gyroADC[axisYAW] / resolutionDevider, 1000.0f * gyroDesiredYaw , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);
				// motor control
				//int drive = yawPIDVal * pid_lap * config.profiles[0].dirMotorYaw;
				int drive = yawPIDVal * config.profiles[0].axisConfig[axisYAW].motorDirection;
				new_yawMotorDrive = yawMotorDrive + drive;


//				yawPIDVal = ComputePID(pid_lap,  (float) angle[axisYAW], gyroDesiredYaw * pid_lap, &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);
//				// motor control
//				new_yawMotorDrive = yawPIDVal * config.profiles[0].dirMotorYaw;

			}
			else if ( (config.profiles[0].axisConfig[axisYAW].mode == 2) ) //|| (config.profiles[0].axisConfig[axisYAW].mode == 3))
			{

				driveSetYaw = getMotorDriveSet(axisYAW);//(int) ( (yawAngleSet / 360.0f) * (float) config.profiles[0].stepsMotorYaw ) + config.profiles[0].offsetMotorYaw;
				driveSetYaw = inormalize_val(driveSetYaw, config.profiles[0].axisConfig[axisYAW].stepsMotor);

				int diff_drive = driveSetYaw - yawMotorDrive;
				diff_drive = inormalize_val(diff_drive, config.profiles[0].axisConfig[axisYAW].stepsMotor);

				diffDriveAngleYaw = 360.0f * (float) diff_drive / (float) config.profiles[0].axisConfig[axisYAW].stepsMotor;


				int driveLimit1 = ((float) config.profiles[0].axisConfig[axisYAW].driveLimit1Angle / 360.0f) * config.profiles[0].axisConfig[axisYAW].stepsMotor;
				int driveLimit2 = ((float) config.profiles[0].axisConfig[axisYAW].driveLimit2Angle / 360.0f) * config.profiles[0].axisConfig[axisYAW].stepsMotor;
				float maxGyro = (float) config.profiles[0].axisConfig[axisYAW].maxGyroDrive;  //in realt sarebbe da legare a rcGain e rcLPF



				//3. formula lineare con tolleranza intorno allo zero
				//
				//    	           maxGyro
				//    	\    |    /
				//    	 \   |   /
				//    	  \--+--/
				//    	     0 L1 L2

				//if (config.profiles[0].axisConfig[axisYAW].mode == 3)
				//{
					if (diff_drive > driveLimit2)
						gyroDesiredYaw = maxGyro ;
					else if (diff_drive > driveLimit1)
						gyroDesiredYaw = maxGyro * (float)(diff_drive - driveLimit1) / (float) (driveLimit2 - driveLimit1);
					else if (diff_drive < -driveLimit2)
						gyroDesiredYaw = - maxGyro;
					else if (diff_drive < -driveLimit1)
						gyroDesiredYaw = - maxGyro * (float)(abs(diff_drive) - driveLimit1) / (float) (driveLimit2 - driveLimit1);
					else
						gyroDesiredYaw = 0;
				//}


				//TODO: TEO: verificare quale moltiplicazione di config.profiles[0].dirMotorYaw *  genera l'inverzione di segno
				gyroDesiredYaw = config.profiles[0].axisConfig[axisYAW].motorDirection * gyroDesiredYaw;

				yawPIDVal = ComputePID(pid_lap,  (float) gyroADC[axisYAW] / resolutionDevider, gyroDesiredYaw , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);
				// motor control
				int drive = yawPIDVal * pid_lap * config.profiles[0].axisConfig[axisYAW].motorDirection;

//				//TEO 20130917
//				//anzich pilotare la velocit angolare provo  settare l'angolo obiettivo come per la stabilize
//				float angle_to_reach = angle[axisYAW] + gyroDesiredYaw * pid_lap;
//				yawPIDVal = ComputePID(pid_lap,  angle[axisYAW], angle_to_reach , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);
//				// motor control
//				int drive = yawPIDVal * config.profiles[0].dirMotorYaw;


				if (config.profiles[0].axisConfig[axisYAW].mode == 3)
				{
					if ((abs(diff_drive) < driveLimit1) && (yawAngleSet == YawPhiSet))
					{
						if (fabs((float) gyroADC[axisYAW] / resolutionDevider) < maxGyro)
						{
							//ho gi raggiunto l'obiettivo: tengo fermo
							drive = 0;
						} else {
							//applico la stabilizzazione perch sto ruotando troppo veloce (quindi qualcuno sta ruotando bruscamente il supporto)
						}
					}
				}

				new_yawMotorDrive = yawMotorDrive + drive;
				new_yawMotorDrive = inormalize_val(new_yawMotorDrive, config.profiles[0].axisConfig[axisYAW].stepsMotor);

			}
			else if ( config.profiles[0].axisConfig[axisYAW].mode == 3)
			{
				driveSetYaw = getMotorDriveSet(axisYAW);//(int) ( (yawAngleSet / 360.0f) * (float) config.profiles[0].stepsMotorYaw ) + config.profiles[0].offsetMotorYaw;
				driveSetYaw = inormalize_val(driveSetYaw, config.profiles[0].axisConfig[axisYAW].stepsMotor);

				int diff_drive = driveSetYaw - yawMotorDrive;
				diff_drive = inormalize_val(diff_drive, config.profiles[0].axisConfig[axisYAW].stepsMotor);

				diffDriveAngleYaw = 360.0f * (float) diff_drive / (float) config.profiles[0].axisConfig[axisYAW].stepsMotor;

				float yaw_target = angle[axisYAW] + diffDriveAngleYaw * 1000;

				yawPIDVal = ComputePID(pid_lap, angle[axisYAW], yaw_target , &yawErrorSum, &yawErrorOld, yawPIDpar.Kp, yawPIDpar.Ki, yawPIDpar.Kd, &yawDelta1, &yawDelta2);

				// motor control
				new_yawMotorDrive = yawPIDVal * config.profiles[0].axisConfig[axisYAW].motorDirection;

			} else {
				new_yawMotorDrive = getMotorDriveSet(axisYAW);
			}

			//aggiorno i valori per il motor interrupt
			pitchMotorDrive_PREV = pitchMotorDrive;
			//pitchMotorDrive = new_pitchMotorDrive;
			pitchMotorDrive = filterMotorDrive(pitchMotorDrive, new_pitchMotorDrive, config.profiles[0].axisConfig[axisPITCH].stepsLimit);
			deltaDrive[axisPITCH] = pitchMotorDrive - pitchMotorDrive_PREV;
			pitchMotorDrive_INT_step = deltaDrive[axisPITCH] / LOOPUPDATE_FACTOR;
			statistic_pitch.append((float) deltaDrive[axisPITCH]);
			if ((g_driveAlert[axisPITCH] > 0) && (abs(deltaDrive[axisPITCH]) > g_driveAlert[axisPITCH]))
			{
				cliSerial->print("DRV!! ");cliSerial->print(deltaDrive[axisPITCH]);
				cliSerial->print(" P ");cliSerial->print(angle[axisPITCH]); cliSerial->println();
			}

			rollMotorDrive_PREV = rollMotorDrive;
			//rollMotorDrive = new_rollMotorDrive;
			rollMotorDrive = filterMotorDrive(rollMotorDrive, new_rollMotorDrive, config.profiles[0].axisConfig[axisROLL].stepsLimit);
			deltaDrive[axisROLL] = rollMotorDrive - rollMotorDrive_PREV;
			rollMotorDrive_INT_step = deltaDrive[axisROLL] / LOOPUPDATE_FACTOR;
			statistic_roll.append((float) deltaDrive[axisROLL]);
			if ((g_driveAlert[axisROLL] > 0) && (abs(deltaDrive[axisROLL]) > g_driveAlert[axisROLL]))
			{
				cliSerial->print("DRV!! ");cliSerial->print(deltaDrive[axisROLL]);
				cliSerial->print(" R ");cliSerial->print(angle[axisROLL]); cliSerial->println();
			}

			yawMotorDrive_PREV = yawMotorDrive;
			//yawMotorDrive = new_yawMotorDrive;
			yawMotorDrive = filterMotorDrive(yawMotorDrive, new_yawMotorDrive, config.profiles[0].axisConfig[axisYAW].stepsLimit);
			deltaDrive[axisYAW] = yawMotorDrive - yawMotorDrive_PREV;

			yawMotorDrive_INT_step = deltaDrive[axisYAW] / LOOPUPDATE_FACTOR;
			statistic_yaw.append((float) deltaDrive[axisYAW]);
			if ((g_driveAlert[axisYAW] > 0) && (abs(deltaDrive[axisYAW]) > g_driveAlert[axisYAW]))
			{
				cliSerial->print("DRV!! ");cliSerial->print(deltaDrive[axisYAW]);
				cliSerial->print(" Y ");cliSerial->print(angle[axisYAW]); cliSerial->println();
			}
			motor_update_values = true;

		}





#ifndef BRUGI_USE_INTERRUPT_TIMER
		//attuazione (spostata qui sotto in modo che sia più immediata e allineata con i LAP calcolati qui sopra)
		motorInterrupt();
#endif

		//****************************
		// debug and telemetry outputs
		//****************************
		//uint32_t now = millis();

		if ((now - output_time) > (1000 / POUT_FREQ))
		{
			output_time = now;
			bOutputDebug = true;

			if (g_bTest[7])
			{
				cliSerial->print(F("DRV "));
				cliSerial->print(deltaDrive[axisROLL]);cliSerial->print(F(" "));
				cliSerial->print(deltaDrive[axisPITCH]);cliSerial->print(F(" "));
				cliSerial->print(deltaDrive[axisYAW]);cliSerial->print(F(" "));
				cliSerial->println();
			}

			//debug stabilizzazione yaw
			if (g_bSendYawOutput)
			{

				cliSerial->print(F("YAW "));
				cliSerial->print(pid_lap);cliSerial->print(F(" "));
				cliSerial->print(YawPhiSet);cliSerial->print(F(" "));
				cliSerial->print(yawAngleSet);cliSerial->print(F(" "));
				cliSerial->print(yawAngleSet_OLD);cliSerial->print(F(" "));
				cliSerial->print(diffDriveAngleYaw);cliSerial->print(F(" "));
				cliSerial->print(gyroADC[axisYAW] / resolutionDevider);cliSerial->print(F(" "));
				cliSerial->print(gyroDesiredYaw);cliSerial->print(F(" "));
				cliSerial->print(driveSetYaw);cliSerial->print(F(" "));
				cliSerial->print(yawMotorDrive);cliSerial->print(F(" "));
				cliSerial->print(yawDiffDrive);cliSerial->print(F(" "));
				cliSerial->println();
				/*
				if (config.profiles[0].axisConfig[axisYAW].mode == 1)
				{
					cliSerial->print(F("YAW "));
					cliSerial->print(pid_lap);cliSerial->print(F(" "));
					cliSerial->print(yawAngleSet);cliSerial->print(F(" "));
					cliSerial->print(yawAngleSet_OLD);cliSerial->print(F(" "));
					cliSerial->print(gyroADC[axisYAW] / resolutionDevider);cliSerial->print(F(" "));
					cliSerial->print(gyroDesiredYaw);cliSerial->print(F(" "));
					cliSerial->print(yawMotorDrive);cliSerial->print(F(" "));
					cliSerial->println();
				} else if (config.profiles[0].axisConfig[axisYAW].mode > 1) {
					cliSerial->print(F("YAW "));
					cliSerial->print(pid_lap);cliSerial->print(F("\tAng "));
					cliSerial->print(yawAngleSet);cliSerial->print(F("\tPhi "));
					cliSerial->print(YawPhiSet);cliSerial->print(F("\tdiff "));
					cliSerial->print(diffDriveAngleYaw);cliSerial->print(F("\tgyro "));
					cliSerial->print(gyroADC[axisYAW] / resolutionDevider);cliSerial->print(F("\tdes "));
					cliSerial->print(gyroDesiredYaw);cliSerial->print(F("\tdriveSet "));
					cliSerial->print(driveSetYaw);cliSerial->print(F("\tmotor "));
					cliSerial->print(yawMotorDrive);cliSerial->print(F(" "));
					cliSerial->println();


				}*/
			}

#ifdef DEBUG_IMU_DELAY
			uint32_t dbg_gyros = lap_gyros;
			if (cnt_gyros > 0)
				dbg_gyros = dbg_gyros / cnt_gyros;

			cnt_gyros = 0;
			lap_gyros = 0;
#endif
			// 600 us
			if(g_accOutput){ cliSerial->print(angle[axisPITCH]);
										cliSerial->print(F(" ACC "));cliSerial->print(angle[axisROLL]);
										cliSerial->print(F(" "));cliSerial->print(angle[axisYAW]);
#ifdef DEBUG_IMU_DELAY
										cliSerial->print(F(" "));cliSerial->print(pid_lap);
										cliSerial->print(F(" "));cliSerial->print(dbg_gyros);
#endif
										cliSerial->print(F(" "));cliSerial->print(interrupt_mean_lap.mean());
										cliSerial->print(F(" "));cliSerial->print(interrupt_mean_duration.mean());
										cliSerial->print(F(" "));cliSerial->print(loop_mean_lap.mean());
										cliSerial->print(F(" "));cliSerial->print(loop_mean_lap.vmin());
										cliSerial->print(F(" "));cliSerial->print(loop_mean_lap.vmax());
										cliSerial->println();
			}

			if (g_bSendDebugOutput){
				cliSerial->print(F("DBG "));
				cliSerial->print(millis());cliSerial->print(F(" "));
				cliSerial->print(angle[axisROLL]);cliSerial->print(F(" "));
				cliSerial->print(angle[axisPITCH]);cliSerial->print(F(" "));
				cliSerial->print(angle[axisYAW]);cliSerial->print(F(" "));

				Vector3f dummy;
				dummy.x =0;
				dummy.y =0;
				dummy.z =0;

#if ((defined ( IMU_AP )) || ( defined (IMU_MIXED)))
				print_vector(ins.get_accel());
				print_vector(ins.get_gyro());
#ifdef GIMBAL_ENABLE_COMPASS
				cliSerial->print(compass.mag_x);cliSerial->print(F(" "));
				cliSerial->print(compass.mag_y);cliSerial->print(F(" "));
				cliSerial->print(compass.mag_z);cliSerial->print(F(" "));

#else
				print_vector(dummy);
#endif
#else

				Vector3f a;
				a.x = accADC[axisROLL];
				a.y = accADC[axisPITCH];
				a.z = accADC[axisYAW];
				print_vector(a);

				Vector3f g;
				g.x = gyroADC[axisROLL];
				g.y = gyroADC[axisPITCH];
				g.z = gyroADC[axisYAW];
				print_vector(g);
#ifdef GIMBAL_ENABLE_COMPASS
#ifdef COMPASS_AP
				cliSerial->print(compass.mag_x);cliSerial->print(F(" "));
				cliSerial->print(compass.mag_y);cliSerial->print(F(" "));
				cliSerial->print(compass.mag_z);cliSerial->print(F(" "));
#else
				Vector3i m = compass.get_raw();
				print_vector(m);
#endif
#else
				print_vector(dummy);
#endif

#endif
				cliSerial->println();
			}

			if (g_bSendRCOutput)
			{
				cliSerial->print("RC  "); cliSerial->print(now); cliSerial->print("\t");
				for (int i = 0; i < RC_DATA_SIZE; i++)
				{
					cliSerial->print(rcData[i].rx); cliSerial->print("\t");
				}

				cliSerial->print(rollAngleSet); cliSerial->print("\t");
				cliSerial->print(pitchAngleSet); cliSerial->print("\t");
				cliSerial->print(yawAngleSet); cliSerial->print("\t");

				cliSerial->print(RollPhiSet); cliSerial->print("\t");
				cliSerial->print(PitchPhiSet); cliSerial->print("\t");
				cliSerial->print(YawPhiSet); cliSerial->print("\t");



				cliSerial->println();
			}

			if (g_bSendJoyOutput)
			{
				cliSerial->print("JOY "); cliSerial->print(now); cliSerial->print("\t");
				for (int i = 0; i < MANUAL_INPUT_COUNT; i++)
				{
					uint16_t val = 0;
					getManCmdAxisValue((uint8) i, &val);
					cliSerial->print(val); cliSerial->print("\t");
				}
				cliSerial->println();
			}
		}

		//****************************
		// slow rate actions
		//****************************
		if (bEnterPID)
		{

			measure_loop(!bOutputDebug);  //evito di misurare i loop che scrivono su seriale...

			bool tmp = false;
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
#ifdef GIMBAL_ENABLE_COMPASS
					readMAG();
#endif
					break;
				case 6:
					// gimbal state transitions
					switch (gimState)
					{
						case GIM_IDLE :
							// wait 2 sec to settle ACC, before PID controlerbecomes active
							if (now - stateStart >= IDLE_TIME_SEC)
							{
							gimState = GIM_UNLOCKED;
							stateStart = now;
							}
							break;
						case GIM_UNLOCKED :
							// allow PID controller to settle on ACC position
							if (now - stateStart >= LOCK_TIME_SEC)
							{
							gimState = GIM_LOCKED;
							stateStart = now;
							}
							break;
						case GIM_LOCKED :
							// normal operation
							break;
					}
						// gimbal state actions
					switch (gimState) {
						case GIM_IDLE :
							enableMotorUpdates = false;
							LEDGREPIN_OFF
							setACCFastMode(true);
							break;
						case GIM_UNLOCKED :
							enableMotorUpdates = true;
							LEDGREPIN_ON
							setACCFastMode(true);
							break;
						case GIM_LOCKED :
							enableMotorUpdates = true;
							LEDGREPIN_ON
							setACCFastMode(false);
							break;
					}
				break;

#ifdef GIMBAL_ENABLE_RC
			case 7:
				// RC Pitch function
				if (rcData[RC_DATA_PITCH].valid) {
					if(config.profiles[0].rcConfig[axisPITCH].absolute) {
						PitchPhiSet = rcData[RC_DATA_PITCH].setpoint;
					} else {
						if(fabs(rcData[RC_DATA_PITCH].rcSpeed)>0.01) {
							PitchPhiSet += rcData[RC_DATA_PITCH].rcSpeed * 0.01;
						}
					}
				} else {
					PitchPhiSet = 0;
				}
				//verifico se  stato richiesto il reset
				tmp = false;
				if (rcData[RC_DATA_RESET_PITCH].valid) {
					//il reset mi interessa solo se sto pilotando in relativo
					if(!config.profiles[0].rcConfig[axisPITCH].absolute) {
						//if (rcData[RC_DATA_RESET_PITCH].rcSpeed > 0)
						if (rcData[RC_DATA_RESET_PITCH].setpoint > 0.5f)
						{
							//PitchPhiSet = 0;
							tmp = true;
						}
					}
				}
				PitchResetting = tmp;

				if (config.profiles[0].rcConfig[axisPITCH].minOutput < config.profiles[0].rcConfig[axisPITCH].maxOutput) {
					PitchPhiSet = constrain(PitchPhiSet, config.profiles[0].rcConfig[axisPITCH].minOutput, config.profiles[0].rcConfig[axisPITCH].maxOutput);
				} else {
					PitchPhiSet = constrain(PitchPhiSet, config.profiles[0].rcConfig[axisPITCH].maxOutput, config.profiles[0].rcConfig[axisPITCH].minOutput);
				}
				break;
			case 8:
				// RC roll function
				if (rcData[RC_DATA_ROLL].valid){
					if(config.profiles[0].rcConfig[axisROLL].absolute){
						RollPhiSet = rcData[RC_DATA_ROLL].setpoint;
					} else {
						if(fabs(rcData[RC_DATA_ROLL].rcSpeed)>0.01) {
							RollPhiSet += rcData[RC_DATA_ROLL].rcSpeed * 0.01;
						}
					}
				} else {
					RollPhiSet = 0;
				}

				//verifico se  stato richiesto il reset
				tmp = false;
				if (rcData[RC_DATA_RESET_ROLL].valid) {
					//il reset mi interessa solo se sto pilotando in relativo
					if(!config.profiles[0].rcConfig[axisROLL].absolute) {
						//if (rcData[RC_DATA_RESET_ROLL].rcSpeed > 0)
						if (rcData[RC_DATA_RESET_ROLL].setpoint > 0.5f)
						{
							//RollPhiSet = 0;
							tmp = true;
						}
					}
				}
				RollResetting = tmp;

				if (config.profiles[0].rcConfig[axisROLL].minOutput < config.profiles[0].rcConfig[axisROLL].maxOutput) {
					RollPhiSet = constrain(RollPhiSet, config.profiles[0].rcConfig[axisROLL].minOutput, config.profiles[0].rcConfig[axisROLL].maxOutput);
				} else {
					RollPhiSet = constrain(RollPhiSet, config.profiles[0].rcConfig[axisROLL].maxOutput, config.profiles[0].rcConfig[axisROLL].minOutput);
				}
				break;
			case 9:
				// RC yaw function
				if (rcData[RC_DATA_YAW].valid){
					if(config.profiles[0].rcConfig[axisYAW].absolute){
						YawPhiSet = rcData[RC_DATA_YAW].setpoint;
					} else {
						if(fabs(rcData[RC_DATA_YAW].rcSpeed)>0.01) {
							YawPhiSet += rcData[RC_DATA_YAW].rcSpeed * 0.01;
						}
					}
				} else {
					YawPhiSet = 0;
				}

				//verifico se  stato richiesto il reset
				tmp = false;
				if (rcData[RC_DATA_RESET_YAW].valid) {
					//il reset mi interessa solo se sto pilotando in relativo
					if(!config.profiles[0].rcConfig[axisYAW].absolute) {
						//if (rcData[RC_DATA_RESET_YAW].rcSpeed > 0)
						if (rcData[RC_DATA_RESET_YAW].setpoint > 0.5f)
						{
							//YawPhiSet = 0;
							tmp = true;
						}
					}
				}
				YawResetting = tmp;

				if ((config.profiles[0].rcConfig[axisYAW].minOutput == config.profiles[0].rcConfig[axisYAW].maxOutput) && (config.profiles[0].rcConfig[axisYAW].maxOutput == 0)) {
					//normalizzare il phiset genera dei casini micidiali quando si salta da -180 a 180 e viceversa
					//quindi lo evito
					//YawPhiSet = normalize_yaw(YawPhiSet);
				} else if (config.profiles[0].rcConfig[axisYAW].minOutput < config.profiles[0].rcConfig[axisYAW].maxOutput) {
					YawPhiSet = constrain(YawPhiSet, config.profiles[0].rcConfig[axisYAW].minOutput, config.profiles[0].rcConfig[axisYAW].maxOutput);
				} else {
					YawPhiSet = constrain(YawPhiSet, config.profiles[0].rcConfig[axisYAW].maxOutput, config.profiles[0].rcConfig[axisYAW].minOutput);
				}
				break;
#endif
			case 10:
#ifdef STACKHEAPCHECK_ENABLE
				stackHeapEval(false);
#endif
				count=0;
				break;
			default:
				break;
		}
		count++;
       
		//****************************
		// check RC channel timeouts
		//****************************
#ifdef GIMBAL_ENABLE_RC
		checkRcTimeouts();
#endif
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
		sCmd.readSerial();

		}
#ifdef BRUGI_USE_INTERRUPT_TIMER
	}
#endif
}



void loop_test()
{


	measure_loop();

	static char pOutCnt = 0;
	static uint32_t output_time = 0;

	//static int stateCount = 0;

#ifndef BRUGI_USE_INTERRUPT_TIMER
	motorInterrupt();
#endif
	uint32_t unow = micros();
	static uint32_t u_last_motor_update = 0;

	float pid_lap = ((float) measure_micro_delay( u_last_motor_update, unow) / 1000.0f);


	//if ( pid_lap >= DT_LOOP_MS)
	{
		u_last_motor_update = unow;

		motorUpdate = false;

		//if (g_bTest[0])
		readGyros();
		if (config.profiles[0].enableGyro) updateGyroAttitude();
		if (config.profiles[0].enableACC) updateACCAttitude();
		//if (g_bTest[1])
		getAttiduteAngles();

//		if (g_bTest[2])
//		{
//			//prova azionamento motori
//			rollMotorDrive++;
//			if (rollMotorDrive > N_SIN/2)
//				rollMotorDrive = 0;
//			pitchMotorDrive++;
//			if (pitchMotorDrive > N_SIN/2)
//				pitchMotorDrive = 0;
//			yawMotorDrive++;
//			if (yawMotorDrive > N_SIN/2)
//				yawMotorDrive = 0;
//		}

		//****************************
		// slow rate actions
		//****************************
		uint32_t now = millis();

		if ((now - output_time) > (1000 / POUT_FREQ))
		{
			output_time = now;

			// 600 us
			if(g_accOutput){
				cliSerial->print(angle[axisPITCH]);
				cliSerial->print(F(" ACC "));cliSerial->print(angle[axisROLL]);
				cliSerial->print(F(" "));cliSerial->print(angle[axisYAW]);

				cliSerial->print(F(" "));cliSerial->print(interrupt_mean_lap.mean());
				cliSerial->print(F(" "));cliSerial->print(interrupt_mean_duration.mean());
				cliSerial->print(F(" "));cliSerial->print(loop_mean_lap.mean());
				cliSerial->println();
			}
		}


		switch (count) {
		case 1:
			//if (g_bTest[3])
				readACC(axisROLL);
			break;
		case 2:
			//if (g_bTest[3])
				readACC(axisPITCH);
			break;
		case 3:
			//if (g_bTest[3])
				readACC(axisYAW);
			break;
		case 4:
			//if (g_bTest[4])
				updateACC();
			break;
		case 5:
			break;
			case 6:
				// gimbal state transitions
				switch (gimState)
				{
					case GIM_IDLE :
						// wait 2 sec to settle ACC, before PID controlerbecomes active
						if (now - stateStart >= IDLE_TIME_SEC)
						{
						gimState = GIM_UNLOCKED;
						stateStart = now;
						}
						break;
					case GIM_UNLOCKED :
						// allow PID controller to settle on ACC position
						if (now - stateStart >= LOCK_TIME_SEC)
						{
						gimState = GIM_LOCKED;
						stateStart = now;
						}
						break;
					case GIM_LOCKED :
						// normal operation
						break;
				}
					// gimbal state actions
				switch (gimState) {
					case GIM_IDLE :
						enableMotorUpdates = false;
						LEDGREPIN_OFF
						setACCFastMode(true);
						break;
					case GIM_UNLOCKED :
						enableMotorUpdates = true;
						LEDGREPIN_ON
						setACCFastMode(true);
						break;
					case GIM_LOCKED :
						enableMotorUpdates = true;
						LEDGREPIN_ON
						setACCFastMode(false);
						break;
				}
			break;
				case 10:
					count=0;
					break;
				default:
					break;
		}
		count++;
		sCmd.readSerial();
	}
}

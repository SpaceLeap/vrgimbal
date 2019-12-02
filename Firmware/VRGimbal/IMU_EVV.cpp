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

// **************************************************
// Simplified IMU based on "Complementary Filter"
// Inspired by http://starlino.com/imu_guide.html
//
// adapted by ziss_dm : http://www.multiwii.com/forum/viewtopic.php?f=8&t=198
//
// The following ideas was used in this project:
// 1) Rotation matrix: http://en.wikipedia.org/wiki/Rotation_matrix
// 2) Small-angle approximation: http://en.wikipedia.org/wiki/Small-angle_approximation
// 3) C. Hastings approximation for atan2()
// 4) Optimization tricks: http://www.hackersdelight.org/
//
// Currently Magnetometer uses separate CF which is used only
// for heading approximation.
//
// **************************************************

#include "main.h"

#if defined ( IMU_EVV )

//******  advanced users settings *******************
/* Set the Low Pass Filter factor for ACC */
/* Increasing this value would reduce ACC noise (visible in GUI), but would increase ACC lag time*/
/* Comment this if  you do not want filter at all.*/
#ifndef ACC_LPF_FACTOR
  #define ACC_LPF_FACTOR 40
#endif

#define NUMAXIS 3
#define EULAR 3

float AccAngleSmooth[EULAR];
float CameraOrient[EULAR];

bool g_imu_use_compass = false;

void EVV_Init_Orientation();

// set default sensor orientation (sensor upside)
void initSensorOrientationDefault() {

	//versione con chip in alto e connettore a vaschetta verso dietro
	// channel assignment
	sensorDef.Gyro[axisROLL].idx = 0;
	sensorDef.Gyro[axisPITCH].idx = 1;
	sensorDef.Gyro[axisYAW].idx = 2;

	sensorDef.Acc[axisROLL].idx = 0;
	sensorDef.Acc[axisPITCH].idx = 1;
	sensorDef.Acc[axisYAW].idx = 2;

	sensorDef.Mag[axisROLL].idx = 0;
	sensorDef.Mag[axisPITCH].idx = 1;
	sensorDef.Mag[axisYAW].idx = 2;

	// direction
	sensorDef.Gyro[axisROLL].dir = 1;
	sensorDef.Gyro[axisPITCH].dir = 1;
	sensorDef.Gyro[axisYAW].dir = 1;

	sensorDef.Acc[axisROLL].dir = 1;
	sensorDef.Acc[axisPITCH].dir = 1;
	sensorDef.Acc[axisYAW].dir = 1;

	sensorDef.Mag[axisROLL].dir = -1;
	sensorDef.Mag[axisPITCH].dir = -1;
	sensorDef.Mag[axisYAW].dir = 1;


//tentativo di allineamento con BruGi
//	// channel assignment
//	sensorDef.Gyro[axisROLL].idx = 1; //0;
//	sensorDef.Gyro[axisPITCH].idx = 0; //1;
//	sensorDef.Gyro[axisYAW].idx = 2;
//
//	sensorDef.Acc[axisROLL].idx = 1; //0;
//	sensorDef.Acc[axisPITCH].idx = 0; //1;
//	sensorDef.Acc[axisYAW].idx = 2;
//
//	// direction
//	sensorDef.Gyro[axisROLL].dir = -1; //1;
//	sensorDef.Gyro[axisPITCH].dir = 1;
//	sensorDef.Gyro[axisYAW].dir = 1;
//
//	sensorDef.Acc[axisROLL].dir = -1; //1;
//	sensorDef.Acc[axisPITCH].dir = 1;
//	sensorDef.Acc[axisYAW].dir = 1;
  
}

// swap two char items
void swap_char(char * a, char * b) {
	char tmp = *a;
	*a = *b;
	*b = tmp;
}
// swap two int items
void swap_int(int * a, int * b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// set sensor orientation according config
//
//   config.axisReverseZ
//        false ... sensor mounted on top
//        true  ... sensor mounted upside down
//   config.axisSwapXY
//        false ... default XY axes
//        true  ... swap XY (means exchange Roll/Pitch)

void initSensorOrientation() {
  
	initSensorOrientationDefault();

	if (config.axisReverseZ) {
		// flip over roll

		sensorDef.Acc[axisYAW].dir *= -1;
		sensorDef.Acc[axisROLL].dir *= -1;
		//sensorDef.Acc[axisPITCH].dir *= -1;

		//sensorDef.Gyro[axisPITCH].dir *= -1;
		sensorDef.Gyro[axisROLL].dir *= -1;
		sensorDef.Gyro[axisYAW].dir *= -1;


		sensorDef.Mag[axisYAW].dir *= -1;
		//sensorDef.Mag[axisROLL].dir *= -1;
		sensorDef.Mag[axisPITCH].dir *= -1;
	}
	if (config.axisSwapXY) {
		// swap gyro axis
		swap_char(&sensorDef.Gyro[axisROLL].idx, &sensorDef.Gyro[axisPITCH].idx);
		swap_int(&sensorDef.Gyro[axisROLL].dir, &sensorDef.Gyro[axisPITCH].dir);

		//sensorDef.Gyro[axisPITCH].dir *= -1;   // try and error ;-)
		sensorDef.Gyro[axisROLL].dir *= -1;


		// swap acc axis
		swap_char(&sensorDef.Acc[axisROLL].idx, &sensorDef.Acc[axisPITCH].idx);
		swap_int(&sensorDef.Acc[axisROLL].dir, &sensorDef.Acc[axisPITCH].dir); sensorDef.Acc[axisROLL].dir *= -1;


		swap_char(&sensorDef.Mag[axisROLL].idx, &sensorDef.Mag[axisPITCH].idx);
		swap_int(&sensorDef.Mag[axisROLL].dir, &sensorDef.Mag[axisPITCH].dir); sensorDef.Mag[axisROLL].dir *= -1;
	}

	//compass.set_transformation
}

void setACCFastMode (bool fastMode) {
	if (fastMode) {
		AccComplFilterConst = (float)DT_FLOAT/(2.0 + DT_FLOAT); // 2 sec
	} else {
		AccComplFilterConst = (float)DT_FLOAT/(config.profiles[0].accTimeConstant + DT_FLOAT);
	}
}

void initIMU() {
 
	// resolutionDevider=131, scale = 0.000133
	// 102us
	gyroScale =  1.0 / resolutionDevider / 180.0 * PI;  // convert to radians

	setACCFastMode(false);
#ifdef GIMBAL_ENABLE_COMPASS
	g_imu_use_compass = true;

	cliSerial->print("COMPASS Init\r\n");
	if (!compass.init() || !compass.read()) {
		//compass.null_offsets_enable();
		cliSerial->println("COMPASS INIT ERROR");
	}
#endif

}


float _readGyros_lap = 0.0f;

void readGyros() {
	int16_t axisRot[3];
	unsigned char idx;
	int i;


	//misuro il tempo dall'ultima lettura
	static uint32_t last_gyro_update = 0;
	uint32_t now = micros();
	_readGyros_lap = ((float) measure_micro_delay( last_gyro_update, now) / 1000000.0f);
	last_gyro_update = now;

	if (_readGyros_lap > 10 * DT_FLOAT)
		_readGyros_lap = DT_FLOAT;


	// read gyros
	mpu.getRotation(&axisRot[0], &axisRot[1], &axisRot[2]);

	for (i = 0; i < 3; i++)
	{
		idx = sensorDef.Gyro[i].idx;
		gyroADC[i] = axisRot[idx]-gyroOffset[idx];
		gyroADC[i] *= sensorDef.Gyro[i].dir;
	}

	if (mpu_yaw_present)
	{
#ifdef IMU_SECONDARY_ON_ROLL
		mpu2.getRotation(&axisRot[0], &axisRot[1], &axisRot[2]);

		for (i = 0; i < 3; i++)
		{
			idx = sensorDef.Gyro[i].idx;
			gyroADC2[i] = axisRot[idx]-gyroOffset2[idx];
			gyroADC2[i] *= sensorDef.Gyro[i].dir;
		}
#else
		i = 2;
		idx = sensorDef.Gyro[i].idx;
		int16_t rotYaw = 0;
		switch (idx)
		{
		case 0:
			rotYaw = mpu_yaw.getRotationX();
			break;
		case 1:
			rotYaw = mpu_yaw.getRotationY();
			break;
		case 2:
			rotYaw = mpu_yaw.getRotationZ();
			break;

		}
		gyroADC2[i] = rotYaw - gyroOffset2[idx];
		gyroADC2[i] *= sensorDef.Gyro[i].dir;
#endif
	}

}

void readACC(axisDef axis) {
	// get acceleration
	unsigned char idx;
	int16_t val;
	idx = sensorDef.Acc[axis].idx;
	val = mpu.getAccelerationN(idx);  // TODO: 370us

	//float v = accScale[idx] * MPU6000_ACCEL_SCALE_1G * (float) val - accOffset[idx] ;

	float v = accScale[idx] * (float) val - accOffset[idx] / MPU6000_ACCEL_SCALE_1G ;
	v *= (float) sensorDef.Acc[axis].dir;
	accADC[axis] = v;

	//accADC[axis] = val;
	//accADC[axis] = MPU6000_ACCEL_SCALE_1G *(float) val;
	//accADC[axis] = accScale[axis] * val - accOffset[axis] ;

}

#ifdef GIMBAL_ENABLE_COMPASS
void readMAG()
{
	if (g_imu_use_compass)
	{
		if (compass.read())
		{
			compass.set_learn(true);
			//compass.null_offsets();
		}
	}
}
void updateMAG()
{

}

void updateMAGAttitude()
{

	if (g_imu_use_compass)
	{
		float p = ((float) angle[axisPITCH] / 1000.0);
		float r = ((float) angle[axisROLL] / 1000.0);
		float heading = compass.calculate_yaw(r, p);
		//angle[axisYAW] = (config.angleOffsetYaw * 10) + (int32_t) (heading * 1000.0 * 180.0 / PI) ;
		CameraOrient[axisYAW] =  (1.0 - AccComplFilterConst) * CameraOrient[axisYAW]  + AccComplFilterConst * heading;
	}
}

#endif


Vector3f meanGyro;
int meanCount = 0;

void print_raw(float x1,float y1,float z1,
		float x2 = 0.0f,float y2 = 0.0f,float z2 = 0.0f,
		float x3 = 0.0f,float y3 = 0.0f,float z3 = 0.0f,
		float temp = 0.0f)
{

	//media giroscopio
	meanCount++;
	meanGyro.x = (meanGyro.x * (meanCount - 1) + x2) / meanCount;
	meanGyro.y = (meanGyro.y * (meanCount - 1) + y2) / meanCount;
	meanGyro.z = (meanGyro.z * (meanCount - 1) + z2) / meanCount;


	//debug
	//****************************
	// slow rate actions
	//****************************
	static uint32_t output_time = 0;
	uint32_t now = millis();
	if ((now - output_time) > (1000 / POUT_FREQ))
	{

		output_time = now;

		cliSerial->print(F("RAW "));
		cliSerial->print(now);cliSerial->print(F(" "));

		Vector3f dummy;
		dummy.x =x1;
		dummy.y =y1;
		dummy.z =z1;

		print_vector(dummy); //(acc);

		dummy.x = x2;
		dummy.y = y2;
		dummy.z = z2;

		print_vector(dummy); //(gyro);

		dummy.x =x3;
		dummy.y =y3;
		dummy.z =z3;

		print_vector(dummy); //(mag);
		cliSerial->print(temp);cliSerial->print(F(" "));

		print_vector(meanGyro);

		cliSerial->println();

		meanGyro.x = 0.0f;
		meanGyro.y = 0.0f;
		meanGyro.z = 0.0f;
		meanCount = 0;
	}

}



void updateGyroAttitude(){
	uint8_t axis;

	float GyroData[3];
	float GyroRate[EULAR];

	for (axis = 0; axis < 3; axis++) {
		GyroData[axis] = (float) gyroADC[axis]  * gyroScale;
	}

//	GyroRate[axisPITCH] = GyroData[axisX];
//	//CameraOrient[axisPITCH] = (CameraOrient[axisPITCH] + GyroRate[axisPITCH] * lap) + 0.0002 * (AccAngleSmooth[axisPITCH] - CameraOrient[axisPITCH]); //Pitch Horizon
//	CameraOrient[axisPITCH] = (1.0 - AccComplFilterConst) * (CameraOrient[axisPITCH] + GyroRate[axisPITCH] * _readGyros_lap) + AccComplFilterConst * AccAngleSmooth[axisPITCH]; //Pitch Horizon
//
//	GyroRate[axisROLL] = -GyroData[axisZ] * sin(CameraOrient[axisPITCH]) + GyroData[axisY] * cos(fabs(CameraOrient[axisPITCH]));
//	//CameraOrient[axisROLL] = (CameraOrient[axisROLL] + GyroRate[axisROLL] * lap) + 0.0002 * (AccAngleSmooth[axisROLL] - CameraOrient[axisROLL]); //Roll Horizon
//	CameraOrient[axisROLL] = (1.0 - AccComplFilterConst) * (CameraOrient[axisROLL] + GyroRate[axisROLL] * _readGyros_lap) + AccComplFilterConst * AccAngleSmooth[axisROLL]; //Roll Horizon
//
//	GyroRate[axisYAW] = -GyroData[axisZ] * cos(fabs(CameraOrient[axisPITCH])) - GyroData[axisY] * sin(CameraOrient[axisPITCH]); //presuming Roll is horizontal
//	CameraOrient[axisYAW] = (CameraOrient[axisYAW] + GyroRate[axisYAW] * _readGyros_lap);

	//applico qui solo il deltaGyro, la parte acc la applico nella funzione dedicata
	GyroRate[axisPITCH] = GyroData[axisX];
	CameraOrient[axisPITCH] = CameraOrient[axisPITCH] + GyroRate[axisPITCH] * _readGyros_lap;

	GyroRate[axisROLL] = -GyroData[axisZ] * sin(CameraOrient[axisPITCH]) + GyroData[axisY] * cos(fabs(CameraOrient[axisPITCH]));
	CameraOrient[axisROLL] = CameraOrient[axisROLL] + GyroRate[axisROLL] * _readGyros_lap;

	if (mpu_yaw_present)
	{
		#ifdef IMU_SECONDARY_ON_ROLL
			TODO: gestire la seconda imu sul roll
		#else
			GyroRate[axisYAW] = -(float) gyroADC2[axisZ]  * gyroScale;
		#endif
	} else {
		GyroRate[axisYAW] = -GyroData[axisZ] * cos(fabs(CameraOrient[axisPITCH])) - GyroData[axisY] * sin(CameraOrient[axisPITCH]); //presuming Roll is horizontal
	}
	CameraOrient[axisYAW] = CameraOrient[axisYAW] + GyroRate[axisYAW] * _readGyros_lap;


}

void updateACC(){
	float AccAngle[EULAR];
	if (accADC[axisZ] != 0)
	{
		//AccAngle[axisROLL] = -(atan2(accADC[axisX], accADC[axisZ])); //Calculating pitch ACC angle
		AccAngle[axisROLL] = -sgn(atan2(accADC[axisX], accADC[axisZ])) * fabs( atan2(accADC[axisX], sqrt(accADC[axisZ] * accADC[axisZ] + accADC[axisY] * accADC[axisY]))); //Calculating pitch ACC angle
		AccAngle[axisPITCH] = +(atan2(accADC[axisY], accADC[axisZ])); //Calculating roll ACC angle

		//LPF on Acc
		utilLP_float(&(AccAngleSmooth[axisROLL]), AccAngle[axisROLL], (1.0f/ACC_LPF_FACTOR));
		utilLP_float(&(AccAngleSmooth[axisPITCH]), AccAngle[axisPITCH], (1.0f/ACC_LPF_FACTOR));

		accMag = sqrt(accADC[axisX] * accADC[axisX] + accADC[axisY] * accADC[axisY]+ accADC[axisZ] * accADC[axisZ]);

	}
}


void updateACCAttitude(){
//TODO: testare questa modifica e verificare che il range di valori sia corretto
	if (fabs(accMag * MPU6000_ACCEL_SCALE_1G - GRAVITY) < 0.4 * GRAVITY) {
		CameraOrient[axisPITCH] = (1.0 - AccComplFilterConst) * CameraOrient[axisPITCH] + AccComplFilterConst * AccAngleSmooth[axisPITCH]; //Pitch Horizon
		CameraOrient[axisROLL] =  (1.0 - AccComplFilterConst) * CameraOrient[axisROLL]  + AccComplFilterConst * AccAngleSmooth[axisROLL]; //Roll Horizon
	}
}






void getAttiduteAngles() {

  // attitude of the estimated vector
	angle[axisROLL]  = ((int32_t)config.profiles[0].axisConfig[axisROLL].angleOffset * 10) +  CameraOrient[axisROLL] * 1000.0f * 180.0f / PI;
	angle[axisPITCH] = ((int32_t)config.profiles[0].axisConfig[axisPITCH].angleOffset * 10) + CameraOrient[axisPITCH] * 1000.0f * 180.0f / PI;
	angle[axisYAW] = ((int32_t)config.profiles[0].axisConfig[axisYAW].angleOffset * 10) + CameraOrient[axisYAW] * 1000.0f * 180.0f / PI;
}




void EVV_Init_Orientation()
{

	AccAngleSmooth[axisROLL] = 0.0f;
	AccAngleSmooth[axisPITCH] = 0.0f;
	AccAngleSmooth[axisYAW] = 0.0f;

	CameraOrient[axisROLL] = 0.0f;
	CameraOrient[axisPITCH] = 0.0f;
	CameraOrient[axisYAW] = 0.0f;



    int init_loops = 150;
    float AccAngle[NUMAXIS];
    int i;

    for (i = 0; i < init_loops; i++)
    {
    	readACC(axisROLL);
    	readACC(axisPITCH);
    	readACC(axisYAW);

        //AccAngle[axisROLL] = -(atan2(accADC[axisX], accADC[axisZ])); //Calculating pitch ACC angle
    	AccAngle[axisROLL] = -sgn(atan2(accADC[axisX], accADC[axisZ])) * fabs( atan2(accADC[axisX], sqrt(accADC[axisZ] * accADC[axisZ] + accADC[axisY] * accADC[axisY]))); //Calculating pitch ACC angle
        AccAngle[axisPITCH] = +(atan2(accADC[axisY], accADC[axisZ])); //Calculating roll ACC angle

        if (i == 0)
        {
        	AccAngleSmooth[axisROLL] = AccAngle[axisROLL];
        	AccAngleSmooth[axisPITCH] = AccAngle[axisPITCH];
        } else {

			//AccAngleSmooth[axisROLL] = ((AccAngleSmooth[axisROLL] * (float)(init_loops - 1)) + AccAngle[axisROLL]) / (float)init_loops; //Averaging pitch ACC values
			//AccAngleSmooth[axisPITCH] = ((AccAngleSmooth[axisPITCH] * (float)(init_loops - 1)) + AccAngle[axisPITCH]) / (float)init_loops; //Averaging roll ACC values

			AccAngleSmooth[axisROLL] = ((AccAngleSmooth[axisROLL] * (float)(i - 1)) + AccAngle[axisROLL]) / (float)i; //Averaging pitch ACC values
			AccAngleSmooth[axisPITCH] = ((AccAngleSmooth[axisPITCH] * (float)(i - 1)) + AccAngle[axisPITCH]) / (float)i; //Averaging roll ACC values
        }
        delay(1);
    }

    CameraOrient[axisPITCH] = AccAngleSmooth[axisPITCH];
    CameraOrient[axisROLL] = AccAngleSmooth[axisROLL];
    CameraOrient[axisYAW] = 0.0;
}


#endif

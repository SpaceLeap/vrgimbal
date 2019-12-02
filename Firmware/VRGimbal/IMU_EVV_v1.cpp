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

#define ACC_1G 16384.0f

// Small angle approximation
#define ssin(val) (val)
#define scos(val) 1.0f




#define NUMAXIS 3
#define EULAR 3

float AccAngleSmooth[EULAR];
float CameraOrient[EULAR];

void EVV_Init_Orientation();
void EVV_Get_Orientation(float *SmoothAcc, float *Orient, float *GyroData, float dt);


// set default sensor orientation (sensor upside)
void initSensorOrientationDefault() {
  
  // channel assignment
  sensorDef.Gyro[axisROLL].idx = 0;
  sensorDef.Gyro[axisPITCH].idx = 1;
  sensorDef.Gyro[axisYAW].idx = 2;

  sensorDef.Acc[axisROLL].idx = 0;     // y
  sensorDef.Acc[axisPITCH].idx = 1;    // x
  sensorDef.Acc[axisYAW].idx = 2;      // z

  // direction
  sensorDef.Gyro[axisROLL].dir = 1;
  sensorDef.Gyro[axisPITCH].dir = 1;
  sensorDef.Gyro[axisYAW].dir = 1;

  sensorDef.Acc[axisROLL].dir = 1;
  sensorDef.Acc[axisPITCH].dir = 1;
  sensorDef.Acc[axisYAW].dir = 1;
  
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
    sensorDef.Gyro[axisPITCH].dir *= -1;
    sensorDef.Gyro[axisYAW].dir *= -1;
  }
  if (config.axisSwapXY) {
    // swap gyro axis
    swap_char(&sensorDef.Gyro[axisROLL].idx, &sensorDef.Gyro[axisPITCH].idx);
    swap_int(&sensorDef.Gyro[axisROLL].dir, &sensorDef.Gyro[axisPITCH].dir); sensorDef.Gyro[axisPITCH].dir *= -1;   // try and error ;-)
    // swap acc axis
    swap_char(&sensorDef.Acc[axisROLL].idx, &sensorDef.Acc[axisPITCH].idx);
    swap_int(&sensorDef.Acc[axisROLL].dir, &sensorDef.Acc[axisPITCH].dir); sensorDef.Acc[axisROLL].dir *= -1;
  }
}

void setACCFastMode (bool fastMode) {
  if (fastMode) {
    AccComplFilterConst = (float)DT_FLOAT/(2.0 + DT_FLOAT); // 2 sec
  } else {
    AccComplFilterConst = (float)DT_FLOAT/(config.accTimeConstant + DT_FLOAT);
  }
}

void initIMU() {
 
  // resolutionDevider=131, scale = 0.000133
  // 102us
  gyroScale =  1.0 / resolutionDevider / 180.0 * PI;  // convert to radians
  
  setACCFastMode(false);
 
  accLPF[0] = 0;
  accLPF[1] = 0;
  accLPF[2] = ACC_1G;
 
 
  // initialize coordinate system in EstG
  EstG.V.X = 0;
  EstG.V.Y = 0;
  EstG.V.Z = ACC_1G;


	for (uint8_t axis = 0; axis < 3; axis++) {
		estimAngle[axis] = 0;
	}

	g_accOutput = true;
}

//// Rotate Estimated vector(s) with small angle approximation, according to the gyro data
//// needs angle in radian units !
//void rotateV(struct fp_vector *v,float* delta) {
//  fp_vector v_tmp = *v;
//  v->Z -= delta[axisROLL]  * v_tmp.X + delta[axisPITCH] * v_tmp.Y;
//  v->X += delta[axisROLL]  * v_tmp.Z - delta[axisYAW]   * v_tmp.Y;
//  v->Y += delta[axisPITCH] * v_tmp.Z + delta[axisYAW]   * v_tmp.X;
//}


// Rotate Estimated vector(s) with small angle approximation, according to the gyro data
void rotateV(struct fp_vector *v, float *delta)
{
    struct fp_vector v_tmp = *v;

    // This does a  "proper" matrix rotation using gyro deltas without small-angle approximation
    float mat[3][3];
    float cosx, sinx, cosy, siny, cosz, sinz;
    float coszcosx, coszcosy, sinzcosx, coszsinx, sinzsinx;

    cosx = cosf(-delta[axisPITCH]);
    sinx = sinf(-delta[axisPITCH]);
    cosy = cosf(delta[axisROLL]);
    siny = sinf(delta[axisROLL]);
    cosz = cosf(delta[axisYAW]);
    sinz = sinf(delta[axisYAW]);

    coszcosx = cosz * cosx;
    coszcosy = cosz * cosy;
    sinzcosx = sinz * cosx;
    coszsinx = sinx * cosz;
    sinzsinx = sinx * sinz;

    mat[0][0] = coszcosy;
    mat[0][1] = sinz * cosy;
    mat[0][2] = -siny;
    mat[1][0] = (coszsinx * siny) - sinzcosx;
    mat[1][1] = (sinzsinx * siny) + (coszcosx);
    mat[1][2] = cosy * sinx;
    mat[2][0] = (coszcosx * siny) + (sinzsinx);
    mat[2][1] = (sinzcosx * siny) - (coszsinx);
    mat[2][2] = cosy * cosx;

    v->X = v_tmp.X * mat[0][0] + v_tmp.Y * mat[1][0] + v_tmp.Z * mat[2][0];
    v->Y = v_tmp.X * mat[0][1] + v_tmp.Y * mat[1][1] + v_tmp.Z * mat[2][1];
    v->Z = v_tmp.X * mat[0][2] + v_tmp.Y * mat[1][2] + v_tmp.Z * mat[2][2];
}

void rotateOnAxis(struct fp_vector *v, float delta, struct fp_vector *axis)
{
	struct fp_vector v_tmp = *v;

	float x = axis->X;
	float y = axis->Y;
	float z = axis->Z;

	float mat[3][3];
	float c = cos(delta);
	float s = sin(delta);

    mat[0][0] = x*x*(1-c)+c;
    mat[0][1] = x*y*(1-c)+z*s;
    mat[0][2] = x*z*(1-c)-y*s;
    mat[1][0] = y*x*(1-c)-z*s;
    mat[1][1] = y*y*(1-c)+c;
    mat[1][2] = y*z*(1-c)+x*s;
    mat[2][0] = x*z*(1-c)+y*s;
    mat[2][1] = y*z*(1-c)+x*s;
    mat[2][2] = z*z*(1-c)+c;

    v->X = v_tmp.X * mat[0][0] + v_tmp.Y * mat[1][0] + v_tmp.Z * mat[2][0];
    v->Y = v_tmp.X * mat[0][1] + v_tmp.Y * mat[1][1] + v_tmp.Z * mat[2][1];
    v->Z = v_tmp.X * mat[0][2] + v_tmp.Y * mat[1][2] + v_tmp.Z * mat[2][2];
}


void readGyros() {
  int16_t axisRot[3];
  char idx;
  // 414 us

  // read gyros
  mpu.getRotation(&axisRot[0], &axisRot[1], &axisRot[2]);
  idx = sensorDef.Gyro[0].idx;
  gyroADC[axisROLL] = axisRot[idx]-gyroOffset[idx];
  gyroADC[axisROLL] *= sensorDef.Gyro[0].dir;

  idx = sensorDef.Gyro[1].idx;
  gyroADC[axisPITCH] = axisRot[idx]-gyroOffset[idx];
  gyroADC[axisPITCH] *= sensorDef.Gyro[1].dir;

  idx = sensorDef.Gyro[2].idx;
  gyroADC[axisYAW] = axisRot[idx]-gyroOffset[idx];
  gyroADC[axisYAW] *= sensorDef.Gyro[2].dir;
  

  if (mpu_yaw_present)
  {
#ifdef IMU_SECONDARY_ON_ROLL
	  gyroADC2[axisROLL] = mpu_yaw.getRotationZ() - gyroOffset2[axisROLL];
	  gyroADC2[axisPITCH] = mpu_yaw.getRotationX() - gyroOffset2[axisPITCH];
	  gyroADC2[axisYAW] = mpu_yaw.getRotationY() - gyroOffset2[axisYAW];
#else
	  gyroADC2[axisYAW] = mpu_yaw.getRotationZ() - gyroOffset2[axisYAW];
#endif

  } else {
	  gyroADC2[axisYAW] = 0;
  }


}

void readACC(axisDef axis) {
  // get acceleration
  // 382 us
  unsigned char idx;
  int16_t val;
  idx = sensorDef.Acc[axis].idx;
  val = mpu.getAccelerationN(idx);  // TODO: 370us 
  val *= sensorDef.Acc[axis].dir;
  accADC[axis] = val;
}


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

//#define USE_COMPENSATION

uint32_t last_gyro_update = 0;
void updateGyroAttitude(){
	uint8_t axis;

	float GyroData[3];
	//float accVec[3];

	//uint32_t now = millis();

	uint32_t now = micros();

	float lap = ((float) measure_micro_delay( last_gyro_update, now) / 1000000.0f);
	last_gyro_update = now;

	if (lap > 10 * DT_FLOAT)
	lap = DT_FLOAT;

	for (axis = 0; axis < 3; axis++) {
		GyroData[axis] = (float) gyroADC[axis]  * gyroScale;
		//accVec[axis] = (float) accADC[axis];
	}

	//EVV_Get_Orientation(AccAngleSmooth, CameraOrient, gyroRate, lap);


	float GyroRate[EULAR];

	GyroRate[axisPITCH] = GyroData[axisX];
	//CameraOrient[axisPITCH] = (CameraOrient[axisPITCH] + GyroRate[axisPITCH] * lap) + 0.0002 * (AccAngleSmooth[axisPITCH] - CameraOrient[axisPITCH]); //Pitch Horizon
	CameraOrient[axisPITCH] = (1.0 - AccComplFilterConst) * (CameraOrient[axisPITCH] + GyroRate[axisPITCH] * lap) + AccComplFilterConst * AccAngleSmooth[axisPITCH]; //Pitch Horizon

	GyroRate[axisROLL] = -GyroData[axisZ] * sin(CameraOrient[axisPITCH]) + GyroData[axisY] * cos(fabs(CameraOrient[axisPITCH]));
	//CameraOrient[axisROLL] = (CameraOrient[axisROLL] + GyroRate[axisROLL] * lap) + 0.0002 * (AccAngleSmooth[axisROLL] - CameraOrient[axisROLL]); //Roll Horizon
	CameraOrient[axisROLL] = (1.0 - AccComplFilterConst) * (CameraOrient[axisROLL] + GyroRate[axisROLL] * lap) + AccComplFilterConst * AccAngleSmooth[axisROLL]; //Roll Horizon

	GyroRate[axisYAW] = -GyroData[axisZ] * cos(fabs(CameraOrient[axisPITCH])) - GyroData[axisY] * sin(CameraOrient[axisPITCH]); //presuming Roll is horizontal
	CameraOrient[axisYAW] = (CameraOrient[axisYAW] + GyroRate[axisYAW] * lap);

}

void updateACC(){
	float AccAngle[EULAR];
	if (accADC[axisZ] != 0)
	{
		//AccAngle[axisROLL] = -(atan2(accADC[axisX], accADC[axisZ])); //Calculating pitch ACC angle
		AccAngle[axisROLL] = -sgn(atan2(accADC[axisX], accADC[axisZ])) * fabs( atan2(accADC[axisX], sqrt(accADC[axisZ] * accADC[axisZ] + accADC[axisY] * accADC[axisY]))); //Calculating pitch ACC angle
		AccAngle[axisPITCH] = +(atan2(accADC[axisY], accADC[axisZ])); //Calculating roll ACC angle
		//AccAngleSmooth[axisROLL] = ((AccAngleSmooth[axisROLL] * 99.0) +  AccAngle[axisROLL]) / 100.0; //Averaging pitch ACC values
		//AccAngleSmooth[axisPITCH] = ((AccAngleSmooth[axisPITCH] * 99.0) + AccAngle[axisPITCH]) / 100.0; //Averaging roll ACC values

		utilLP_float(&(AccAngleSmooth[axisROLL]), AccAngle[axisROLL], (1.0f/ACC_LPF_FACTOR));
		utilLP_float(&(AccAngleSmooth[axisPITCH]), AccAngle[axisPITCH], (1.0f/ACC_LPF_FACTOR));

	}
}


void updateACCAttitude(){
}






void getAttiduteAngles() {

  // attitude of the estimated vector

  // 272 us
  angleIMU[axisROLL]  = ((int32_t)config.angleOffsetRoll * 10) +  CameraOrient[axisROLL] * 1000.0f * 180.0f / PI;

  // 192 us
  angleIMU[axisPITCH] = ((int32_t)config.angleOffsetPitch * 10) + CameraOrient[axisPITCH] * 1000.0f * 180.0f / PI;

  angleIMU[axisYAW] = ((int32_t)config.angleOffsetYaw * 10) + CameraOrient[axisYAW] * 1000.0f * 180.0f / PI;

  angle[axisROLL] = angleIMU[axisROLL];
  angle[axisPITCH] = angleIMU[axisPITCH];
  angle[axisYAW] = angleIMU[axisYAW];
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

void EVV_Get_Orientation(float *SmoothAcc, float *Orient,float *GyroData, float dt) // float *AccData,
{
	//cliSerial->print(__FUNCTION__);
    float GyroRate[EULAR];

    //lo faccio solo nell'update acc
//    float AccAngle[EULAR];
//    AccAngle[axisROLL] = -(atan2(AccData[axisX], AccData[axisZ])); //Calculating pitch ACC angle
//    AccAngle[axisPITCH] = +(atan2(AccData[axisY], AccData[axisZ])); //Calculating roll ACC angle
//    SmoothAcc[axisROLL] = ((SmoothAcc[axisROLL] * 99.0) + AccAngle[axisROLL]) / 100.0; //Averaging pitch ACC values
//    SmoothAcc[axisPITCH] = ((SmoothAcc[axisPITCH] * 99.0) + AccAngle[axisPITCH]) / 100.0; //Averaging roll ACC values

    GyroRate[axisPITCH] = GyroData[axisX];
    //Orient[axisPITCH] = (Orient[axisPITCH] + GyroRate[axisPITCH] * dt) + 0.0002 * (SmoothAcc[axisPITCH] - Orient[axisPITCH]); //Pitch Horizon
    Orient[axisPITCH] = (1.0 - AccComplFilterConst) * (Orient[axisPITCH] + GyroRate[axisPITCH] * dt) + AccComplFilterConst * (SmoothAcc[axisPITCH] - Orient[axisPITCH]);



    GyroRate[axisROLL] = -GyroData[axisZ] * sin(Orient[axisPITCH]) + GyroData[axisY] * cos(fabs(Orient[axisPITCH]));
    //Orient[axisROLL] = (Orient[axisROLL] + GyroRate[axisROLL] * dt) + 0.0002 * (SmoothAcc[axisROLL] - Orient[axisROLL]); //Roll Horizon
    Orient[axisROLL] = (1.0 - AccComplFilterConst) * (Orient[axisROLL] + GyroRate[axisROLL] * dt) + AccComplFilterConst * (SmoothAcc[axisROLL] - Orient[axisROLL]); //Roll Horizon

    GyroRate[axisYAW] = -GyroData[axisZ] * cos(fabs(Orient[axisPITCH])) - GyroData[axisY] * sin(Orient[axisPITCH]); //presuming Roll is horizontal
    Orient[axisYAW] = (Orient[axisYAW] + GyroRate[axisYAW] * dt); //Yaw

    //cliSerial->println(" END");

}

#endif

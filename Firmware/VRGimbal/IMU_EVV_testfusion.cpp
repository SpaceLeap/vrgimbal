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

#define NUMAXIS 3
#define EULAR 3

float AccAngleSmooth[EULAR];
float CameraOrient[EULAR];

void EVV_Init_Orientation();

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

	sensorDef.Acc[axisROLL].dir = -1;
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

}


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

float _readGyros_lap = 0.0f;

void readGyros() {
	int16_t axisRot[3];
	char idx;
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

}

void readACC(axisDef axis) {
	// get acceleration
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



void updateGyroAttitude(){
	uint8_t axis;

	float GyroData[3];
	float deltaGyroAngle[EULAR];

	for (axis = 0; axis < 3; axis++) {
		GyroData[axis] = (float) gyroADC[axis]  * gyroScale;
	}

	CameraOrient[axisPITCH] = angle[axisPITCH] * PI / ( 1000.0f * 180.0f );

	deltaGyroAngle[axisPITCH] = _readGyros_lap * GyroData[axisX];
	deltaGyroAngle[axisROLL] = _readGyros_lap * (-GyroData[axisZ] * sin(CameraOrient[axisPITCH]) + GyroData[axisY] * cos(fabs(CameraOrient[axisPITCH])));
	deltaGyroAngle[axisYAW] = _readGyros_lap * (-GyroData[axisZ] * cos(fabs(CameraOrient[axisPITCH])) - GyroData[axisY] * sin(CameraOrient[axisPITCH])); //presuming Roll is horizontal

	rotateV(&EstG.V,deltaGyroAngle);

	CameraOrient[axisYAW] = (CameraOrient[axisYAW] + deltaGyroAngle[axisYAW]);
}

void updateACC(){
	uint8_t axis;

	accMag = 0;
	for (axis = 0; axis < 3; axis++) {
		utilLP_float(&accLPF[axis], accADC[axis], (1.0f/ACC_LPF_FACTOR)); // 96/3 us
		accMag += accLPF[axis]*accLPF[axis] ; // 63/3us
	}

	accMag = accMag*100.0/(ACC_1G*ACC_1G);
}


void updateACCAttitude(){
	uint8_t axis;


	// Apply complimentary filter (Gyro drift correction)
	// If accel magnitude >1.4G or <0.6G and ACC vector outside of the limit range => we neutralize the effect of accelerometers in the angle estimation.
	// To do that, we just skip filter, as EstV already rotated by Gyro
	if ( 36 < accMag && accMag < 196 ) {
		for (axis = 0; axis < 3; axis++) {
			//utilLP_float(&EstG.A[axis], accLPF[axis], AccComplFilterConst);
			EstG.A[axis] = EstG.A[axis] * (1.0 - AccComplFilterConst) + accLPF[axis] * AccComplFilterConst; // note: this is different from MultiWii (wrong brackets postion in MultiWii ??.
		}
	}
}






void getAttiduteAngles() {

  // attitude of the estimated vector
	angle[axisROLL]  = ((int32_t)config.angleOffsetRoll * 10) +  Rajan_FastArcTan2_deg1000(EstG.V.X , sqrt(EstG.V.Z*EstG.V.Z+EstG.V.Y*EstG.V.Y));
	angle[axisPITCH] = ((int32_t)config.angleOffsetPitch * 10) + Rajan_FastArcTan2_deg1000(EstG.V.Y , EstG.V.Z);
	angle[axisYAW] = ((int32_t)config.angleOffsetYaw * 10) + CameraOrient[axisYAW] * 1000.0f * 180.0f / PI;
}




void EVV_Init_Orientation()
{
	CameraOrient[axisROLL] = 0.0f;
	CameraOrient[axisPITCH] = 0.0f;
	CameraOrient[axisYAW] = 0.0f;
}


#endif

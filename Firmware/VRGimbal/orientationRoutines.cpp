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
/* MPU6050 Routines      */
/*************************/

#include "main.h"
#include "calibrationRoutines.h"

/* FS_SEL | Full Scale Range   | LSB Sensitivity
 * -------+--------------------+----------------
 * 0      | +/- 250 degrees/s  | 131 LSB/deg/s
 * 1      | +/- 500 degrees/s  | 65.5 LSB/deg/s
 * 2      | +/- 1000 degrees/s | 32.8 LSB/deg/s
 * 3      | +/- 2000 degrees/s | 16.4 LSB/deg/s
 */
/*
 * AFS_SEL | Full Scale Range | LSB Sensitivity
 * --------+------------------+----------------
 * 0       | +/- 2g           | 8192 LSB/mg
 * 1       | +/- 4g           | 4096 LSB/mg
 * 2       | +/- 8g           | 2048 LSB/mg
 * 3       | +/- 16g          | 1024 LSB/mg
*/

#if defined ( IMU_BRUGI )

//#define GRAVITY 16384.0f
#define IMU_GRAVITY 15500.0f

void gyroOffsetCalibration_LSQ(MPU6050 * p_mpu, int16_t * p_offsets);

void initResolutionDevider()
{
    if(MPU6050_GYRO_FS == 0x00) resolutionDevider = 131.0;
    if(MPU6050_GYRO_FS == 0x01) resolutionDevider = 65.5;
    if(MPU6050_GYRO_FS == 0x02) resolutionDevider = 32.8;
    if(MPU6050_GYRO_FS == 0x03) resolutionDevider = 16.4;
}


// This functions performs an initial gyro offset calibration
// INCLUDING motion detection
// Board should be still for some seconds
void gyroOffsetCalibration(MPU6050 * p_mpu, int16_t * p_offsets)
{

	gyroOffsetCalibration_LSQ(p_mpu, p_offsets);
	return;

  int i;
  #define TOL 64
  #define GYRO_INTERATIONS 2000
  int16_t prevGyro[3],gyro[3];
  float fp_gyroOffset[3];
  uint8_t tiltDetected = 0;
  int calibGCounter = GYRO_INTERATIONS;
  
  // Set voltage on all motor phases to zero 
  enableMotorUpdates = false;

  // wait 1 second
  for (i=0; i<100; i++) {
    delayMicroseconds(10000); // 1 ms 
  }
  
  while(calibGCounter>0)
  {

    if(calibGCounter==GYRO_INTERATIONS)
    {
      for (i=0; i<70; i++) { // wait 0.7sec if calibration failed
        delayMicroseconds(10000); // 10 ms 
      }
      p_mpu->getRotation(&gyro[0], &gyro[1], &gyro[2]);
      for (i=0; i<3; i++) {
        fp_gyroOffset[i] = 0;
        prevGyro[i]=gyro[i];
      }
    }
    
    p_mpu->getRotation(&gyro[0], &gyro[1], &gyro[2]);

    for (i=0; i<3; i++) {
      if(abs(prevGyro[i] - gyro[i]) > TOL) {
        tiltDetected++;
        //cliSerial->print(F(" i="));cliSerial->print(i);
        //cliSerial->print(F(" calibGCounter="));cliSerial->print(calibGCounter);
        //cliSerial->print(F(" diff="));cliSerial->print(prevGyro[i] - gyro[i]);
        //cliSerial->print(F(" gyroi="));cliSerial->print(gyro[i]);
        //cliSerial->print(F(" prevgyroi="));cliSerial->println(prevGyro[i]);
        break;
      }
    } 
     
    for (i=0; i<3; i++) {
        fp_gyroOffset[i] += (float)gyro[i]/GYRO_INTERATIONS;
        prevGyro[i]=gyro[i];
    }
      
    calibGCounter--;
    if(tiltDetected>=1)
    {
      cliSerial->println(F("Motion detected during Gyro calibration. Starting over!"));
      calibGCounter=GYRO_INTERATIONS;
      tiltDetected=0;
    }
  }

  // put result into integer
  for (i=0; i<3; i++) {
    //gyroOffset[i] = fp_gyroOffset[i];
	p_offsets[i] = fp_gyroOffset[i];
    cliSerial->print(F("gyroOffset="));cliSerial->println(fp_gyroOffset[i], 3);
  }

  enableMotorUpdates = true;
}

void gyroOffsetCalibration_LSQ(MPU6050 * p_mpu, int16_t * p_offsets)
{
  int i;
  #define TOL 64
  #define GYRO_INTERATIONS 2000
  int16_t prevGyro[3],gyro[3];
  float fp_gyroOffset[3];
  uint8_t tiltDetected = 0;
  int calibGCounter = GYRO_INTERATIONS;

  // Set voltage on all motor phases to zero
  enableMotorUpdates = false;

  // wait 1 second
  for (i=0; i<100; i++) {
    delayMicroseconds(10000); // 1 ms
  }


  LSQIntermediate lsq;
  lsq_init(&lsq);

  while(calibGCounter>0)
  {

    if(calibGCounter==GYRO_INTERATIONS)
    {
      for (i=0; i<70; i++) { // wait 0.7sec if calibration failed
        delayMicroseconds(10000); // 10 ms
      }
      p_mpu->getRotation(&gyro[0], &gyro[1], &gyro[2]);
      for (i=0; i<3; i++) {
        prevGyro[i]=gyro[i];
      }

      //ricomincio da capo
      lsq_init(&lsq);

    }

    p_mpu->getRotation(&gyro[0], &gyro[1], &gyro[2]);

    for (i=0; i<3; i++) {
      if(abs(prevGyro[i] - gyro[i]) > TOL) {
        tiltDetected++;
        //cliSerial->print(F(" i="));cliSerial->print(i);
        //cliSerial->print(F(" calibGCounter="));cliSerial->print(calibGCounter);
        //cliSerial->print(F(" diff="));cliSerial->print(prevGyro[i] - gyro[i]);
        //cliSerial->print(F(" gyroi="));cliSerial->print(gyro[i]);
        //cliSerial->print(F(" prevgyroi="));cliSerial->println(prevGyro[i]);
        break;
      }
    }

    for (i=0; i<3; i++) {
        prevGyro[i]=gyro[i];
    }
    lsq_accumulate(&lsq, gyro[0], gyro[1], gyro[2]);

    calibGCounter--;
    if(tiltDetected>=1)
    {
      cliSerial->println(F("Motion detected during Gyro calibration. Starting over!"));
      calibGCounter=GYRO_INTERATIONS;
      tiltDetected=0;
    }
  }

  float fRadius = 0;
  lsq_calculate(&lsq, 10, 0.0f, &(fp_gyroOffset[0]),&(fp_gyroOffset[1]), &(fp_gyroOffset[2]), &fRadius);

  // put result into integer
  for (i=0; i<3; i++) {
    //gyroOffset[i] = fp_gyroOffset[i];
	p_offsets[i] = fp_gyroOffset[i];
    cliSerial->print(F("gyroOffset="));cliSerial->println(fp_gyroOffset[i], 3);
  }

  enableMotorUpdates = true;
}



// calibrate_accel - perform accelerometer calibration including providing user instructions and feedback
// Gauss-Newton accel calibration routines borrowed from Rolfe Schmidt
// blog post describing the method: http://chionophilous.wordpress.com/2011/10/24/accelerometer-calibration-iv-1-implementing-gauss-newton-on-an-atmega/
// original sketch available at http://rolfeschmidt.com/mathtools/skimetrics/adxl_gn_calibration.pde
bool accelCalibration(MPU6050 * p_mpu, float * offsets, float * scales)
{

	enableMotorUpdates = false;
	switchOffMotors();


	Vector3f offset;
	Vector3f scale;

	offset.x = offsets[0];
	offset.y = offsets[1];
	offset.z = offsets[2];
	scale.x = scales[0];
	scale.y = scales[1];
	scale.z = scales[2];


	bool ret = calibrate_accel(p_mpu, offset, scale,
			delay, flash_leds, setup_printf_P, setup_wait_key);
	cliSerial->println("Place gimbal level and press any key.");
	setup_wait_key();

	offsets[0] = offset.x;
	offsets[1] = offset.y;
	offsets[2] = offset.z;
	scales[0] = scale.x;
	scales[1] = scale.y;
	scales[2] = scale.z;

	enableMotorUpdates = true;

	return ret;
}

#elif defined ( IMU_AP )

void initResolutionDevider() {}
void gyroOffsetCalibration() {}

#endif

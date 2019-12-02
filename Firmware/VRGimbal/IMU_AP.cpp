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


#if defined( IMU_AP )

// set default sensor orientation (sensor upside)
void initSensorOrientationDefault() {

  // channel assignment
  sensorDef.Gyro[axisROLL].idx = 0;
  sensorDef.Gyro[axisPITCH].idx = 1;
  sensorDef.Gyro[axisYAW].idx = 2;

  sensorDef.Acc[axisROLL].idx = 1;     // y
  sensorDef.Acc[axisPITCH].idx = 0;    // x
  sensorDef.Acc[axisYAW].idx = 2;      // z

  // direction
  sensorDef.Gyro[axisROLL].dir = 1;
  sensorDef.Gyro[axisPITCH].dir = 1;
  sensorDef.Gyro[axisYAW].dir = 1;

  sensorDef.Acc[axisROLL].dir = 1;
  sensorDef.Acc[axisPITCH].dir = 1;
  sensorDef.Acc[axisYAW].dir = 1;

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

}

void setACCFastMode (bool fastMode) {

}


void
print_accel_offsets_and_scaling(void)
{
    Vector3f accel_offsets = ins.get_accel_offsets();
    Vector3f accel_scale = ins.get_accel_scale();
    cliSerial->print("A_off: ");
    cliSerial->print((float)accel_offsets.x); cliSerial->print(" ");   // Pitch
    cliSerial->print((float)accel_offsets.y); cliSerial->print(" ");   // Roll
    cliSerial->print((float)accel_offsets.z); cliSerial->print(" ");   // YAW
    cliSerial->print((float)accel_scale.x); cliSerial->print(" ");    // Pitch
    cliSerial->print((float)accel_scale.y); cliSerial->print(" ");    // Roll
    cliSerial->print((float)accel_scale.z); cliSerial->print(" ");    // YAW
    cliSerial->println();
}

void
print_gyro_offsets(void)
{
    Vector3f gyro_offsets = ins.get_gyro_offsets();
    cliSerial->print("G_off: ");
    cliSerial->print((float)gyro_offsets.x); cliSerial->print(" ");
	cliSerial->print((float)gyro_offsets.y); cliSerial->print(" ");
	cliSerial->print((float)gyro_offsets.z); cliSerial->print(" ");
	cliSerial->println();
}

void report_ins()
{
    cliSerial->printf_P(PSTR("INS\n"));
    print_divider();

    print_gyro_offsets();
    print_accel_offsets_and_scaling();
    print_blanks(2);
}


void initIMU() {

	//isr_registry.init();
	//scheduler.init(&isr_registry);

#ifdef GIMBAL_IMU_SPI

	//cliSerial->print("Init SPI1\r\n");
	//spi1.begin(SPI_1_125MHZ, MSBFIRST, 0);
	//delay(500);

	cliSerial->print("INS Init\r\n");
	ins.init(AP_InertialSensor::COLD_START, AP_InertialSensor::RATE_100HZ, delay, flash_leds, NULL, &Serial);
	//ins.calibrate_accel(delay, flash_leds, setup_printf_P, setup_wait_key);
	//ins.init(AP_InertialSensor::COLD_START, AP_InertialSensor::RATE_100HZ, delay, flash_leds, NULL, &Serial);


#else
	//cliSerial->print("I2C Begin\r\n");
	//i2c2.begin();
	//delay(500);


	cliSerial->print("INS Init\r\n");


	if (config.recalibrateOnStartup) {
		ins.init(AP_InertialSensor::COLD_START, AP_InertialSensor::RATE_1000HZ, delay, flash_leds, NULL, cliSerial);

		ins.calibrate_accel(delay, flash_leds, setup_printf_P, setup_wait_key);
		cliSerial->println("Place gimbal level and press any key.");
		setup_wait_key();
	} else {
		ins.init(AP_InertialSensor::COLD_START, AP_InertialSensor::RATE_1000HZ, delay, flash_leds, NULL, cliSerial);
	}

#endif



	ahrs.init();
	ahrs.set_fast_gains(true);

#ifdef GIMBAL_ENABLE_COMPASS
	cliSerial->print("COMPASS Init\r\n");
	compass.set_orientation(MAG_ORIENTATION);                                                   // set compass's orientation on aircraft
	if (!compass.init() || !compass.read()) {
		cliSerial->println("COMPASS INIT ERROR");
		ahrs.set_compass(NULL);
	} else {
		ahrs.set_compass(&compass);
	}
#else
	//ahrs.set_compass(NULL);
#endif


	report_ins();

	cliSerial->print("Sensors Init END\r\n");
}

// Rotate Estimated vector(s) with small angle approximation, according to the gyro data
// needs angle in radian units !
void rotateV(struct fp_vector *v,float* delta) {

}

void readGyros() {
	//ins.read();
}

void readACC(axisDef axis) {

}

void updateGyroAttitude(){
#ifdef GIMBAL_ENABLE_COMPASS
//	static uint32_t compass_time = 0;
	compass.read();

//	if ((millis() - compass_time > 100) &&
//			compass.read()) {
//			compass.calculate(ahrs.get_dcm_matrix());
//			// read compass at 10Hz
//			compass_time = millis();
//		}
#endif

	//ahrs.update();
}

void updateACC(){


}


void updateACCAttitude(){

}


void getAttiduteAngles() {

  // attitude of the estimated vector

  //angle[axisROLL]  = (int32_t) ( 180.0 * ahrs.roll * 1000.0 / PI );
  //angle[axisPITCH] = (int32_t) ( 180.0 * ahrs.pitch * 1000.0  / PI );
  //angle[axisYAW] = (int32_t) ( 180.0 * ahrs.yaw * 1000.0  / PI );

	angle[axisROLL]  = (int32_t)  (ahrs.roll_sensor * (ANGLE_PRECISION / 100));
	angle[axisPITCH] = (int32_t)  (ahrs.pitch_sensor * (ANGLE_PRECISION / 100));
	angle[axisYAW] = (int32_t)  (ahrs.yaw_sensor * (ANGLE_PRECISION / 100));

}


#endif

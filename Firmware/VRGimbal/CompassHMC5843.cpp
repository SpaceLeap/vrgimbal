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

/*
 * CompassHMC5843.cpp
 *
 *  Created on: 28/ago/2013
 *      Author: Murtas Matteo
 */

#include "CompassHMC5843.h"

// compass product id
#define AP_COMPASS_TYPE_UNKNOWN  0x00
#define AP_COMPASS_TYPE_HIL      0x01
#define AP_COMPASS_TYPE_HMC5843  0x02
#define AP_COMPASS_TYPE_HMC5883L 0x03


#define COMPASS_ADDRESS      0x1E // 0x3C
#define ConfigRegA           0x00
#define ConfigRegB           0x01
#define magGain              0x20
#define PositiveBiasConfig   0x11
#define NegativeBiasConfig   0x12
#define NormalOperation      0x10
#define ModeRegister         0x02
#define ContinuousConversion 0x00
#define SingleConversion     0x01

// ConfigRegA valid sample averaging for 5883L
#define SampleAveraging_1    0x00
#define SampleAveraging_2    0x01
#define SampleAveraging_4    0x02
#define SampleAveraging_8    0x03

// ConfigRegA valid data output rates for 5883L
#define DataOutputRate_0_75HZ 0x00
#define DataOutputRate_1_5HZ  0x01
#define DataOutputRate_3HZ    0x02
#define DataOutputRate_7_5HZ  0x03
#define DataOutputRate_15HZ   0x04
#define DataOutputRate_30HZ   0x05
#define DataOutputRate_75HZ   0x06


Compass_HMC5843::Compass_HMC5843(HardwareI2C *i2c_d, BetterStream *ser_port) :
_I2Cx(i2c_d)
{
	serPort = ser_port;
	_retry_time = 0;
	_learning = false;
	//_orientation = ROTATION_NONE;


    axisIndex[0] = 0;
    axisIndex[1] = 1;
    axisIndex[2] = 2;

    axisSign[0] = 1;
    axisSign[1] = 1;
    axisSign[2] = 1;

    magLPF[0] = 0.0f;
    magLPF[1] = 0.0f;
    magLPF[2] = 0.0f;

}

Compass_HMC5843::~Compass_HMC5843() {
	// TODO Auto-generated destructor stub
}


// read_register - read a register value
bool Compass_HMC5843::read_register(uint8_t address, uint8_t *value)
{
	serPort->printf("Read %02x ", address);
   if (_I2Cx->read((uint8_t)COMPASS_ADDRESS, address, 1, value) != 0) {

	   serPort->println("FAILED");
	  healthy = false;
	  return false;
   }
   serPort->println("OK");
   return true;
}

// write_register - update a register value
bool Compass_HMC5843::write_register(uint8_t address, uint8_t value)
{
	serPort->printf("Write %02x %02x", address, value);
   if (_I2Cx->write((uint8_t)COMPASS_ADDRESS, address, value) != 0) {
	   serPort->println("FAILED");


	  healthy = false;
	  return false;
   }
   serPort->println("OK");
   return true;
}


// Read Sensor data
bool Compass_HMC5843::read_raw()
{
   uint8_t buff[6];

   //serPort->println("Read RAW ");

  if (_I2Cx->read((uint8_t)COMPASS_ADDRESS, (uint8_t)0x03, (uint8_t)6, (uint8_t *)buff) != 0) {
	  healthy = false;
	  return false;
   }


   int16_t rx, ry, rz;
   rx = (int16_t)(buff[0] << 8) | buff[1];
   if (product_id == AP_COMPASS_TYPE_HMC5883L) {
	  rz = (int16_t)(buff[2] << 8) | buff[3];
	  ry = (int16_t)(buff[4] << 8) | buff[5];
   } else {
	  ry = (int16_t)(buff[2] << 8) | buff[3];
	  rz = (int16_t)(buff[4] << 8) | buff[5];
   }
   if (rx == -4096 || ry == -4096 || rz == -4096) {
	  // no valid data available
	  return false;
   }

   _raw.x = rx;  // -rx;
   _raw.y = ry; //ry;
   _raw.z = rz;  //-rz;


   if (_learning)
   {
	   _max.x = max(_max.x, _raw.x);
	   _max.y = max(_max.y, _raw.y);
	   _max.z = max(_max.z, _raw.z);

	   _min.x = min(_min.x, _raw.x);
	   _min.y = min(_min.y, _raw.y);
	   _min.z = min(_min.z, _raw.z);

	   _ref = (_max + _min) / 2;

		if (_ref.length() > 0)
			_calibrated = true;
   }

   return true;
}


/*
  re-initialise after a IO error
 */
bool Compass_HMC5843::re_initialise()
{
   if (! write_register(ConfigRegA, _base_config) ||
	   ! write_register(ConfigRegB, magGain) ||
	   ! write_register(ModeRegister, ContinuousConversion))
	  return false;
   return true;
}


// Public Methods //////////////////////////////////////////////////////////////
bool
Compass_HMC5843::init(void)
{
  delay(10);

  // determine if we are using 5843 or 5883L
  if (! write_register(ConfigRegA, SampleAveraging_8<<5 | DataOutputRate_75HZ<<2 | NormalOperation)) {
	  serPort->println("Write config error\r\n");
	 healthy = false;
	 return false;
  }

  if (! read_register(ConfigRegA, &_base_config)) {
	  serPort->println("Read config error\r\n");
	 healthy = false;
	 return false;
  }
  if ( _base_config == (SampleAveraging_8<<5 | DataOutputRate_75HZ<<2 | NormalOperation)) {
	 // a 5883L supports the sample averaging config
	 product_id = AP_COMPASS_TYPE_HMC5883L;
	 serPort->println("AP_COMPASS_TYPE_HMC5883L");
  } else if (_base_config == (NormalOperation | DataOutputRate_75HZ<<2)) {
      product_id = AP_COMPASS_TYPE_HMC5843;
  	  serPort->println("MAG5843");
  } else {
	 // not behaving like either supported compass type
	 serPort->println("NO MAG");
	 return false;
  }


  // leave test mode
  if (!re_initialise()) {
	 return false;
  }

  _initialised = true;

	// perform an initial read
	healthy = true;
	read();

    return true;
}


// Read Sensor data
bool Compass_HMC5843::read()
{
   if (!_initialised) {
	  // someone has tried to enable a compass for the first time
	  // mid-flight .... we can't do that yet (especially as we won't
	  // have the right orientation!)
	  return false;
   }
   if (!healthy) {
	  if (millis() < _retry_time) {
		 return false;
	  }
	  if (!re_initialise()) {
		 _retry_time = millis() + 1000;
		 _I2Cx->setSpeed(false);
		 return false;
	  }
   }

   read_raw();

   last_update = micros();  // record time of update

   if (_calibrated)
   {
	   mag_x = (float) (_raw.x - _ref.x) / (float) (_max.x - _ref.x);
	   mag_y = (float) (_raw.y - _ref.y) / (float) (_max.y - _ref.y);
	   mag_z = (float) (_raw.z - _ref.z) / (float) (_max.z - _ref.z);
   } else {
	   mag_x = _raw.x;
	   mag_y = _raw.y;
	   mag_z = _raw.z;
   }

   // rotate to the desired orientation
   Vector3f rot_mag = Vector3f(mag_x,mag_y,mag_z);

   rot_mag.normalize();

   //rot_mag.rotate(_orientation);

   //rot_mag += _offset.get();
   mag_x = 255 * rot_mag.x;
   mag_y = 255 * rot_mag.y;
   mag_z = 255 * rot_mag.z;
   healthy = true;


   return true;
}

//// set orientation
//void
//Compass_HMC5843::set_orientation(enum Rotation rotation)
//{
//   _orientation = rotation;
//}

void Compass_HMC5843::set_transformation(int * sign, int * index)
{
	for (int i = 0; i < 3; i++)
	{
		axisIndex[i] = index[i];
		axisSign[i] = sign[i];
	}
}

void Compass_HMC5843::get_transformation(int * sign, int * index)
{
	for (int i = 0; i < 3; i++)
	{
		index[i] = axisIndex[i];
		sign[i] = axisSign[i];
	}
}

Vector3f Compass_HMC5843::get_mag()
{
	const double factor = 0.1;

	magLPF[0] = (1 - factor) * magLPF[0] + factor * mag_x;
	magLPF[1] = (1 - factor) * magLPF[1] + factor * mag_y;
	magLPF[2] = (1 - factor) * magLPF[2] + factor * mag_z;

	Vector3f v;
	v.x = axisSign[0] * magLPF[axisIndex[0]];
	v.y = axisSign[1] * magLPF[axisIndex[1]];
	v.z = axisSign[2] * magLPF[axisIndex[2]];

	return v;
}

float Compass_HMC5843::calculate_yaw(float deg_roll, float deg_pitch)
{
	float roll = deg_roll * PI / 180.0;
	float pitch = deg_pitch * PI / 180.0;


	Vector3f mag = get_mag();

    double cos_roll = 1.0; //cos(pitch);
    double sin_roll = 0.0; //sin(pitch);
    double cos_pitch = 1.0; //cos(roll);
    double sin_pitch = 0.0; //sin(roll);

    // Tilt compensated magnetic field X component:
    float headX = mag.x * cos_pitch + mag.y * sin_roll * sin_pitch + mag.z * cos_roll * sin_pitch;
    // Tilt compensated magnetic field Y component:
    float headY = mag.y * cos_roll - mag.z * sin_roll;
    // magnetic heading
    //double hd = Math.Atan2(-headY, headX);
    float yaw = atan2(headY, headX);

	return yaw;
}


void Compass_HMC5843::set_calibration(Vector3i min, Vector3i max)
{
	_max = max;
	_min = min;
	_ref = (_max + _min) / 2;

	if (_ref.length() > 0)
		_calibrated = true;
}

void Compass_HMC5843::get_calibration(Vector3i *min, Vector3i *max)
{
	*min = _min;
	*max = _max;
}

void
Compass_HMC5843::set_learn(bool learn)
{
	_learning = learn;
}

bool
Compass_HMC5843::get_learn()
{
	return _learning;
}

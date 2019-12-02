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
 * CompassHMC5843.h
 *
 *  Created on: 28/ago/2013
 *      Author: Murtas Matteo
 */

#ifndef COMPASSHMC5843_H_
#define COMPASSHMC5843_H_

#include "wirish.h"
#include "AP_Math.h"

class Compass_HMC5843 {
public:
	Compass_HMC5843(HardwareI2C *i2c_d, BetterStream *ser_port);
	virtual ~Compass_HMC5843();

	bool read_raw();

	bool init(void);
	bool read();
	Vector3f get_mag();
	Vector3i get_raw() { return _raw; }  //lettura raw

	//void set_orientation(enum Rotation rotation);
	void set_transformation(int * sign, int * index);
	void get_transformation(int * sign, int * index);
	float calculate_yaw(float deg_roll, float deg_pitch);


	void set_calibration(Vector3i min, Vector3i max);
	void get_calibration(Vector3i *min, Vector3i *max);
	virtual void set_learn(bool learn);
	bool get_learn();

	int16_t			product_id;     /// product id
	int16_t         mag_x;          ///< magnetic field strength along the X axis
	int16_t         mag_y;          ///< magnetic field strength along the Y axis
	int16_t         mag_z;          ///< magnetic field strength along the Z axis
	uint32_t        last_update;    ///< micros() time of last update
	bool			healthy;        ///< true if last read OK


	Vector3i _raw;  //lettura raw


protected:
	// read_register - read a register value
	bool read_register(uint8_t address, uint8_t *value);
	// write_register - update a register value
	bool write_register(uint8_t address, uint8_t value);

	bool re_initialise();

	BetterStream * serPort;
	HardwareI2C *_I2Cx;



    bool _initialised;
    bool _calibrated;
	uint8_t _base_config;

    Vector3i _max;
    Vector3i _min;
    Vector3i _ref;

    //enum Rotation		_orientation;

    uint32_t _retry_time;

    bool _learning;

    int axisIndex[3];
    int axisSign[3];
    float magLPF[3];
};

#endif /* COMPASSHMC5843_H_ */

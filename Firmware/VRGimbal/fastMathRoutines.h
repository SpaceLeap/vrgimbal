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

// Org ATAN2 ~200us, fastAtan2 ~128us, ultraFastAtan2 ~92us

#ifndef _FASTMATHROUTINES_H_
#define _FASTMATHROUTINES_H_

#include "wirish.h"

// Fast arctan2
float ultraFastAtan2(float y, float x);

float fastAtan2(float y, float x); // in deg

//inline
int16_t _atan2(float y, float x);

int8_t sgn(int val);
double sgn(double val);


uint32_t measure_micro_delay(uint32_t microStart, uint32_t microEnd);

inline float normalize_yaw(float y)
{
	if (y > 180.0f) {
		return y - 360.0f;
	} else if (y < -180.0f) {
		return y + 360.0f;
	} else {
		return y;
	}
}

inline int32_t inormalize_yaw(int32_t y)
{
	if (y > 180000) {
		return y - 360000;
	} else if (y < -180000) {
		return y + 360000;
	} else {
		return y;
	}
}

//inline int32_t inormalize_val(int32_t y, int32_t period)
//{
//	if (y > period / 2) {
//		return y - period;
//	} else if (y < - period / 2) {
//		return y + period;
//	} else {
//		return y;
//	}
//}

int32_t inormalize_val(int32_t y, int32_t period);


inline int32_t constrain_int32(int32_t x , int32_t l, int32_t h) {
  if (x <= l) {
    return l;
  } else if (x >= h) {
    return h;
  } else {
    return x;
  }
}

//***************************************************************
// Efficient approximations for the arctangent functions,
// Rajan, S. Sichun Wang Inkol, R. Joyal, A., May 2006
//***************************************************************
inline float Rajan_FastArcTan(float x);

// atan2 for all quadrants by A. Hahn
inline float Rajan_FastArcTan2(float y, float x);

//// atan2 returnig degrees * 100
//int16_t Rajan_FastArcTan2_deg100(float y, float x);
// atan2 returnig degrees * 1000
int32_t Rajan_FastArcTan2_deg1000(float y, float x);

/************************/
/* LP Filter            */
/************************/
inline void utilLP_float(float * q, float i, float coeff) {
  //*q += (i-*q)*coeff;
  *q = *q * (1.0f-coeff) + i * coeff;  // this one seems to be a bit faster on ATMega
}

// CRC definitions
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
typedef uint8_t crc;
crc crcSlow(uint8_t const message[], int nBytes);

#endif // _FASTMATHROUTINES_H_

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
// Fast arctan2
#include "fastMathRoutines.h"

uint32_t measure_micro_delay(uint32_t microStart, uint32_t microEnd)
{
	if (microStart > microEnd)
	{
		//c'è stato un overflow, devo correggere
		uint32_t halflap = 0xFFFFFFFF - microStart;
		return (microEnd + halflap);
	}
	return (microEnd - microStart);
}


int32_t inormalize_val(int32_t y, int32_t period)
{
	if (y > period) {
		y = y % period;
	} else if (y < - period) {
		y = -y;
		y = y % period;
		y = period - y;
	}

	if (y > period / 2) {
		return y - period;
	} else if (y < - period / 2) {
		return y + period;
	} else {
		return y;
	}
}

float ultraFastAtan2(float y, float x)
{
  float angle; 
  float coeff_1 = PI/4;
   float coeff_2 = 3*coeff_1;
   float abs_y = fabs(y)+1e-10 ;     // kludge to prevent 0/0 condition
   if (x>=0)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }
   if (y < 0)
   return(-angle* (180.0f / PI));     // negate if in quad III or IV
   else
   return(angle* (180.0f / PI));
}

#define fp_is_neg(val) ((((byte*)&val)[3] & 0x80) != 0)

float fastAtan2(float y, float x) // in deg
{
  float z = y / x;
  int16_t zi = abs(int16_t(z * 100));
  int8_t y_neg = fp_is_neg(y);
  if ( zi < 100 ){
    if (zi > 10)
      z = z / (1.0f + 0.28f * z * z);
    if (fp_is_neg(x)) {
      if (y_neg) z -= PI;
      else z += PI;
    }
  } else {
    z = (PI / 2.0f) - z / (z * z + 0.28f);
    if (y_neg) z -= PI;}
  z *= (180.0f / PI);
  return z;
}

//inline
int16_t _atan2(float y, float x){
  float z = y / x;
  int16_t zi = abs(int16_t(z * 100)); 
  int8_t y_neg = fp_is_neg(y);
  if ( zi < 100 ){
    if (zi > 10) 
     z = z / (1.0f + 0.28f * z * z);
   if (fp_is_neg(x)) {
     if (y_neg) z -= PI;
     else z += PI;
   }
  } else {
   z = (PI / 2.0f) - z / (z * z + 0.28f);
   if (y_neg) z -= PI;
  }
  z *= (180.0f / PI * 100); 
  return z;
}

int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

double sgn(double val) {
  if (val < 0.0f) return -1.0f;
  if (val==0.0f) return 0.0f;
  return 1.0f;
}

//inline int32_t constrain_int32(int32_t x , int32_t l, int32_t h) {
//  if (x <= l) {
//    return l;
//  } else if (x >= h) {
//    return h;
//  } else {
//    return x;
//  }
//}

//***************************************************************
//    Efficient approximations for the arctangent function ,
// Rajan, S. Sichun Wang Inkol, R. Joyal, A., May 2006
//***************************************************************
inline float Rajan_FastArcTan(float x) {
  return PI/4.0*x - x*(fabs(x) - 1)*(0.2447 + 0.0663*fabs(x));
}

// atan2 for all quadrants by A. Hahn
inline float Rajan_FastArcTan2(float y, float x) {

  uint8_t qCode;
  const float pi_2 = PI/2.0;
  float q;
  float z;

  // 6 us
  bool swap45 = (fabs(y) > fabs(x));
    
  // 22us
  if ((y >= 0) && (x >= 0)) { qCode = 0; }
  if ((y >= 0) && (x <= 0)) { qCode = 1; }
  if ((y <= 0) && (x <= 0)) { qCode = 2; }
  if ((y <= 0) && (x >= 0)) { qCode = 3; }

  // 54 us
  if (swap45) {
    q = x / y;
  } else {
    q = y / x;
  }

  // 92 us
  z = Rajan_FastArcTan(q);

  if (swap45) {
    switch (qCode) {
      case 0: z = pi_2 - z;  break;
      case 1: z = pi_2 - z;  break;
      case 2: z = -pi_2 - z; break;
      case 3: z = -pi_2 - z; break;
    }
  } else {
    switch (qCode) {    
      case 0: z = z;         break;
      case 1: z = PI + z;    break;
      case 2: z = -PI + z;   break;
      case 3: z = z;         break;
    }
  }
  
  return z;
}

//// atan2 returnig degrees * 100
//int16_t Rajan_FastArcTan2_deg100(float y, float x) {
//  return 180/PI * 100 * Rajan_FastArcTan2(y, x);
//}

// atan2 returnig degrees * 1000
int32_t Rajan_FastArcTan2_deg1000(float y, float x) {
  return 180/PI * 1000 * Rajan_FastArcTan2(y, x);
}

/************************/
/* LP Filter            */
/************************/
//inline void utilLP_float(float * q, float i, float coeff) {
//  *q += (i-*q)*coeff;
//}




/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 * Thanks to
 * http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
 *
 */
 
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))

crc crcSlow(uint8_t const message[], int nBytes)
{
    crc  remainder = 0;	


    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
      
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}   /* crcSlow() */

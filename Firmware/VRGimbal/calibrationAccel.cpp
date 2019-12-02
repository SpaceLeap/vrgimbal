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
 * calibrationAccel.cpp
 *
 *  Created on: 17/ott/2013
 *      Author: Murtas Matteo
 */


#include "mpu6050.h"

#include <math.h>
#include <string.h>

#include "AP_Math.h"

#include "main.h"

#define SAMPLE_UNIT 1


bool _calibrate_accel( Vector3f accel_sample[6], Vector3f& accel_offsets, Vector3f& accel_scale );
void _calibrate_find_delta(float dS[6], float JS[6][6], float delta[6]);
void _calibrate_update_matrices(float dS[6], float JS[6][6], float beta[6], float data[3]);
void _calibrate_reset_matrices(float dS[6], float JS[6][6]);

// calibrate_accel - perform accelerometer calibration including providing user instructions and feedback
// Gauss-Newton accel calibration routines borrowed from Rolfe Schmidt
// blog post describing the method: http://chionophilous.wordpress.com/2011/10/24/accelerometer-calibration-iv-1-implementing-gauss-newton-on-an-atmega/
// original sketch available at http://rolfeschmidt.com/mathtools/skimetrics/adxl_gn_calibration.pde
bool calibrate_accel(MPU6050 * mpu, Vector3f & p_accel_offset, Vector3f & p_accel_scale,
		void (*delay_cb)(unsigned long t), void (*flash_leds_cb)(bool on),
                                        void (*send_msg)(const prog_char_t *, ...),
                                        void (*wait_key)(void))
{
    Vector3f samples[6];
    Vector3f new_offsets;
    Vector3f new_scaling;
    Vector3f orig_offset;
    Vector3f orig_scale;

    // backup original offsets and scaling
    orig_offset = p_accel_offset;
    orig_scale = p_accel_scale;

    // clear accelerometer offsets and scaling
    p_accel_offset = Vector3f(0,0,0);
    p_accel_scale = Vector3f(1,1,1);

    // capture data from 6 positions
    for (int8_t i=0; i<6; i++) {
        const prog_char_t *msg;

        // display message to user
        switch ( i ) {
            case 0:
                msg = PSTR("level");
                break;
            case 1:
                msg = PSTR("on it's left side");
                break;
            case 2:
                msg = PSTR("on it's right side");
                break;
            case 3:
                msg = PSTR("nose down");
                break;
            case 4:
                msg = PSTR("nose up");
                break;
            default:    // default added to avoid compiler warning
            case 5:
                msg = PSTR("on it's back");
                break;
        }
        send_msg(PSTR("Place APM %s and press any key.\n"), msg);

        wait_key();

        // clear out any existing samples from ins
        int32_t sum[3];
        sum[0] = 0;
        sum[1] = 0;
        sum[2] = 0;
        int32_t cnt = 0;

        // wait until we have 32 samples
        while( cnt < 32 * SAMPLE_UNIT ) {
        	//read();
        	//sommo i valori letti
        	for (int j = 0; j < 3; j++)
        	{
        		sum[j] += mpu->getAccelerationN(j);
        	}
        	cnt++;
            delay(1);
        }

        //faccio la media
        // capture sample
        samples[i].x = MPU6000_ACCEL_SCALE_1G * (float) sum[0] / (float) cnt;
        samples[i].y = MPU6000_ACCEL_SCALE_1G * (float) sum[1] / (float) cnt;
        samples[i].z = MPU6000_ACCEL_SCALE_1G * (float) sum[2] / (float) cnt;

        cliSerial->printf(("Sample %d: "), i);
        cliSerial->print(samples[i].x);cliSerial->print(" ");
        cliSerial->print(samples[i].y);cliSerial->print(" ");
        cliSerial->print(samples[i].z);cliSerial->print(" ");
        cliSerial->println();
    }

    // run the calibration routine
    if( _calibrate_accel(samples, new_offsets, new_scaling) ) {
        send_msg(PSTR("Calibration successful ("));


        cliSerial->print(new_offsets.x);cliSerial->print(" ");
        cliSerial->print(new_offsets.y);cliSerial->print(" ");
        cliSerial->print(new_offsets.z);cliSerial->print(" ");
        cliSerial->print(new_scaling.x);cliSerial->print(" ");
        cliSerial->print(new_scaling.y);cliSerial->print(" ");
        cliSerial->print(new_scaling.z);
        cliSerial->println(")");

        // set and save calibration
        p_accel_offset = new_offsets;
        p_accel_scale = new_scaling;
        //_save_parameters();
        return true;
    }

    send_msg(PSTR("Calibration failed (")); //%.1f %.1f %.1f %.1f %.1f %.1f)\n"),
    cliSerial->print(new_offsets.x);cliSerial->print(" ");
    cliSerial->print(new_offsets.y);cliSerial->print(" ");
    cliSerial->print(new_offsets.z);cliSerial->print(" ");
    cliSerial->print(new_scaling.x);cliSerial->print(" ");
    cliSerial->print(new_scaling.y);cliSerial->print(" ");
    cliSerial->print(new_scaling.z);cliSerial->print(")\r\n");
    // restore original scaling and offsets
    p_accel_offset = orig_offset;
    p_accel_scale = orig_scale;
    return false;
}

// _calibrate_model - perform low level accel calibration
// accel_sample are accelerometer samples collected in 6 different positions
// accel_offsets are output from the calibration routine
// accel_scale are output from the calibration routine
// returns true if successful
bool _calibrate_accel( Vector3f accel_sample[6], Vector3f& accel_offsets, Vector3f& accel_scale )
{
    int16_t i;
    int16_t num_iterations = 0;
    float eps = 0.000000001;
    float change = 100.0;
    float data[3];
    float beta[6];
    float delta[6];
    float ds[6];
    float JS[6][6];
    bool success = true;

    // reset
    beta[0] = beta[1] = beta[2] = 0;
    beta[3] = beta[4] = beta[5] = 1.0f/GRAVITY;

    while( num_iterations < 20 && change > eps ) {
        num_iterations++;

        _calibrate_reset_matrices(ds, JS);

        for( i=0; i<6; i++ ) {
            data[0] = accel_sample[i].x;
            data[1] = accel_sample[i].y;
            data[2] = accel_sample[i].z;
            _calibrate_update_matrices(ds, JS, beta, data);
        }

        _calibrate_find_delta(ds, JS, delta);

        change =    delta[0]*delta[0] +
                    delta[0]*delta[0] +
                    delta[1]*delta[1] +
                    delta[2]*delta[2] +
                    delta[3]*delta[3] / (beta[3]*beta[3]) +
                    delta[4]*delta[4] / (beta[4]*beta[4]) +
                    delta[5]*delta[5] / (beta[5]*beta[5]);

        for( i=0; i<6; i++ ) {
            beta[i] -= delta[i];
        }
    }

    // copy results out
    accel_scale.x = beta[3] * GRAVITY;
    accel_scale.y = beta[4] * GRAVITY;
    accel_scale.z = beta[5] * GRAVITY;
    accel_offsets.x = beta[0] * accel_scale.x;
    accel_offsets.y = beta[1] * accel_scale.y;
    accel_offsets.z = beta[2] * accel_scale.z;

    // sanity check scale
    if( accel_scale.is_nan() || fabsf(accel_scale.x-1.0f) > 0.1f || fabsf(accel_scale.y-1.0f) > 0.1f || fabsf(accel_scale.z-1.0f) > 0.1f ) {
        success = false;
    }
    // sanity check offsets (2.0 is roughly 2/10th of a G, 5.0 is roughly half a G)
    if( accel_offsets.is_nan() || fabsf(accel_offsets.x) > 2.0f || fabsf(accel_offsets.y) > 2.0f || fabsf(accel_offsets.z) > 3.0f ) {
        success = false;
    }

    // return success or failure
    return success;
}

void _calibrate_update_matrices(float dS[6], float JS[6][6], float beta[6], float data[3])
{
    int16_t j, k;
    float dx, b;
    float residual = 1.0;
    float jacobian[6];

    for( j=0; j<3; j++ ) {
        b = beta[3+j];
        dx = (float)data[j] - beta[j];
        residual -= b*b*dx*dx;
        jacobian[j] = 2.0f*b*b*dx;
        jacobian[3+j] = -2.0f*b*dx*dx;
    }

    for( j=0; j<6; j++ ) {
        dS[j] += jacobian[j]*residual;
        for( k=0; k<6; k++ ) {
            JS[j][k] += jacobian[j]*jacobian[k];
        }
    }
}


// _calibrate_reset_matrices - clears matrices
void _calibrate_reset_matrices(float dS[6], float JS[6][6])
{
    int16_t j,k;
    for( j=0; j<6; j++ ) {
        dS[j] = 0.0f;
        for( k=0; k<6; k++ ) {
            JS[j][k] = 0.0f;
        }
    }
}

void _calibrate_find_delta(float dS[6], float JS[6][6], float delta[6])
{
    //Solve 6-d matrix equation JS*x = dS
    //first put in upper triangular form
    int16_t i,j,k;
    float mu;

    //make upper triangular
    for( i=0; i<6; i++ ) {
        //eliminate all nonzero entries below JS[i][i]
        for( j=i+1; j<6; j++ ) {
            mu = JS[i][j]/JS[i][i];
            if( mu != 0.0 ) {
                dS[j] -= mu*dS[i];
                for( k=j; k<6; k++ ) {
                    JS[k][j] -= mu*JS[k][i];
                }
            }
        }
    }

    //back-substitute
    for( i=5; i>=0; i-- ) {
        dS[i] /= JS[i][i];
        JS[i][i] = 1.0f;

        for( j=0; j<i; j++ ) {
            mu = JS[i][j];
            dS[j] -= mu*dS[i];
            JS[i][j] = 0.0f;
        }
    }

    for( i=0; i<6; i++ ) {
        delta[i] = dS[i];
    }
}

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
 * calibrationRoutines.h
 *
 *  Created on: 30/set/2013
 *      Author: Murtas Matteo
 */


 struct LSQIntermediate
 {
	 float x_sumplain ;
	 float x_sumsq ;
	 float x_sumcube ;

	 float y_sumplain ;
	 float y_sumsq ;
	 float y_sumcube ;

	 float z_sumplain ;
	 float z_sumsq ;
	 float z_sumcube ;

	 float xy_sum ;
	 float xz_sum ;
	 float yz_sum ;

	 float x2y_sum ;
	 float x2z_sum ;
	 float y2x_sum ;
	 float y2z_sum ;
	 float z2x_sum ;
	 float z2y_sum ;

	 unsigned int size;
 };



 int sphere_fit_least_squares(const float x[], const float y[], const float z[],
                              unsigned int size, unsigned int max_iterations, float delta, float *sphere_x, float *sphere_y, float *sphere_z, float *sphere_radius);


 void lsq_init(LSQIntermediate * pLSQ);
 unsigned int lsq_accumulate(LSQIntermediate * pLSQ, float x, float y, float z);
 void lsq_calculate(LSQIntermediate * pLSQ, unsigned int max_iterations, float delta, float *sphere_x, float *sphere_y, float *sphere_z, float *sphere_radius);

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
 * realtimeStatistics.h
 *
 *  Created on: 01/ott/2013
 *      Author: Murtas Matteo
 */

#ifndef REALTIMESTATISTICS_H_
#define REALTIMESTATISTICS_H_

class realtimeStatistics {
public:
	realtimeStatistics();
	virtual ~realtimeStatistics();

	void clear();
	void append(float v);
	float mean();
	float sqdev();
	float stddev();
	float vmin();
	float vmax();
protected:
	unsigned int m_count;
	float m_mean;
	float m_sqdev;
	float m_min;
	float m_max;
};

#endif /* REALTIMESTATISTICS_H_ */

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
 * realtimeStatistics.cpp
 *
 *  Created on: 01/ott/2013
 *      Author: Murtas Matteo
 */

#include "realtimeStatistics.h"
#include <math.h>

realtimeStatistics::realtimeStatistics() {
	m_count = 0;
	m_mean = 0.0f;
	m_sqdev = 0.0f;

}

realtimeStatistics::~realtimeStatistics() {

}

void realtimeStatistics::clear()
{
	m_count = 0;
	m_mean = 0.0f;
	m_sqdev = 0.0f;
	m_min = 0.0f;
	m_max = 0.0f;
}

void realtimeStatistics::append(float v)
{
	m_count++;
	float m_prev = m_mean;
	m_mean = m_mean + (v - m_mean) / m_count;
	m_sqdev = m_sqdev + (v - m_mean) * (v - m_prev);
	if (m_count == 1)
	{
		m_min = v;
		m_max = v;
	} else {
		m_min = (v < m_min ? v : m_min);
		m_max = (v > m_max ? v : m_max);
	}
}

float realtimeStatistics::mean()
{
	return m_mean;
}

float realtimeStatistics::sqdev()
{
	return m_sqdev;
}

float realtimeStatistics::vmin()
{
	return m_min;
}

float realtimeStatistics::vmax()
{
	return m_max;
}


float realtimeStatistics::stddev()
{
	if ( m_count > 0)
		return sqrt(m_sqdev/ m_count);
	return 0.0f;
}

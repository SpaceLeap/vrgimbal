// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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

#ifndef _DEFINES_H
#define _DEFINES_H

// Just so that it's completely clear...
#define ENABLED			1
#define DISABLED		0

// this avoids a very common config error
#define ENABLE ENABLED
#define DISABLE DISABLED

#define LED_ON           HIGH
#define LED_OFF          LOW

#define WATCHDOG_TIME	WDT_24_SEC

#endif // _DEFINES_H

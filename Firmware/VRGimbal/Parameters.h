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

// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <AP_Common.h>
//#include "IO_Analog.h"
//#include "config.h"

// Global parameter class.
//
class Parameters {
public:
	// The version of the layout as described by the parameter enum.
	//
	// When changing the parameter enum in an incompatible fashion, this
	// value should be incremented by one.
	//
	// The increment will prevent old parameters from being used incorrectly
	// by newer code.
	//
	static const uint16_t k_format_version = 206;

	// The parameter software_type is set up solely for ground station use
	// and identifies the software type (eg ArduPilotMega versus ArduCopterMega)
	// GCS will interpret values 0-9 as ArduPilotMega.  Developers may use
	// values within that range to identify different branches.
	//
	static const uint16_t k_software_type = 10;		// 0 for APM trunk



	static const uint16_t k_space_end = 0xAA;		// to detect errors in reading the whole param space


	// Parameter identities.
	//
	// The enumeration defined here is used to ensure that every parameter
	// or parameter group has a unique ID number.	This number is used by
	// AP_Var to store and locate parameters in EEPROM.
	//
	// Note that entries without a number are assigned the next number after
	// the entry preceding them.	When adding new entries, ensure that they
	// don't overlap.
	//
	// Try to group related variables together, and assign them a set
	// range in the enumeration.	Place these groups in numerical order
	// at the end of the enumeration.
	//
	// WARNING: Care should be taken when editing this enumeration as the
	//			AP_Var load/save code depends on the values here to identify
	//			variables saved in EEPROM.
	//
	//
	enum {
	// Layout version number, always key zero.
	//
	k_param_format_version			=	0,	// 0	Not Used
	k_param_software_type			=	1,	// 1	Not Used


	k_param_ins, 						//2

	k_param_compass,	 			//3	Not Used

	k_param_ahrs,	 // AHRS group			// 4	Not Used



	k_param_space_end,								// 5	Not Used
	// 6	Not Used
	// 7	Not Used
	// 8	Not Used
	// 9	Not Used
	// 10	Not Used
	// 11	Not Used
	// 12	Not Used
	// 13	Not Used
	// 14	Not Used
	// 15	Not Used
	// 16	Not Used
	// 17	Not Used
	// 18	Not Used
	// 19	Not Used
	// 20	Not Used
	// 21	Not Used
	// 22	Not Used
	// 23	Not Used
	// 24	Not Used
	// 25	Not Used
	// 26	Not Used
	// 27	Not Used
	// 28	Not Used
	// 29	Not Used
	// 30	Not Used
	// 31	Not Used
	// 32	Not Used
	// 33	Not Used
	// 34	Not Used
	// 35	Not Used
	// 36	Not Used
	// 37	Not Used
	// 38	Not Used
	// 39	Not Used
	// 40	Not Used
	// 41	Not Used
	// 42	Not Used
	// 43	Not Used
	// 44	Not Used
	// 45	Not Used
	// 46	Not Used
	// 47	Not Used
	// 48	Not Used
	// 49	Not Used
	// 50	Not Used
	// 51	Not Used
	// 52	Not Used
	// 53	Not Used
	// 54	Not Used
	// 55	Not Used
	// 56	Not Used
	// 57	Not Used
	// 58	Not Used
	// 59	Not Used
	// 60	Not Used
	// 61	Not Used
	// 62	Not Used
	// 63	Not Used
	// 64	Not Used
	// 65	Not Used
	// 66	Not Used
	// 67	Not Used
	// 68	Not Used
	// 69	Not Used
	// 70	Not Used
	// 71	Not Used
	// 72	Not Used
	// 73	Not Used
	// 74	Not Used
	// 75	Not Used
	// 76	Not Used
	// 77	Not Used
	// 78	Not Used
	// 79	Not Used
	// 80	Not Used
	// 81	Not Used
	// 82	Not Used
	// 83	Not Used
	// 84	Not Used
	// 85	Not Used
	// 86	Not Used
	// 87	Not Used
	// 88	Not Used
	// 89	Not Used
	// 90	Not Used
	// 91	Not Used
	// 92	Not Used
	// 93	Not Used
	// 94	Not Used
	// 95	Not Used
	// 96	Not Used
	// 97	Not Used
	// 98	Not Used
	// 99	Not Used
	// 100	Not Used
	// 101	Not Used
	// 102	Not Used
	// 103	Not Used
	// 104	Not Used
	// 105	Not Used
	// 106	Not Used
	// 107	Not Used
	// 108	Not Used
	// 109	Not Used
	// 110	Not Used
	// 111	Not Used
	// 112	Not Used
	// 113	Not Used
	// 114	Not Used
	// 115	Not Used
	// 116	Not Used
	// 117	Not Used
	// 118	Not Used
	// 119	Not Used
	// 120	Not Used
	// 121	Not Used
	// 122	Not Used
	// 123	Not Used
	// 124	Not Used
	// 125	Not Used
	// 126	Not Used
	// 127	Not Used
	// 128	Not Used
	// 129	Not Used
	// 130	Not Used
	// 131	Not Used
	// 132	Not Used
	// 133	Not Used
	// 134	Not Used
	// 135	Not Used
	// 136	Not Used
	// 137	Not Used
	// 138	Not Used
	// 139	Not Used
	// 140	Not Used
	// 141	Not Used
	// 142	Not Used
	// 143	Not Used
	// 144	Not Used
	// 145	Not Used
	// 146	Not Used
	// 147	Not Used
	// 148	Not Used
	// 149	Not Used
	// 150	Not Used
	// 151	Not Used
	// 152	Not Used
	// 153	Not Used
	// 154	Not Used
	// 155	Not Used
	// 156	Not Used
	// 157	Not Used
	// 158	Not Used
	// 159	Not Used
	// 160	Not Used
	// 161	Not Used
	// 162	Not Used
	// 163	Not Used
	// 164	Not Used
	// 165	Not Used
	// 166	Not Used
	// 167	Not Used
	// 168	Not Used
	// 169	Not Used
	// 170	Not Used
	// 171	Not Used
	// 172	Not Used
	// 173	Not Used
	// 174	Not Used
	// 175	Not Used
	// 176	Not Used
	// 177	Not Used
	// 178	Not Used
	// 179	Not Used
	// 180	Not Used
	// 181	Not Used
	// 182	Not Used
	// 183	Not Used
	// 184	Not Used
	// 185	Not Used
	// 186	Not Used
	// 187	Not Used
	// 188	Not Used
	// 189	Not Used
	// 190	Not Used
	// 191	Not Used
	// 192	Not Used
	// 193	Not Used
	// 194	Not Used
	// 195	Not Used
	// 196	Not Used
	// 197	Not Used
	// 198	Not Used
	// 199	Not Used
	// 200	Not Used
	// 201	Not Used
	// 202	Not Used
	// 203	Not Used
	// 204	Not Used
	// 205	Not Used
	// 206	Not Used
	// 207	Not Used
	// 208	Not Used
	// 209	Not Used
	// 210	Not Used
	// 211	Not Used
	// 212	Not Used
	// 213	Not Used
	// 214	Not Used
	// 215	Not Used
	// 216	Not Used
	// 217	Not Used
	// 218	Not Used
	// 219	Not Used
	// 220	Not Used
	// 221	Not Used
	// 222	Not Used
	// 223	Not Used
	// 225	Not Used
	// 226	Not Used
	// 227	Not Used
	// 228	Not Used
	// 229	Not Used
	// 230	Not Used
	// 231	Not Used
	// 232	Not Used
	// 233	Not Used
	// 234	Not Used
	// 235	Not Used
	// 236	Not Used
	// 237	Not Used
	// 238	Not Used
	// 239	Not Used
	// 240	Not Used
	// 241	Not Used
	// 242	Not Used
	// 243	Not Used
	// 244	Not Used
	// 245	Not Used
	// 246	Not Used
	// 247	Not Used
	// 248	Not Used
	// 249	Not Used
	// 250	Not Used
	// 251	Not Used
	// 252	Not Used
	// 253	Not Used
	// 254	Not Used
	// 255	Reserved
	};

	AP_Int16	format_version;
	AP_Int8		software_type;

	AP_Int8		space_end;

	// Note: keep initializers here in the same order as they are declared above.
	Parameters()
	// variable					default
	//----------------------------------------
	{
	}
};

extern const AP_Param::Info        var_info[];

void print_param(ap_var_type type, AP_Param *ap);

#endif // PARAMETERS_H


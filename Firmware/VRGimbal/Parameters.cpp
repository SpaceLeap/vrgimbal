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

/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-


#include "main.h"
#ifdef ENABLE_AP_PARAM

#ifdef FUN_EEPROM

Parameters g;

#define GSCALAR(v, name, def) { g.v.vtype, name, Parameters::k_param_ ## v, &g.v, {def_value : def} }
#define GGROUP(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &g.v, {group_info : class::var_info} }
#define GOBJECT(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &v, {group_info : class::var_info} }

//#define GSTRING16(v, name) { AP_PARAM_STRING16, name, Parameters::k_param_ ## v, &g.v }

const AP_Param::Info var_info[] = {
	GSCALAR(format_version,		"SYSID_SW_MREV", 0),
	GSCALAR(software_type,		"SYSID_SW_TYPE", Parameters::k_software_type),



#if (defined ( GIMBAL_ENABLE_COMPASS ))
    GOBJECT(compass,            "CMP_",    Compass),
#endif

#if ((defined ( IMU_AP )) || ( defined (IMU_MIXED)))

    // @Group: INS_
    // @Path: ../libraries/AP_InertialSensor/AP_InertialSensor.cpp
#if HIL_MODE == HIL_MODE_DISABLED
    GOBJECT(ins,            "INS_", AP_InertialSensor),
#endif


    // @Group: AHRS_
    // @Path: ../libraries/AP_AHRS/AP_AHRS.cpp
    GOBJECT(ahrs,               "AHRS_",    AP_AHRS),
#endif

    GSCALAR(space_end,			"SPACE_END", 0),

    AP_VAREND
};

/*
void save_default_parameters()
{
#if ((defined ( IMU_AP )) || ( defined (IMU_MIXED)))

	ahrs.gps_gain.set_and_save(1.0);

	//ahrs._gps_use.set_and_save(1);
	ahrs._gps_use.set_and_save(0);

	ahrs._kp_yaw.set_and_save(0.1);
	//ahrs._kp_yaw.set_and_save(0.4);

	//ahrs._kp.set_and_save(0.4);
	ahrs._kp.set_and_save(0.1);

	ahrs._wind_max.set_and_save(0.0);
	ahrs._baro_use.set_and_save(0);

//	Vector3f v;
//	v.x = 0;v.y = 0;v.z = 0;
//	ahrs._trim.set_and_save(v);
#endif

#ifdef COMPASS_AP
	compass.save_offsets();
#endif
}


void format_eeprom()
{
	const char * buff = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int j, i;
	for (j = 0; j < EEPROM_PAGE_SIZE / 32; j++ )
	{
		for (i = 0; i < 32; i++)
		{
			char c = buff[i];
			EEPROM.write((uint32_t) (i + 32*j), c);
		}
	}
}
*/
void load_parameters(void)
{


//	if (!ahrs._gps_use.load()) {
//		ahrs._gps_use.set_and_save(0);
//	}
//
//	// change the default for the AHRS_GPS_GAIN for ArduCopter
//	// if it hasn't been set by the user
//	if (!ahrs.gps_gain.load()) {
//		ahrs.gps_gain.set_and_save(1.0);
//	}
//
//	// setup different AHRS gains for ArduCopter than the default
//	// but allow users to override in their config
//	if (!ahrs._kp.load()) {
//		ahrs._kp.set_and_save(0.1);
//	}
//	if (!ahrs._kp_yaw.load()) {
//		ahrs._kp_yaw.set_and_save(0.1);
//	}



	if (!g.format_version.load() ||
		g.format_version != Parameters::k_format_version) {

		g.space_end.load();
		if (g.space_end != Parameters::k_space_end)
		{
			cliSerial->print("Error reading PARAM_END!!\r\n");
		}

		// erase all parameters
		cliSerial->printf_P(PSTR("Firmware change: erasing EEPROM...\n"));

		//format_eeprom();

		//zero_eeprom();
		AP_Param::erase_all();

		// save the current format version
		g.format_version.set_and_save(Parameters::k_format_version);

		//save_default_parameters();

		g.space_end.set_and_save(Parameters::k_space_end);
//	        default_dead_zones();
		cliSerial->println_P(PSTR("done."));
	} else {
		uint32_t before = micros();
		// Load all auto-loaded EEPROM variables
		AP_Param::load_all();




		cliSerial->printf_P(PSTR("load_all took %luus\n"), micros() - before);
	}


/*
	//BruGi: Read Config, fill with default settings if versions do not match or CRC fails
	read_config();
	if ((config.vers != VERSION) || (config.versEEPROM != VERSION_EEPROM))
	{
		cliSerial->print(F("EEPROM version mismatch, initialize EEPROM"));
		setDefaultParameters();
		//config.crc8 = crcSlow((crc *)&config, sizeof(config)-1); // set proper CRC
		write_config();
	}
*/
}


void print_param(ap_var_type type, AP_Param *ap)
{
	char s[AP_MAX_NAME_SIZE+1];
	ap->copy_name(s, sizeof(s), true);
	s[AP_MAX_NAME_SIZE] = 0;

	switch (type) {
	case AP_PARAM_INT8:
		cliSerial->printf_P(PSTR("%s: %d\n"), s, (int)((AP_Int8 *)ap)->get());
		break;
	case AP_PARAM_INT16:
		cliSerial->printf_P(PSTR("%s: %d\n"), s, (int)((AP_Int16 *)ap)->get());
		break;
	case AP_PARAM_INT32:
		cliSerial->printf_P(PSTR("%s: %ld\n"), s, (long)((AP_Int32 *)ap)->get());
		break;
	case AP_PARAM_FLOAT:
		cliSerial->printf(PSTR("%s: "), s);
		cliSerial->print(((AP_Float *)ap)->get(), 3);
		cliSerial->print("\n");
		break;
	default:
		break;
	}
}

#endif //FUN_EEPROM

#endif //ENABLE_AP_PARAM

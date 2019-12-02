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
 * ManualCmd.cpp
 *
 *  Created on: 17/set/2013
 *      Author: Murtas Matteo
 */

#include "main.h"

#define STR_THEN						" then"
#define STR_PRESS_ENTER					" press ENTER"

#define STR_MIN							" MIN"
#define STR_MAX							" MAX"


typedef struct AXIS_specs{
	uint8		Pin;
	MAN_CMD_AXIS_Mode	Mode;
	uint16_t	Min;
	uint16_t	Mid;
	uint16_t	Max;
	bool		NegativeSlope;
} AXIS_specs_t;



AXIS_specs_t manCmdAxis[MANUAL_INPUT_COUNT];




void init_axis(uint8 n_axis, uint8 pin, MAN_CMD_AXIS_config cfg)
{
	manCmdAxis[n_axis].Pin = pin;
	manCmdAxis[n_axis].Mode = (MAN_CMD_AXIS_Mode) cfg.Mode;
	switch (manCmdAxis[n_axis].Mode)
	{
	case joyAnalog:
		pinMode(pin, INPUT_ANALOG);
		break;
	case joyDigital:
		pinMode(pin, INPUT_PULLUP);
		break;
	default:
		break;
	}
	manCmdAxis[n_axis].Min = cfg.Min;  //config.manCmdAxisParam[n_axis].Min;
	manCmdAxis[n_axis].Mid = cfg.Mid; // config.manCmdAxisParam[n_axis].Mid;
	manCmdAxis[n_axis].Max = cfg.Max; // config.manCmdAxisParam[n_axis].Max;

	manCmdAxis[n_axis].NegativeSlope = (manCmdAxis[n_axis].Min > manCmdAxis[n_axis].Max);

}

//da usare solo in fase di calibrazione
void reinit_axis_mode(uint8 n_axis, uint16 md)
{
	manCmdAxis[n_axis].Mode = (MAN_CMD_AXIS_Mode) md;
	switch (manCmdAxis[n_axis].Mode)
	{
	case joyAnalog:
		pinMode(manCmdAxis[n_axis].Pin , INPUT_ANALOG);
		break;
	case joyDigital:
		pinMode(manCmdAxis[n_axis].Pin , INPUT_PULLUP);
		break;
	default:
		break;
	}
}

void initManualControllers()
{

	cliSerial->print("Init axis\r\n");


	//pin map
	uint8 pins[] = {
			BOARD_ANA1,
			BOARD_ANA2,
			BOARD_ANA3,
			BOARD_ANA4,
			BOARD_ANA5,
			BOARD_ANA6
	};


	for (int i = 0; i < MANUAL_INPUT_COUNT; i++)
	{
		init_axis(i, pins[i], config.manCmdAxisParam[i]);
	}

}

/********************************/
/* Manual Control IRQ Routine    */
/********************************/

void buttonInitInterrupt()
{
}

// motor position control
uint32_t _last_button_interrupt = 0;
void buttonInterrupt()
{
}

void getManCmdAxisValue(uint8 nAxis, uint16_t* val){

	if (manCmdAxis[nAxis].Mode == joyAnalog)
		*val = analogRead(manCmdAxis[nAxis].Pin);
	else if (manCmdAxis[nAxis].Mode == joyDigital)
		*val = digitalRead(manCmdAxis[nAxis].Pin);
	else
		*val = 0;
}


void waitEnter(){

	cliSerial->printf(STR_THEN);
	cliSerial->printf(STR_PRESS_ENTER);
	while (cliSerial->read() != 0x0D){};
	return;
}

void ManCmdAxisCalibration(){

	uint8		nAxis;

	bool bAbort = false;

	for (nAxis=0; nAxis<MANUAL_INPUT_COUNT; nAxis++){

		MAN_CMD_AXIS_config_t tmpcfg;
		tmpcfg.Mode = config.manCmdAxisParam[nAxis].Mode;
		tmpcfg.Min = config.manCmdAxisParam[nAxis].Min;
		tmpcfg.Mid = config.manCmdAxisParam[nAxis].Mid;
		tmpcfg.Max = config.manCmdAxisParam[nAxis].Max;

		cliSerial->printf("\r\nAxis %u Current mode: %d\r\n", nAxis, tmpcfg.Mode);
		cliSerial->printf("Change Mode (0 - Disable, 1 - Joystick, 2 - Button)?\r\n");
		cliSerial->flush();
		while (1)
		{
			if(cliSerial->available()){
				char ch[2];
				ch[0] = cliSerial->read();
				ch[1] = 0;
				if (ch[0] == 0x0A)
					break;
				else if (ch[0] == 0x1B)
				{
					bAbort = true;
					break;
				} else {
					//config.manCmdAxisParam[nAxis].Mode = atoi(ch);
					tmpcfg.Mode = atoi(ch);
					reinit_axis_mode(nAxis, tmpcfg.Mode);
					cliSerial->printf("\r\n- Mode: %d\r\n", tmpcfg.Mode);
				}
			}
		}

		if (!bAbort)
		{
			cliSerial->printf("\r\nAxis %u - MID point calibration:\r\n", nAxis);
			cliSerial->printf("Leave Axis in center. Press ENTER when done.\r\n\r\n");

			while (1)
			{
				delay(100);
				uint16_t v = 0;
				getManCmdAxisValue(nAxis, &v);
				cliSerial->printf("# MidV= %u      \r", v);

				if(cliSerial->available()){
					uint8 ch = cliSerial->read();
					if (ch == 0x0A)
					{
						//config.manCmdAxisParam[nAxis].Mid = v;
						tmpcfg.Mid = v;
						cliSerial->printf("\r\n- MID: %d\r\n", tmpcfg.Mid);
						break;
					}
					else if (ch == 0x1B)
					{
						bAbort = true;
						break;
					}
				}
			}
		}

		if (!bAbort)
		{
			cliSerial->printf("\r\nAxis %u - MIN point calibration:\r\n", nAxis);
			cliSerial->printf("Keep Axis to MIN position. Press ENTER when done.\r\n\r\n");

			while (1)
			{
				delay(100);
				uint16_t v = 0;
				getManCmdAxisValue(nAxis, &v);
				cliSerial->printf("# MinV= %u      \r", v);


				if(cliSerial->available()){
					uint8 ch = cliSerial->read();
					if (ch == 0x0A)
					{
						//config.manCmdAxisParam[nAxis].Min = v;
						tmpcfg.Min = v;
						cliSerial->printf("\r\n- MIN: %d\r\n", tmpcfg.Min);
						break;
					}
					else if (ch == 0x1B)
					{
						bAbort = true;
						break;
					}
				}
			}
		}

		if (!bAbort)
		{
			cliSerial->printf("\r\nAxis %u - MAX point calibration:\r\n", nAxis);
			cliSerial->printf("Keep Axis to MAX position. Press ENTER when done.\r\n\r\n");

			while (1)
			{
				delay(100);
				uint16_t v = 0;
				getManCmdAxisValue(nAxis, &v);
				cliSerial->printf("# MaxV= %u      \r", v);


				if(cliSerial->available()){
					uint8 ch = cliSerial->read();
					if (ch == 0x0A)
					{
						//config.manCmdAxisParam[nAxis].Max = v;
						tmpcfg.Max = v;
						cliSerial->printf("\r\n- MAX: %d\r\n", tmpcfg.Max);
						break;
					}
					else if (ch == 0x1B)
					{
						bAbort = true;
						break;
					}
				}
			}
		}



		if (!bAbort)
		{
			config.manCmdAxisParam[nAxis].Mode = tmpcfg.Mode;
			config.manCmdAxisParam[nAxis].Min = tmpcfg.Min;
			config.manCmdAxisParam[nAxis].Mid = tmpcfg.Mid;
			config.manCmdAxisParam[nAxis].Max = tmpcfg.Max;
		}
		init_axis(nAxis, manCmdAxis[nAxis].Pin, config.manCmdAxisParam[nAxis]);

		if (bAbort)
		{
			break;
		}
	}
	if (bAbort)
		cliSerial->printf("\r\nCalibration Aborted.\r\n");
	else
		cliSerial->printf("\r\nCalibration DONE! Remember to save to Flash (WE).\r\n");
	return;

}

uint16_t map(uint16_t value, uint16_t fromStart, uint16_t fromEnd,
		uint16_t toStart, uint16_t toEnd) {

	if (fromEnd == fromStart)
		return toStart; // (toStart + toEnd) / 2;
	else
	    return (uint16_t) ((float) (value - fromStart) * (float) (toEnd - toStart) /  (float) (fromEnd - fromStart) + toStart);

    //return (uint16_t) map((long) value, (long) fromStart, (long)fromEnd, (long)  toStart, (long) toEnd  );
}

uint16_t getManCmdAxisRC(uint8 nAxis)
{
	uint16_t ax;
	getManCmdAxisValue(nAxis, &ax);


	uint16_t vRC = 1500;
	if (manCmdAxis[nAxis].NegativeSlope)
	{
		if (ax >  manCmdAxis[nAxis].Mid)
			vRC = map(ax, manCmdAxis[nAxis].Min, manCmdAxis[nAxis].Mid, (uint16_t) 1000, (uint16_t)1500);
		else if (ax ==  manCmdAxis[nAxis].Mid)
		{
			if ( manCmdAxis[nAxis].Min ==  manCmdAxis[nAxis].Mid)
				vRC = 1000;
			else if ( manCmdAxis[nAxis].Max ==  manCmdAxis[nAxis].Mid)
				vRC = 2000;
			else
				vRC = 1500;
		}
		else
			vRC = map(ax, manCmdAxis[nAxis].Mid, manCmdAxis[nAxis].Max, (uint16_t)1500, (uint16_t)2000);

	} else {
		if (ax <  manCmdAxis[nAxis].Mid)
			vRC = map(ax, manCmdAxis[nAxis].Min, manCmdAxis[nAxis].Mid, (uint16_t)1000, (uint16_t)1500);
		else if (ax ==  manCmdAxis[nAxis].Mid)
		{
			if ( manCmdAxis[nAxis].Min ==  manCmdAxis[nAxis].Mid)
				vRC = 1000;
			else if ( manCmdAxis[nAxis].Max ==  manCmdAxis[nAxis].Mid)
				vRC = 2000;
			else
				vRC = 1500;
		}
		else
			vRC = map(ax, manCmdAxis[nAxis].Mid, manCmdAxis[nAxis].Max,(uint16_t)1500, (uint16_t)2000);
	}


	return (uint16_t) vRC;

}



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

/*************************/
/* RC-Decoder            */
/*************************/

#include "main.h"

#ifdef GIMBAL_ENABLE_RC

// init RC config variables
void initRC() {
	for (unsigned char id = 0; id < 3; id++)
	{
		rcLPF_tc[id] =  LOWPASS_K_FLOAT(config.profiles[0].rcConfig[id].LPF*0.1);
	}
}


#ifndef RC_USE_LIBS
// pinChange Int driven Functions

void intDecodePWM_Ch0_rising();
void intDecodePWM_Ch0_falling();
void intDecodePWM_Ch1_rising();
void intDecodePWM_Ch1_falling();
void intDecodePWM_Ch2_rising();
void intDecodePWM_Ch2_falling();
void intDecodePWM_Ch3_rising();
void intDecodePWM_Ch3_falling();
void intDecodePWM(char chIdx, uint8 pinState);

void intDecodePWM_Ch0_rising()
{
	intDecodePWM(0, HIGH);
	detachInterrupt(BOARD_PWM_IN0);
	attachInterrupt(BOARD_PWM_IN0, &intDecodePWM_Ch0_falling, FALLING);
}
void intDecodePWM_Ch0_falling()
{
	intDecodePWM(0, LOW);
	detachInterrupt(BOARD_PWM_IN0);
	attachInterrupt(BOARD_PWM_IN0, &intDecodePWM_Ch0_rising, RISING);
}

void intDecodePWM_Ch1_rising()
{
	intDecodePWM(1, HIGH);
	detachInterrupt(BOARD_PWM_IN1);
	attachInterrupt(BOARD_PWM_IN1, &intDecodePWM_Ch1_falling, FALLING);
}
void intDecodePWM_Ch1_falling()
{
	intDecodePWM(1, LOW);
	detachInterrupt(BOARD_PWM_IN1);
	attachInterrupt(BOARD_PWM_IN1, &intDecodePWM_Ch1_rising, RISING);
}

void intDecodePWM_Ch2_rising()
{
	intDecodePWM(2, HIGH);
	detachInterrupt(BOARD_PWM_IN2);
	attachInterrupt(BOARD_PWM_IN2, &intDecodePWM_Ch2_falling, FALLING);
}
void intDecodePWM_Ch2_falling()
{
	intDecodePWM(2, LOW);
	detachInterrupt(BOARD_PWM_IN2);
	attachInterrupt(BOARD_PWM_IN2, &intDecodePWM_Ch2_rising, RISING);
}

void intDecodePWM_Ch3_rising()
{
	intDecodePWM(3, HIGH);
	detachInterrupt(BOARD_PWM_IN3);
	attachInterrupt(BOARD_PWM_IN3, &intDecodePWM_Ch3_falling, FALLING);
}
void intDecodePWM_Ch3_falling()
{
	intDecodePWM(3, LOW);
	detachInterrupt(BOARD_PWM_IN3);
	attachInterrupt(BOARD_PWM_IN3, &intDecodePWM_Ch3_rising, RISING);
}

// RC Channel 0
void intDecodePWM(char chIdx, uint8 pinState)
{ 
  uint32_t microsNow = micros();
  uint16_t pulseInPWMtmp;


	rcData_t* data = 0; 
	if (chIdx == config.rcChannelRoll)
		data = &rcData[RC_DATA_ROLL];
	else if (chIdx == config.rcChannelPitch)
		data = &rcData[RC_DATA_PITCH];
	else if (chIdx == config.rcChannelYaw)
		data = &rcData[RC_DATA_YAW];
	else if (chIdx == config.rcChannelResetRoll)
		data = &rcData[RC_DATA_RESET_ROLL];
	else if (chIdx == config.rcChannelResetPitch)
		data = &rcData[RC_DATA_RESET_PITCH];
	else if (chIdx == config.rcChannelResetYaw)
		data = &rcData[RC_DATA_RESET_YAW];
	if (data)
	{
		if (pinState==HIGH)
		{
			rcData->microsRisingEdge = microsNow;
		}
		else
		{
			rcData->microsLastUpdate = microsNow;
			pulseInPWMtmp = (microsNow - rcData->microsRisingEdge)/CC_FACTOR;
			if ((pulseInPWMtmp >= MIN_RC) && (pulseInPWMtmp <= MAX_RC)) 
			{
			  // update if within expected RC range
			  rcData->rx = pulseInPWMtmp;
			  rcData->valid=true;
			  rcData->update=true;
			}
		}
	}
}



//******************************************
// PPM Decoder
//******************************************
void intDecodePPM()
{ 
  int32_t microsNow = micros();
  
  static int32_t microsPPMLastEdge = 0;
  uint16_t pulseInPPM;

  static char channel_idx = 0;

  pulseInPPM = (microsNow - microsPPMLastEdge)/CC_FACTOR;
  microsPPMLastEdge = microsNow;

  if (pulseInPPM > RC_PPM_GUARD_TIME) 
  {
    // sync detected 
    channel_idx = 0;
  }
  else if (channel_idx < RC_PPM_RX_MAX_CHANNELS)
  {      
	rcData_t* data = 0; 
	if (channel_idx == config.rcChannelRoll)
		data = &rcData[RC_DATA_ROLL];
	else if (channel_idx == config.rcChannelPitch) 
		data = &rcData[RC_DATA_PITCH];
	else if (channel_idx == config.rcChannelYaw)
		data = &rcData[RC_DATA_YAW];
	else if (channel_idx == config.rcChannelResetRoll)
		data = &rcData[RC_DATA_RESET_ROLL];
	else if (channel_idx == config.rcChannelResetPitch)
		data = &rcData[RC_DATA_RESET_PITCH];
	else if (channel_idx == config.rcChannelResetYaw)
		data = &rcData[RC_DATA_RESET_YAW];
	if (data)
	{  
      data->microsLastUpdate = microsNow;    
      if ((pulseInPPM >= MIN_RC) && (pulseInPPM <= MAX_RC)) 
      {
        data->rx     = pulseInPPM;
        data->valid  = true;
        data->update = true;
      }  
    }
    channel_idx++;  
  }
}
#endif

//******************************************
// PPM & PWM Decoder
//******************************************

// check for RC timout
void checkRcTimeouts()
{
#ifdef RC_USE_LIBS
	return;
#else
  int32_t microsNow = micros();
  int32_t microsLastUpdate;
  for (char id = 0; id < RC_DATA_SIZE; id++)
  {
    cli();
    microsLastUpdate = rcData[id].microsLastUpdate;
    sei();
    if (rcData[id].valid && ((microsNow - microsLastUpdate)/CC_FACTOR) > RC_TIMEOUT) 
    {
      rcData[id].rx     = config.rcMid;
      rcData[id].valid  = false;
      rcData[id].update = true;
    }
  }
#endif
}


int getRCDataChannel(int index)
{
	switch (index)
	{
	case RC_DATA_ROLL:
		return config.profiles[0].rcConfig[axisROLL].channel;
		break;
	case RC_DATA_PITCH:
		return config.profiles[0].rcConfig[axisPITCH].channel;
		break;
	case RC_DATA_YAW:
		return config.profiles[0].rcConfig[axisYAW].channel;
		break;
	case RC_DATA_RESET_ROLL:
		return config.profiles[0].rcConfig[axisROLL].resetChannel;
		break;
	case RC_DATA_RESET_PITCH:
		return config.profiles[0].rcConfig[axisPITCH].resetChannel;
		break;
	case RC_DATA_RESET_YAW:
		return config.profiles[0].rcConfig[axisYAW].resetChannel;
		break;
	}
	return -1;
}


uint32_t last_joystick[RC_DATA_SIZE] = {0};

void updateRCsignals()
{
#ifdef RC_USE_LIBS
	if (APM_RC.GetState() > 0){
//		for (unsigned char channel_idx = 0; channel_idx < RC_PPM_RX_MAX_CHANNELS; channel_idx++)
//		{
//
//			rcData_t* data = 0;
//			if (channel_idx == config.rcChannelRoll)
//				data = &rcData[RC_DATA_ROLL];
//			else if (channel_idx == config.rcChannelPitch)
//				data = &rcData[RC_DATA_PITCH];
//			else if (channel_idx == config.rcChannelYaw)
//				data = &rcData[RC_DATA_YAW];
//			else if (channel_idx == config.rcChannelResetRoll)
//				data = &rcData[RC_DATA_RESET_ROLL];
//			else if (channel_idx == config.rcChannelResetPitch)
//				data = &rcData[RC_DATA_RESET_PITCH];
//			else if (channel_idx == config.rcChannelResetYaw)
//				data = &rcData[RC_DATA_RESET_YAW];
//			if (data)
//			{
//				data->rx = APM_RC.InputCh(channel_idx);
//				data->valid = true;
//				data->update = true;
//			}
//		}

		//anzichè scorrere tutti i canali della radio, vado a leggere direttamente quelli configurati
		//in questo modo posso anche usare lo stesso canale per due funzioni contemporaneamente
		//(utile per il resetAngle)
		for (int ch = 0; ch < RC_DATA_SIZE; ch++)
		{
			int channel = getRCDataChannel(ch);

			if ((channel >= 0) && (channel < RC_PPM_RX_MAX_CHANNELS))
			{
				rcData[ch].rx = APM_RC.InputCh(channel);
				rcData[ch].valid = true;
				rcData[ch].update = true;
			}
		}


	}
#endif

	bool bJoystickUpdated = false; //eseguo solo una lettura per ciclo per non rallentare

	uint32_t now = millis();


	for (int ch = 0; ch < RC_DATA_SIZE; ch++)
	{
		int channel = getRCDataChannel(ch);

		if (channel >= 100)
		{
			if ((!bJoystickUpdated) && ( now - last_joystick[ch] > MANUAL_INPUT_UPDATE_INTERVAL))
			{
				last_joystick[ch] = now;
				bJoystickUpdated = true;
				uint8 axis = channel - 100;
				rcData[ch].rx = getManCmdAxisRC(axis);
				rcData[ch].valid = true;
				rcData[ch].update = true;
			}
		}
	}

//
//	//qui provo con i canali joystick
//	if (config.rcChannelRoll >= 100)
//	{
//		if ((!bJoystickUpdated) && ( now - last_joystick_roll > MANUAL_INPUT_UPDATE_INTERVAL))
//		{
//			last_joystick_roll = now;
//			bJoystickUpdated = true;
//			uint8 axis = config.rcChannelRoll - 100;
//			rcData[RC_DATA_ROLL].rx = getManCmdAxisRC(axis);
//			rcData[RC_DATA_ROLL].valid = true;
//			rcData[RC_DATA_ROLL].update = true;
//		}
//	}
//
//	if (config.rcChannelPitch >= 100)
//	{
//		if ((!bJoystickUpdated) && ( now - last_joystick_pitch > MANUAL_INPUT_UPDATE_INTERVAL))
//		{
//			last_joystick_pitch = now;
//			bJoystickUpdated = true;
//
//			uint8 axis = config.rcChannelPitch - 100;
//			rcData[RC_DATA_PITCH].rx = getManCmdAxisRC(axis);
//			rcData[RC_DATA_PITCH].valid = true;
//			rcData[RC_DATA_PITCH].update = true;
//		}
//	}
//	if (config.rcChannelYaw >= 100)
//	{
//		if ((!bJoystickUpdated) && ( now - last_joystick_yaw > MANUAL_INPUT_UPDATE_INTERVAL))
//		{
//			last_joystick_yaw = now;
//			bJoystickUpdated = true;
//
//			uint8 axis = config.rcChannelYaw - 100;
//			rcData[RC_DATA_YAW].rx = getManCmdAxisRC(axis);
//			rcData[RC_DATA_YAW].valid = true;
//			rcData[RC_DATA_YAW].update = true;
//		}
//	}

}


// initialize RC Pin mode
void initRCPins()
{  

#ifdef RC_USE_LIBS
	if (config.profiles[0].rcModePPM)
	{
		APM_RC.Init(12, NULL, cliSerial);
	} else {
		APM_RC.Init(2, NULL, cliSerial);
	}
#else
  if (config.rcModePPM)
  {
    pinMode(BOARD_PWM_IN0, INPUT); digitalWrite(BOARD_PWM_IN0, HIGH);
    attachInterrupt(BOARD_PWM_IN0, &intDecodePPM, RISING);
  }
  else
  {
    pinMode(BOARD_PWM_IN0, INPUT);
    digitalWrite(BOARD_PWM_IN0, HIGH);
    attachInterrupt(BOARD_PWM_IN0, &intDecodePWM_Ch0_rising, RISING);

    pinMode(BOARD_PWM_IN1, INPUT);
    digitalWrite(BOARD_PWM_IN1, HIGH);
    attachInterrupt(BOARD_PWM_IN1, &intDecodePWM_Ch1_rising, RISING);

    pinMode(BOARD_PWM_IN2, INPUT);
    digitalWrite(BOARD_PWM_IN2, HIGH);
    attachInterrupt(BOARD_PWM_IN2, &intDecodePWM_Ch2_rising, RISING);

    pinMode(BOARD_PWM_IN3, INPUT);
    digitalWrite(BOARD_PWM_IN3, HIGH);
    attachInterrupt(BOARD_PWM_IN3, &intDecodePWM_Ch3_rising, RISING);
  }
#endif

  for (unsigned char id = 0; id < RC_DATA_SIZE; id++)
  {
    cli();
    rcData[id].microsRisingEdge = 0;
    rcData[id].microsLastUpdate = 0;
    rcData[id].rx               = 1500;
    rcData[id].update           = true;
    rcData[id].valid            = true;
    rcData[id].rcSpeed          = 0.0;
    rcData[id].setpoint         = 0.0;
    sei();
  }
}

//******************************************
// Proportional
//******************************************

void evalRCChannelProportional(rcData_t* rcData, int16_t rcGain, int16_t rcMid)
{
  if(rcData->update == true)
  {
    if(rcData->rx >= rcMid + RC_DEADBAND)
    {
      rcData->rcSpeed = rcGain * (float)(rcData->rx - (rcMid + RC_DEADBAND))/ (float)(MAX_RC - (rcMid + RC_DEADBAND)) + 0.9 * rcData->rcSpeed;
    }
    else if(rcData->rx <= rcMid-RC_DEADBAND)
    {
      rcData->rcSpeed = -rcGain * (float)((rcMid - RC_DEADBAND) - rcData->rx)/ (float)((rcMid - RC_DEADBAND)-MIN_RC) + 0.9 * rcData->rcSpeed;
    }
    else
    {
      rcData->rcSpeed = 0.0;
    }
    rcData->rcSpeed = constrain(rcData->rcSpeed, -200.0f, +200.0f);  // constrain for max speed
    rcData->update = false;
  }
}




//******************************************
// Absolute
//******************************************

inline void evalRCChannelAbsolute(rcData_t* rcData, int16_t rcMin, int16_t rcMax, int16_t rcMid)
{
  float k;
  float y0;
  int16_t rx;
  
  if(rcData->update == true)
  {
    k = (float)(rcMax - rcMin)/(float)(MAX_RC - MIN_RC);
    y0 = rcMin + k * (float)(MID_RC - MIN_RC);
    rx = rcData->rx - rcMid;
    utilLP_float(&rcData->setpoint, y0 + k * (float) rx, 0.05f);
    rcData->update = false;
  }
}

// RC control

void evaluateRC()
{

	updateRCsignals();

	for (unsigned char id = 0; id < 3; id++)
	{
		if (config.profiles[0].rcConfig[id].absolute)
			evalRCChannelAbsolute(&rcData[id], config.profiles[0].rcConfig[id].minOutput, config.profiles[0].rcConfig[id].maxOutput, config.profiles[0].rcMid);
		else
			evalRCChannelProportional(&rcData[id], config.profiles[0].rcConfig[id].gain, config.profiles[0].rcMid);
	}

	evalRCChannelAbsolute(&rcData[RC_DATA_RESET_ROLL], 0, 1, config.profiles[0].rcMid + RC_DEADBAND);
	evalRCChannelAbsolute(&rcData[RC_DATA_RESET_PITCH], 0, 1, config.profiles[0].rcMid + RC_DEADBAND);
	evalRCChannelAbsolute(&rcData[RC_DATA_RESET_YAW], 0, 1, config.profiles[0].rcMid + RC_DEADBAND);


}


#endif

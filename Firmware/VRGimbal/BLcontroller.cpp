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

#include "main.h"

/*
https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328

// CS BITS
CS02	CS01    CS00 	 DESCRIPTION
0	0 	0 	 Timer/Counter0 Disabled 
0	0 	1 	 No Prescaling
0	1 	0 	 Clock / 8
0	1 	1 	 Clock / 64
1	0 	0 	 Clock / 256
1	0 	1 	 Clock / 1024

CS12	 CS11 	 CS10 	 DESCRIPTION
0	0 	0 	 Timer/Counter1 Disabled 
0	0 	1 	 No Prescaling
0	1 	0 	 Clock / 8
0	1 	1 	 Clock / 64
1	0 	0 	 Clock / 256
1	0 	1 	 Clock / 1024

CS22	 CS21 	 CS20 	 DESCRIPTION
0	0 	0 	 Timer/Counter2 Disabled 
0	0 	1 	 No Prescaling
0	1 	0 	 Clock / 8
0	1 	1 	 Clock / 32
1	0 	0 	 Clock / 64
1	0 	1 	 Clock / 128
1	1 	0 	 Clock / 256
1	1 	1 	 Clock / 1024 


// WAVEFORM GENERATOR BITS
	WGM02	WGM01	WGM00	 DESCRIPTION	 	TOP
0	0 	0	0	 Normal 	 	0xFF
1	0	0	1	 PWM, Phase Corrected	0xFF
2	0	1	0	 CTC			OCR0A
3	0	1	1	 Fast PWM		0xFF
4	1	0	0	 Reserved	 	-
5	1	0	1	 Fast PWM, Phase Corr	OCR0A
6	1	1	0	 Reserved		-
7	1	1	1	 Fast PWM		OCR0A

MODE	WGM13	WGM12	WGM11	WGM10	 DESCRIPTION            	 TOP
0	 0	 0 	0	0	 Normal 	                 0xFFFF
1	0	0	0	1	 PWM, Phase Corrected, 8bit	 0x00FF
2	0	0	1	0	 PWM, Phase Corrected, 9bit	 0x01FF
3	0	0	1	1	 PWM, Phase Corrected, 10bit 	 0x03FF 
4	0	1	0	0        CTC	                         OCR1A 
5	0	1	0	1	 Fast PWM, 8bit 	          0x00FF 
6	0	1	1	0	 Fast PWM, 9bit 	          0x01FF 
7	0	1	1	1	 Fast PWM, 10bit 	          0x03FF 
8	1	0	0	0	 PWM, Phase and Frequency Corr    ICR1 
9	1	0	0	1	 PWM, Phase and Frequency Corr    OCR1A 
10	1	0	1	0	 PWM, Phase Correct 	          ICR1 
11	1	0	1	1	 PWM, Phase Correct 	         OCR1A
12	1	1	0	0	 CTC	                         ICR1
13	1	1	0	1	 RESERVED	 
14	1	1	1	0	 Fast PWM 	                  ICR1 
15	1	1	1	1	 Fast PWM	                  OCR1A 

MODE	WGM21	WGM20	 DESCRIPTION	          TOP
0	0	0	 Normal 	         0xFF
1	0	1	 PWM Phase Corrected	 
2	1	0	 CTC	                  OCR2
3	1	1	 Fast PWM 	 



x = Timer Number
 	7 bit	 6 bit 	 5 bit 	 4 bit 	 3 bit 	 2 bit 	 1 bit 	 0 bit     Description
TCCRxA	COMxA1	 COMxA0  COMxB1  COMxB0  -	 -	 WGMx1	 WGMx0     Timer/Counter Control Register x A (x=0,2)

TCCR1B	ICNC1	 ICES1	 -	 WGM13	 WGM12	 CS12	 CS11	CS10 
TCCRxB	FOCxA    FOCxB   -       -       WGMx2   CSx2    CSx1    CSx0      Timer/Counter Control Register x B

TIMSKx	-        -       -       -       -       OCIExB  OCIExA  TOIEx     Timer/Counter Interrupt Mask Register
TIFRx	-	 -	 -	 -       -       OCFxB	 OCFxA   TOVx      Timer/Counter Interrupt Flag Register
TCNTx                                                                      Timer/Counter Register (stores the counter value)
OCRxA                                                                      Output Compare Register x A
OCRxB                                                                      Output Compare Register x B


*/

#define BL_PHASE_CONFIG 1

#if (BL_PHASE_CONFIG == 1)
#define BL_PHASE_A  0
#define BL_PHASE_B  85
#define BL_PHASE_C  170

#elif (BL_PHASE_CONFIG == 2)
#define BL_PHASE_A  0
#define BL_PHASE_B  170
#define BL_PHASE_C  85

#elif (BL_PHASE_CONFIG == 3)
#define BL_PHASE_A  85
#define BL_PHASE_B  0
#define BL_PHASE_C  170

#elif (BL_PHASE_CONFIG == 4)
#define BL_PHASE_A  85
#define BL_PHASE_B  170
#define BL_PHASE_C  0

#elif (BL_PHASE_CONFIG == 5)
#define BL_PHASE_A  170
#define BL_PHASE_B  0
#define BL_PHASE_C  85

#elif (BL_PHASE_CONFIG == 6)
#define BL_PHASE_A  170
#define BL_PHASE_B  85
#define BL_PHASE_C  0

#endif


void motorInterrupt();
void motorInitInterrupt();



#ifdef PWM_USE_OFFSET
#define PWM_OFFSET_FACTOR 0.5   //percentuale di pwmResolution su cui centrare le variazioni del duty cicle
//uint8_t  g_pwmOffsetFactor = 50; //da 0 a 100
//bool 	 g_pwmSinUseOffset = false;
pwmsin_t g_pwmOffset = 0;
#else
#define MIN_PWM_VAL 1  //per evitare che il pwm vada a zero
#endif

//float g_pwmSinMotor[N_SIN];
int32_t g_pwmSinMotor[N_SIN];

#define MOTOR_COUNT 3
#define MOTOR_PIN_COUNT 3

uint8 MOTOR_PINS[][MOTOR_PIN_COUNT] = {
		{ BOARD_MOT1_CMD_A, BOARD_MOT1_CMD_B, BOARD_MOT1_CMD_C},
		{ BOARD_MOT2_CMD_A, BOARD_MOT2_CMD_B, BOARD_MOT2_CMD_C},
		{ BOARD_MOT3_CMD_A, BOARD_MOT3_CMD_B, BOARD_MOT3_CMD_C}
};

enum MOTOR_PHASE_PINS
{
	pinA = 0,
	pinB = 1,
	pinC = 2
};

//int MOTOR_PHASES[] = { BL_PHASE_A, BL_PHASE_B, BL_PHASE_C };

int MOTOR_PHASES[] = { BL_PHASE_A * N_SIN / 256, BL_PHASE_B * N_SIN / 256, BL_PHASE_C  * N_SIN / 256 };


//compatibilità
uint16 motors_pwm_period = 0;	//periodo (ovvero il COUNT) del PWM, quando duty è pari a questo valore, l'uscita è sempre alta

void pinSetPwm(uint8 pin, uint16 freq, uint16 dutyPerMille)
{
	uint16 period = pwmSetFrequency(pin, freq);
	uint16 duty2 = period * dutyPerMille / 1000;
	pwmWrite(pin, duty2);
}
//
//void pinSetDuty(uint8 pin, uint16 dutyPerMille)
//{
//	uint16 duty2 = motors_pwm_period * dutyPerMille / 1000;
//	pwmWrite(pin, duty2);
//}


void init_motors(void)
{
	cliSerial->print("Init MOTORs\r\n");
	pinMode(BOARD_MOTOR_EN, OUTPUT);
	cliSerial->print("Disable MOTOR output\r\n");
	digitalWrite(BOARD_MOTOR_EN, LOW);

	cliSerial->print("Init MOTOR SENSE\r\n");
	pinMode(BOARD_MOT1_ISENSE, INPUT_ANALOG);
	pinMode(BOARD_MOT2_ISENSE, INPUT_ANALOG);
	pinMode(BOARD_MOT3_ISENSE, INPUT_ANALOG);

	cliSerial->print("Init PWM for MOTORs\r\n");


//#ifdef PWM_32KHZ_PHASE
//	freq = 32000;
//#endif
//
//#ifdef PWM_16KHZ_PHASE
//	freq = 16000;
//#endif
//
//
//#ifdef PWM_8KHZ_PHASE
//	freq = 8000;
//#endif
//
//
//#ifdef PWM_4KHZ_PHASE
//	freq = 4000;
//#endif


	uint16 duty = 0;
	int m = 0;
	int p = 0;

	for (m = 0; m < MOTOR_COUNT; m++)
		for (p = 0; p < MOTOR_PIN_COUNT; p++)
			pinMode(MOTOR_PINS[m][p], PWM);
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	delay(100);


	resetMotorFreq();
//	uint16 period = 0;
//	for (m = 0; m < MOTOR_COUNT; m++)
//		for (p = 0; p < MOTOR_PIN_COUNT; p++)
//			period = pwmSetFrequency(MOTOR_PINS[m][p], freq);
//	motors_pwm_period = period;
//
//	recalcMotorStuff();

	for (m = 0; m < MOTOR_COUNT; m++)
		for (p = 0; p < MOTOR_PIN_COUNT; p++)
			pwmWrite(MOTOR_PINS[m][p], duty);





	cliSerial->print("Enable MOTORs\r\n");

	//cliSerial->printf("PWM period %d\r\nPWM max    %d\r\nPWM sin_x  %d\r\n", motors_pwm_period, motors_pwm_max , motors_sin_count);

	digitalWrite(BOARD_MOTOR_EN, HIGH);
	/*
	delay(10);
	uint8 v = digitalRead(BOARD_MOT1_ISENSE);
	if (v != 0)
	{
		cliSerial->print("Detected motor 1\r\n");

	}
	//motorMove(0, 0);
	v = digitalRead(BOARD_MOT2_ISENSE);
	 if (v != 0)
	 {
		cliSerial->print("Detected motor 2\r\n");

	 }
	 //motorMove(1, 0);
	 v = digitalRead(BOARD_MOT3_ISENSE);
	 if (v != 0)
	 {
		cliSerial->print("Detected motor 3\r\n");

	 }
	 motorMove(2, 0);
*/
}

void initBlController()
{
	init_motors();
	sei();

	motorInitInterrupt();
}

void calcSinusArray(pwmsin_t pwmResolution)
{
	//cliSerial->printf("pwmResolution %d ", pwmResolution);
#ifdef PWM_USE_OFFSET
	g_pwmOffset = pwmResolution * PWM_OFFSET_FACTOR;
#endif

	float funval = 0;

	for(int i=0; i<N_SIN; i++)
	{
		float x = 2.0 * PI * (float)i / (float) N_SIN;

#ifdef PWM_USE_OFFSET
		g_pwmSinMotor[i] = sin(x) * (float) pwmResolution / 2.0;
#else

		if (config.profiles[0].pwmFormula == 1)
		{
			//TEST: formula triangolare
			float gradino = (float) pwmResolution / (float) (N_SIN / 2);
			if (i == 0)
				funval = (float) pwmResolution / 2.0;
			else if (i < N_SIN / 4)
				funval = funval + gradino;
			else if (i < 3 * N_SIN / 4)
				funval = funval - gradino;
			else
				funval = funval + gradino;

			g_pwmSinMotor[i] = (int32_t) funval;
		} else {
			g_pwmSinMotor[i] = (1.0 + sin(x)) * (float) pwmResolution / 2.0;

			//TEO 20131126: modifico la formula per evitare di andare a zero
			//g_pwmSinMotor[i] = (1.0 + sin(x)) * (float) (pwmResolution - MIN_PWM_VAL) / 2.0;
		}

#endif
//		cliSerial->printf("SIN %d ", i);cliSerial->print(x);cliSerial->print(" ");cliSerial->print(g_pwmSinMotor[i]);
//		cliSerial->println();
	}
}

/*
pwmsin_t getSinusValue(uint8_t maxPWM, pwmsin_t pwmResolution, int step)
{
	step &= (N_SIN - 1);
	return (pwmResolution / 2) + sin(2.0 * step / N_SIN * 3.14159265) * (pwmResolution * maxPWM / 255.0) / 2.0;
}*/
/*
inline void setPwmSin(int pin, int pos, int pwmMax)
{
	//pos &= (N_SIN - 1);
	pos = pos % N_SIN;
	if (pos < 0)
		pos = pos + N_SIN;

	const int PWM_SCALE = 10000;
	int pwmCenter = config.profiles[0].pwmCenter;
	int pwmScale = config.profiles[0].pwmMax - config.profiles[0].pwmMin;

	int pwmExt = PWM_SCALE * pwmMax / 255;
	if (pwmExt > pwmScale)
	{
		pwmCenter = (config.profiles[0].pwmMax + config.profiles[0].pwmMin) / 2;
		pwmExt = pwmScale;
	} else if (pwmCenter + pwmExt/2 > config.profiles[0].pwmMax)
	{
		pwmCenter = config.profiles[0].pwmMax - pwmExt/2;
	} else if (pwmCenter - pwmExt/2 < config.profiles[0].pwmMin)
	{
		pwmCenter = config.profiles[0].pwmMin + pwmExt/2;
	}



//#ifdef PWM_USE_OFFSET
//	pwmsin_t pwmSin = (pwmsin_t) (g_pwmOffset + ((g_pwmSinMotor[pos] * pwmMax) / 255  ));
//#else
//	pwmsin_t pwmSin = MIN_PWM_VAL + (pwmsin_t) ((g_pwmSinMotor[pos] * (float)pwmMax) / 255 );
//#endif

	pwmsin_t pwmSin = ((pwmCenter - pwmExt/2) + pwmExt * (pwmsin_t) ((g_pwmSinMotor[pos] * (float)pwmMax) / 255 )) / PWM_SCALE;

//	if (g_bTest[8]) {
//		cliSerial->printf("pwmpin %d pos %d pwm %d", pin, pos, pwmSin);cliSerial->println();
//	}
	if (g_bTest[9])
		pwmWrite(pin, motors_pwm_period - pwmSin);
	else
		pwmWrite(pin, pwmSin);
}
*/


inline void setPwmSin(int pin, int pos, int pwmMax)
{
	//pos &= (N_SIN - 1);
	pos = pos % N_SIN;
	if (pos < 0)
		pos = pos + N_SIN;



	pwmsin_t escursione = (pwmsin_t) ((float)( motors_pwm_period * pwmMax) / 255.0f);

	pwmsin_t pwmSin =  (pwmsin_t) ((float)(g_pwmSinMotor[pos] * pwmMax) / 255.0f );

	if (config.profiles[0].pwmMode == 0)
	{
		//vicino a zero
	}
	else if (config.profiles[0].pwmMode == 1)
	{
		//center
		pwmSin = pwmSin + (motors_pwm_period - escursione)/2 ;
	}
	else if (config.profiles[0].pwmMode == 2)
	{
		//reverse
		pwmSin = motors_pwm_period - pwmSin;
	}

	pwmWrite(pin, pwmSin);
}


// 3 lsb of MotorPos still reserved for precision improvement (TBD) 
inline void MoveMotorPosSpeed(uint8_t motorNumber, int MotorPos, int motorPwmMax)
{
	if (motorNumber < MOTOR_COUNT)
	{
		int posStep = MotorPos >> STEP_DOWNSCALE;
		//posStep &= 0xff;

		for (int p = 0; p < MOTOR_PIN_COUNT; p++)
			setPwmSin(MOTOR_PINS[motorNumber][p], posStep + MOTOR_PHASES[p], motorPwmMax);
	}
}


void fastMoveMotor(uint8_t motorNumber, int dirStep, int motorPwmMax)
{


	if (motorNumber < MOTOR_COUNT)
	{
		int posStep = 0;

		switch (motorNumber)
		{
		case 0:
			currentStepMotor0 += dirStep;
			posStep = currentStepMotor0;
			break;
		case 1:
			currentStepMotor1 += dirStep;
			posStep = currentStepMotor1;
			break;
		case 2:
			currentStepMotor2 += dirStep;
			posStep = currentStepMotor2;
			break;
		}

		for (int p = 0; p < MOTOR_PIN_COUNT; p++)
			setPwmSin(MOTOR_PINS[motorNumber][p], posStep + MOTOR_PHASES[p], motorPwmMax);
	}

}



// switch off motor power
// TODO: for some reason motor control gets noisy, if call from ISR
inline void MotorOff(uint8_t motorNumber)
{

	if (motorNumber < MOTOR_COUNT)
	{
		int posStep = 0;
		int motorPwmMax = 0;
		for (int p = 0; p < MOTOR_PIN_COUNT; p++)
			setPwmSin(MOTOR_PINS[motorNumber][p], posStep + MOTOR_PHASES[p], motorPwmMax);
	}

}

void switchOffMotors()
{
	int m = 0;
	for (m = 0; m < MOTOR_COUNT; m++) {
		MotorOff(m);
	}
}

void switchOffAndMoveToPosition(uint8_t motorNumber, uint8_t maxPWM, int position, uint32_t total_delay)
{

	//cliSerial->printf(F("switchOffAndMoveToPosition %d %d %d %d\r\n"),  motorNumber, maxPWM, position, total_delay);

	pwmsin_t pwmResolution = motors_pwm_period;

	uint8_t pwm = 0;

	uint32_t lap = total_delay / maxPWM;
	if (lap == 0)
		lap = 1;

	for (uint8_t i = 1; i <= maxPWM; i++)
	{
		delay(lap);
		pwm = i;
		//cliSerial->printf(F("move %d\r\n"),  pwm);

		MoveMotorPosSpeed(motorNumber, position, pwm);
/*
		pwmsin_t pwmA = getSinusValue( pwm, pwmResolution, position + BL_PHASE_A);
		pwmsin_t pwmB = getSinusValue( pwm, pwmResolution, position + BL_PHASE_B);
		pwmsin_t pwmC = getSinusValue( pwm, pwmResolution, position + BL_PHASE_C);


		if (motorNumber == 0)
		{
			pwmWrite(BOARD_MOT1_CMD_A, pwmA);
			pwmWrite(BOARD_MOT1_CMD_B, pwmB);
			pwmWrite(BOARD_MOT1_CMD_C, pwmC);
		}

		if (motorNumber == 1)
		{
			pwmWrite(BOARD_MOT2_CMD_A, pwmA);
			pwmWrite(BOARD_MOT2_CMD_B, pwmB);
			pwmWrite(BOARD_MOT2_CMD_C, pwmC);
		}

		if (motorNumber == 2)
		{
			pwmWrite(BOARD_MOT3_CMD_A, pwmA);
			pwmWrite(BOARD_MOT3_CMD_B, pwmB);
			pwmWrite(BOARD_MOT3_CMD_C, pwmC);
		}
*/

	}


}

void setPositionAndPower(uint8_t motorNumber, uint8_t pwm, int position)
{
	MoveMotorPosSpeed(motorNumber, position, pwm);
/*
	pwmsin_t pwmResolution = motors_pwm_period;


	pwmsin_t pwmA = getSinusValue( pwm, pwmResolution, position + BL_PHASE_A);
	pwmsin_t pwmB = getSinusValue( pwm, pwmResolution, position + BL_PHASE_B);
	pwmsin_t pwmC = getSinusValue( pwm, pwmResolution, position + BL_PHASE_C);


	if (motorNumber == 0)
	{
		pwmWrite(BOARD_MOT1_CMD_A, pwmA);
		pwmWrite(BOARD_MOT1_CMD_B, pwmB);
		pwmWrite(BOARD_MOT1_CMD_C, pwmC);
	}

	if (motorNumber == 1)
	{
		pwmWrite(BOARD_MOT2_CMD_A, pwmA);
		pwmWrite(BOARD_MOT2_CMD_B, pwmB);
		pwmWrite(BOARD_MOT2_CMD_C, pwmC);
	}

	if (motorNumber == 2)
	{
		pwmWrite(BOARD_MOT3_CMD_A, pwmA);
		pwmWrite(BOARD_MOT3_CMD_B, pwmB);
		pwmWrite(BOARD_MOT3_CMD_C, pwmC);
	}
*/
}



void recalcMotorStuff()
{
  cli();
  calcSinusArray(motors_pwm_period);


//  MOTOR_PHASES[0] = config.profiles[0].pwmPhaseA * N_SIN / 360;
//  MOTOR_PHASES[1] = config.profiles[0].pwmPhaseB * N_SIN / 360;
//  MOTOR_PHASES[2] = config.profiles[0].pwmPhaseC * N_SIN / 360;


  sei();
}


void resetMotorFreq()
{

	uint16 freq = GIMBAL_PWM_FREQ;

	if (config.profiles[0].pwmFrequency < 4)
		config.profiles[0].pwmFrequency = 4;
	if (config.profiles[0].pwmFrequency > 32)
		config.profiles[0].pwmFrequency = 32;


	freq = (uint16) config.profiles[0].pwmFrequency * 1000;

	uint16 period = 0;
	int m = 0; int p = 0;
	for (m = 0; m < MOTOR_COUNT; m++) {
		if (motors_pwm_period != 0)
		{
			MotorOff(m);
		}
		for (p = 0; p < MOTOR_PIN_COUNT; p++) {
			period = pwmSetFrequency(MOTOR_PINS[m][p], freq);
		}
	}
	motors_pwm_period = period;

	recalcMotorStuff();
}


/********************************/
/* Motor Control IRQ Routine    */
/********************************/

void motorInitInterrupt()
{
#ifdef BRUGI_USE_INTERRUPT_TIMER
	//inizializzo interrupt timer
	Timer2.setChannel1Mode(TIMER_OUTPUTCOMPARE);
	Timer2.setPeriod(MOTORUPDATE_FREQ); // in microseconds
	Timer2.setCompare1(1);      // overflow might be small
	Timer2.attachInterrupt(1, motorInterrupt);
#endif
}

// motor position control
uint32_t _last_motor_interrupt = 0;




int _pitchMotorDrive_PREV = 0;
int _rollMotorDrive_PREV = 0;
int _yawMotorDrive_PREV = 0;
int _pitchMotorDrive_INT_step = 0;
int _rollMotorDrive_INT_step = 0;
int _yawMotorDrive_INT_step = 0;
int _count_INT = 0;
bool led_status = false;

//ISR( TIMER1_OVF_vect )
void motorInterrupt_TEST()
{
	uint32_t unow = micros();

	//if ((unow - _last_motor_interrupt) >= DT_INT_US)
	{
		_last_motor_interrupt = unow;


		if (motor_update_values)
		{
			motor_update_values = false;
			_count_INT = 0;

			_pitchMotorDrive_PREV = pitchMotorDrive_PREV;
			_rollMotorDrive_PREV = rollMotorDrive_PREV;
			_yawMotorDrive_PREV = yawMotorDrive_PREV;
			_pitchMotorDrive_INT_step = pitchMotorDrive_INT_step;
			_rollMotorDrive_INT_step = rollMotorDrive_INT_step;
			_yawMotorDrive_INT_step = yawMotorDrive_INT_step;


			if (led_status)
				LEDGREPIN_ON
			else
				LEDGREPIN_OFF
			led_status = !led_status;
		}

		if (enableMotorUpdates)
		{
			_count_INT++;

			// move pitch motor
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisPITCH].motorNumber, _pitchMotorDrive_PREV + _count_INT * _pitchMotorDrive_INT_step, config.profiles[0].axisConfig[axisPITCH].maxPWM);
			// move roll motor
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisROLL].motorNumber, _rollMotorDrive_PREV + _count_INT * _rollMotorDrive_INT_step, config.profiles[0].axisConfig[axisROLL].maxPWM);
			// move roll yaw
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisYAW].motorNumber, _yawMotorDrive_PREV + _count_INT * _yawMotorDrive_INT_step, config.profiles[0].axisConfig[axisYAW].maxPWM);
		}
		// update event
		motorUpdate = true;
	}

	//misuro durata funzione
	uint32_t lap = measure_micro_delay(unow, micros());
	interrupt_mean_duration.append((float) lap);

	//misuro intervallo tra le chiamate
	static uint32_t ulast_motor_interrupt = 0;
	if (ulast_motor_interrupt != 0)
	{
		uint32_t lap2 = measure_micro_delay(ulast_motor_interrupt, unow);
		interrupt_mean_lap.append((float) lap2);
	}
	ulast_motor_interrupt = unow;
}



void motorInterrupt()
{
  // 0.88us / 8.1us
//  freqCounter++;
//  if(freqCounter==(CC_FACTOR*1000/MOTORUPDATE_FREQ))
//  {
 //   freqCounter=0;

	uint32_t unow = micros();

	uint32_t now = millis();


	if ((now - _last_motor_interrupt) >= DT_INT_MS) //1000/MOTORUPDATE_FREQ)
	{
		_last_motor_interrupt = now;
		if (enableMotorUpdates)
		{
			// move pitch motor
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisPITCH].motorNumber, pitchMotorDrive, config.profiles[0].axisConfig[axisPITCH].maxPWM);
			// move roll motor
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisROLL].motorNumber, rollMotorDrive, config.profiles[0].axisConfig[axisROLL].maxPWM);
			// move roll yaw
			MoveMotorPosSpeed(config.profiles[0].axisConfig[axisYAW].motorNumber, yawMotorDrive, config.profiles[0].axisConfig[axisYAW].maxPWM);
		}
		// update event
		motorUpdate = true;
	}

	//misuro durata funzione
	uint32_t lap = measure_micro_delay(unow, micros());
	interrupt_mean_duration.append((float) lap);

	//misuro intervallo tra le chiamate
	static uint32_t ulast_motor_interrupt = 0;
	if (ulast_motor_interrupt != 0)
	{
		uint32_t lap2 = measure_micro_delay(ulast_motor_interrupt, unow);
		interrupt_mean_lap.append((float) lap2);
	}
	ulast_motor_interrupt = unow;
}


void motorMove(uint8_t motorNum, int steps)
{
	if (motorNum == 0)
		fastMoveMotor(0, steps, config.profiles[0].axisConfig[axisROLL].maxPWM);
	if (motorNum == 1)
		fastMoveMotor(1, steps, config.profiles[0].axisConfig[axisPITCH].maxPWM); //config.profiles[0].axisConfig[axisPITCH].motorNumber
	if (motorNum == 2)
		fastMoveMotor(2, steps, config.profiles[0].axisConfig[axisYAW].maxPWM);
}

void motorTest()
{
  #define MOT_DEL 10
  cli();
  delay(10 * CC_FACTOR);
  // Move Motors to ensure function
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisPITCH].motorNumber, 1,config.profiles[0].axisConfig[axisPITCH].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisPITCH].motorNumber, -1,config.profiles[0].axisConfig[axisPITCH].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  delay(200 * CC_FACTOR);
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisROLL].motorNumber, 1,config.profiles[0].axisConfig[axisROLL].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisROLL].motorNumber, -1,config.profiles[0].axisConfig[axisROLL].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  delay(200 * CC_FACTOR);
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisYAW].motorNumber, 1,config.profiles[0].axisConfig[axisYAW].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  for(int i=0; i<N_SIN; i++) { fastMoveMotor(config.profiles[0].axisConfig[axisYAW].motorNumber, -1,config.profiles[0].axisConfig[axisYAW].maxPWM); delay(MOT_DEL * CC_FACTOR);  }
  sei();  
}



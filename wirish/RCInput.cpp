#include <ext_interrupts.h>
#include <timer.h>
#include "RCInput.h"
#include <pwm_in.h>

//extern const AP_HAL::HAL& hal;



// MP32F4
#define PPM_IN_CH1 BOARD_PWM_IN0 //22
#define PPM_IN_CH2 BOARD_PWM_IN1 //63  // PA8
#define PPM_IN_CH3 BOARD_PWM_IN2 //66  //
#define PPM_IN_CH4 BOARD_PWM_IN3 //89
//#define PPM_IN_CH5 59
//#define PPM_IN_CH6 62
//#define PPM_IN_CH7 0 //57
//#define PPM_IN_CH8 60

#define RISING_EDGE 1
#define FALLING_EDGE 0
#define MINONWIDTH 950
#define MAXONWIDTH 2075
// PATCH FOR FAILSAFE AND FRSKY
#define MINOFFWIDTH 1000
#define MAXOFFWIDTH 22000

#define MINCHECK 900
#define MAXCHECK 2100



// STANDARD PPM VARIABLE

volatile uint16_t rcPinValue[RCINPUT_NUM_CHANNELS] =
    {
    1500, 1500, 1000, 1500
    };
// interval [1000;2000]
static byte receiverPin[RCINPUT_NUM_CHANNELS] =
    {
	    PPM_IN_CH1,
	    PPM_IN_CH2,
	    PPM_IN_CH3,
	    PPM_IN_CH4

    };

// ***PPM SUM SIGNAL***
static uint8_t rcChannel[12];
volatile uint16_t rcValue[20] =
    {
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500
    }; // interval [1000;2000]
volatile uint16_t rcTmpValue[20] =
    {
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500,
	    1500
    }; // interval [1000;2000]

// Variable definition for Input Capture interrupt
//volatile uint16_t APM_RC_MP32::_PWM_RAW[NUM_CHANNELS] = {2400,2400,2400,2400,2400,2400,2400,2400};
//volatile uint8_t APM_RC_MP32::_radio_status=0;

volatile unsigned char radio_status_rc = 0;
volatile unsigned char sync = 0;
static int analogOutPin[20];
volatile unsigned int currentChannel = 0;
static unsigned int last = 0;

unsigned int uiRcErrCnt1 = 0;
unsigned int uiRcErrCnt2 = 0;
unsigned int uiRcErrCnt3 = 0;

typedef struct
    {
    byte edge;
    unsigned long riseTime;
    unsigned long fallTime;
    unsigned int lastGoodWidth;
    } tPinTimingData;
volatile static tPinTimingData pinData[RCINPUT_NUM_CHANNELS];

//// PE5 is PIN69
//#define PIN69	69
//#define PIN69IN	(*((unsigned long int *) 0x42420214))
//#define PIN69OUT	(*((unsigned long int *) 0x42420294))
//
//// PE6 is PIN70
//#define PIN70	70
//#define PIN70IN	(*((unsigned long int *) 0x42420218))
//#define PIN70OUT	(*((unsigned long int *) 0x42420298))


void rxIntPPMSUM(void)
    {
    volatile unsigned int now;
    volatile unsigned int diff;
    int i;

//    hal.scheduler->suspend_timer_procs();

//    now = hal.scheduler->micros();
    diff = now - last;
    last = now;
    if (diff > 4000 && diff < 21000) // Sincro del frame
	{
	currentChannel = 0;
	radio_status_rc = 0;
	if (uiRcErrCnt1 == 0)
	    {		// if the frame is error free, copy it to rcValue Array
	    for (i = 0; i < 9; i++)
		{
		rcValue[i] = rcTmpValue[i]; // THE PPMSUM VALUE START FROM 10 ' STANDARD PPM channel < 10
		}
	    }
	sync = 1;
	uiRcErrCnt1 = 0;	// Reset Error counter
	}
    else if ((diff > 2400) || (diff < 650))
	{// the signal from my jeti receiver goes around 740 to 1550 ms, with <650 or >2000 bad data will be recorded
	uiRcErrCnt1++;
	}
    if (sync == 1)
	{
	//rcValue[currentChannel] = diff;
	rcTmpValue[currentChannel] = diff;
	currentChannel++;
	if (diff <= MAXCHECK && diff >= MINCHECK)
	    radio_status_rc++;
	}
    if (currentChannel > 9)
	{
	//currentChannel=0;
	sync = 0;
	radio_status_rc = 0;
	}

//    hal.scheduler->resume_timer_procs();
    }

//// PE7 is PIN71
//#define PIN71	71
//#define PIN71IN	(*((unsigned long int *) 0x4242021C))
//#define PIN71OUT	(*((unsigned long int *) 0x4242029C))


/*
 0 PE9		75	PWM_IN0		 IRQ 5-9  * Conflict  PPM1
 1 PE11		80	PWM_IN1		 IRQ 10-15			  PPM2
 2 PE13		86	PWM_IN2		 IRQ 10-15			  PPM3
 3 PE14		89	PWM_IN3		 IRQ 10-15			  PPM4
 4 PC6		12	PWM_IN4		 IRQ 5-9			  PPM5
 5 PC7		13	PWM_IN5		 IRQ 5-9			  PPM6
 6 PC8		14	PWM_IN6		 IRQ 5-9			  PPM7
 7 PC9		15	PWM_IN7	     IRQ 5-9   * Conflict (PPMSUM)
 */

// Constructors ////////////////////////////////////////////////////////////////

//using namespace VRBRAIN;

	/* ADD ON PIN NORMALLY AVAILABLE ON RX BUT IF PPM SUM ACTIVE AVAILABLE AS SERVO OUTPUT */

void RCInput::InitDefaultPPMSUM(char board)
	    {
	    switch (board)
		{
	    case 10:
		ppm_sum_channel = BOARD_PWM_IN0; //75;
		rcChannel[0] = 0;
		rcChannel[1] = 1;
		rcChannel[2] = 2;
		rcChannel[3] = 3;
		rcChannel[4] = 4;
		rcChannel[5] = 5;
		rcChannel[6] = 6;
		rcChannel[7] = 7;
		rcChannel[8] = 8;
		break;

	    case 11:
		ppm_sum_channel = BOARD_PWM_IN0; //75;
		rcChannel[0] = 0;
		rcChannel[1] = 1;
		rcChannel[2] = 2;
		rcChannel[3] = 3;
		rcChannel[4] = 4;
		rcChannel[5] = 5;
		rcChannel[6] = 6;
		rcChannel[7] = 7;
		rcChannel[8] = 8;

		break;
		}
	    }

void RCInput::InitPPM(void)
	    {
	        for(byte channel = 0; channel < RCINPUT_NUM_CHANNELS; channel++)
    		   pinData[channel].edge = FALLING_EDGE;

    	//attachPWMCaptureCallback(PWMCaptureCallback);
    	pwmInit();
}

void RCInput::InitDefaultPPM(char board)
{
		// MP32V1F1
		input_channel_ch1 = BOARD_PWM_IN0;
		input_channel_ch2 = BOARD_PWM_IN1;
		input_channel_ch3 = BOARD_PWM_IN2;
		input_channel_ch4 = BOARD_PWM_IN3;

}

// Public Methods //////////////////////////////////////////////////////////////
void RCInput::InitPPMSUM(void)
	    {
	    pinMode(ppm_sum_channel, INPUT);
	    attachInterrupt(ppm_sum_channel, rxIntPPMSUM, RISING);
	    }

uint16_t RCInput::InputCh(unsigned char ch)
	    {
	    uint16_t data;
	    noInterrupts();
	    if (_iboard < 10)
		data = pwmRead(ch);
	    else
		{
		data = rcValue[rcChannel[ch + 1]];
		}
	    interrupts();
	    return data; // We return the value correctly copied when the IRQ's where disabled
	    }

unsigned char RCInput::GetState(void)
	    {
	    return (radio_status_rc);
	    }

RCInput::RCInput()
	    {
	    }

void RCInput::init()
	    {
	    _iboard = 2;
	    if (_iboard < 10)
		{
		// Init Radio In
//		hal.console->println("Init Default PPM");
		InitPPM();
		}
	    else
		{
		// Init Radio In
//		hal.console->println("Init Default PPMSUM");
		InitDefaultPPMSUM(_iboard);
//		hal.console->println("Init PPMSUM HWD");
		InitPPMSUM();

		}

	    }

uint8_t RCInput::valid_channels()
	    {
	    return 1;
	    }

uint16_t RCInput::read(uint8_t ch)
	    {
	    uint16_t data;
	    noInterrupts();
	    if (_iboard < 10){
		//data = rcPinValue[ch];
		data = pwmRead(ch);
	    }
	    else
		{
		data = rcValue[rcChannel[ch + 1]];
		}
	    interrupts();
	    return data; // We return the value correctly copied when the IRQ's where disabled
	    }

	uint8_t RCInput::read(uint16_t* periods, uint8_t len)
	    {
	    noInterrupts();
	    for (uint8_t i = 0; i < len; i++)
		{
		if (_iboard < 10)
		    //periods[i] = rcPinValue[i];
		    periods[i] = pwmRead(i);
		else
		    {
		    periods[i] = rcValue[rcChannel[i + 1]];
		    }
		}
	    interrupts();
	    return len;
	    }

	bool RCInput::set_overrides(int16_t *overrides, uint8_t len)
	    {
	    return true;
	    }

	bool RCInput::set_override(uint8_t channel, int16_t override)
	    {
	    return true;
	    }

	void RCInput::clear_overrides()
	    {
	    }


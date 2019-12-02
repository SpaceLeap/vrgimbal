/*
 * AP_LANC.cpp
 *
 *  Created on: 20/ago/2013
 *      Author: Murtas Matteo
 */

#include "AP_LANC.h"
#include "wirish.h"

#define bitMicroSeconds 103 //  // 102 bad sony / good cannon, 103, good (canon & sony), 104 (spec) good sony bad cannon
#define MAX_WAIT_MILLISEC 20 // 17 measures from sony

AP_LANC::AP_LANC() {
	// TODO Auto-generated constructor stub

}

AP_LANC::~AP_LANC() {
	// TODO Auto-generated destructor stub
}

void AP_LANC::Init(int pin)
{
	m_LANC_X_PIN = pin;
	pinMode(m_LANC_X_PIN,INPUT);
}

void AP_LANC::SendCode(int type,int code) {
  /* Okay i really don't know why i have to send this twice biut it works:
     Sonys need 2 cannons 3
  */
    frameStartBitWait();
    writeByte(m_LANC_X_PIN,type,bitMicroSeconds);
    lowWait(m_LANC_X_PIN);
    writeByte(m_LANC_X_PIN,code,bitMicroSeconds);

    frameStartBitWait();
    writeByte(m_LANC_X_PIN,type,bitMicroSeconds);
    lowWait(m_LANC_X_PIN);
    writeByte(m_LANC_X_PIN,code,bitMicroSeconds);

    frameStartBitWait();
    writeByte(m_LANC_X_PIN,type,bitMicroSeconds);
    lowWait(m_LANC_X_PIN);
    writeByte(m_LANC_X_PIN,code,bitMicroSeconds);
}

void AP_LANC::lowWait(int pin) {
  byte in;
  unsigned long start = millis();
  do {
    in = digitalRead(pin);
  }
  while (in && millis() < start + MAX_WAIT_MILLISEC );
}

void AP_LANC::frameStartBitWait() {
  // finds the start of a telegram/frame
  unsigned long usec = 0;
  unsigned long start = millis();
  do {
    usec = pulseIn(m_LANC_X_PIN,HIGH);

    /* DEBUG
     if (usec > 1200 && usec < candidate) {
     candidate = usec;
     Serial.print(usec); Serial.println(" usec");
     }
     */
    // } while (usec < 5864);  // works for sony TVR900 & HDR-HC9
    // } while (usec < 5085);  //  5089 // Canon
  }
  while (usec < 1450  && millis() < start + MAX_WAIT_MILLISEC); //  Frame Lengths are 1200-1400 dep. on device
  /* DEBUG */
  //Serial.print("frame start after "); Serial.print(usec); Serial.println(" usec");
}

void AP_LANC::writeByte(int pin, unsigned char value, unsigned uSec /* bit width */) {


  delayMicroseconds(uSec); // wait for stop bit
  pinMode(pin,OUTPUT);
  for (int i = 0; i < 8; i++) {
    boolean bit = value & 0x1;
    digitalWrite(pin,!bit); // NOT (!) pin because all data is inverted in LANC
    value >>= 1;
    delayMicroseconds(uSec);
  }

  //digitalWrite(pin,HIGH);
  //delayMicroseconds(uSec);

  pinMode(pin,INPUT);
}

unsigned char AP_LANC::readByte(int pin,unsigned long uSec /* bit width*/ ) {
	unsigned char result = 0;
  delayMicroseconds(uSec * 1.5); // skips the Start Bit and Land in the midlle of the first byte

  for (int i = 0; i < 8; i++) {
    if (digitalRead(pin) == LOW) { // == *LOW* because bits inverted in LANC
      result++;
    }
    result <<= 1;
    delayMicroseconds(uSec);
  }
  delayMicroseconds(0.5*uSec);
  return result; // return happens at end of last (8ths) bit
}

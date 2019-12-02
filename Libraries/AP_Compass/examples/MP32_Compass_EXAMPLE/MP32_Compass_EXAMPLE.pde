/*
	Example of APM_Compass library (HMC5843 sensor).
	Code by Jordi MuÃ’oz and Jose Julio. DIYDrones.com
*/

//#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Compass.h> // Compass Library
#include <AP_Math.h>		// ArduPilot Mega Vector/Matrix math Library
//#include <Wire.h>

#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi

AP_Compass_HMC5843 compass;


unsigned long timer;

void setup()
{
  Serial1.begin(38400);
  Serial1.println("Compass library test (HMC5843)");
  
  compass.init();	 // Initialization

  compass.set_orientation(AP_COMPASS_COMPONENTS_UP_PINS_FORWARD);  // set compass's orientation on aircraft
  compass.set_offsets(0,0,0);  // set offsets to account for surrounding interference
  compass.set_declination(ToRad(0.0));  // set local difference between magnetic north and true north

  delay(1000);
  timer = millis();
}

void loop()
{
  static float min[3], max[3], offset[3];

  if((millis()- timer) > 100)
  {
    timer = millis();
    compass.read();
    compass.calculate(0,0);  // roll = 0, pitch = 0 for this example

    // capture min
    if( compass.mag_x < min[0] )
        min[0] = compass.mag_x;
    if( compass.mag_y < min[1] )
        min[1] = compass.mag_y;
    if( compass.mag_z < min[2] )
        min[2] = compass.mag_z;

    // capture max
    if( compass.mag_x > max[0] )
        max[0] = compass.mag_x;
    if( compass.mag_y > max[1] )
        max[1] = compass.mag_y;
    if( compass.mag_z > max[2] )
        max[2] = compass.mag_z;

    // calculate offsets
    offset[0] = -(max[0]+min[0])/2;
    offset[1] = -(max[1]+min[1])/2;
    offset[2] = -(max[2]+min[2])/2;

    // display all to user
    Serial1.print("Heading:");
    Serial1.print(ToDeg(compass.heading));
    Serial1.print("  (");
    Serial1.print(compass.mag_x);
    Serial1.print(",");
    Serial1.print(compass.mag_y);
    Serial1.print(",");
    Serial1.print(compass.mag_z);
    Serial1.print(")");

    // display offsets
    Serial1.print("\t offsets(");
    Serial1.print(offset[0]);
    Serial1.print(",");
    Serial1.print(offset[1]);
    Serial1.print(",");
    Serial1.print(offset[2]);
    Serial1.print(")");

    Serial1.println();
  }
}


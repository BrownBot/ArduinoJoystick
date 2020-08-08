// Peter Leahy, August 2020
// Joystick with Arduino Micro

// CONFIG
#define MAX_SWITCHES 6 // the number of switches
byte switch_pin[MAX_SWITCHES] = {2,3,4,5,6,7};//,8,9,10,11,12,14,15}; // digital input pins
#define DEBOUNCE_TIME 5 // ms delay before the push button changes state
#define MAX_ANALOG 2 // the number of analog inputs
byte analog_pin[MAX_ANALOG] = {A0,A1};//,A2,A3}; // analog input pins X,Y,Z,THROT

// END CONFIG

// DECLARATIONS
#include "Joystick.h"
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, MAX_SWITCHES, 0, true, true, false, false, false, false, false, false, false, false, false);
byte reading, clk, clk_old;
byte switch_state[MAX_SWITCHES];
byte switch_state_old[MAX_SWITCHES];
byte switch_state_count[MAX_SWITCHES];
int analog_value[MAX_ANALOG]; 
unsigned long debounce_time[MAX_SWITCHES]; 
// END DECLARATIONS

// SETUP
void setup() {
	for (byte i=0; i<MAX_SWITCHES; i++) 
  {
	  pinMode(switch_pin[i],INPUT_PULLUP);
  }

	pinMode(13,OUTPUT); // on board LED
	digitalWrite(13,0);
	Joystick.begin(false); 
	Joystick.setXAxisRange(-511, 511);
	Joystick.setYAxisRange(-511, 511);
	Joystick.setZAxisRange(-511, 511);
	Joystick.setRudderRange(ENC_MIN, ENC_MAX);
	Joystick.setThrottleRange(0, 1023);
} 
// END SETUP

// LOOP
void loop() 
{
	for (byte i=0; i<MAX_SWITCHES; i++) 
	{ 
	  // read the switches
    reading = !digitalRead(switch_pin[i]);
    if (reading == switch_state[i]) 
    {
      debounce_time[i] = millis() + (unsigned long)DEBOUNCE_TIME;
    }
    else if (millis() > debounce_time[i]) 
    {
      switch_state[i] = reading;

    }

    if(i == 2 || i == 3)
    {
      // press on state change
  		if (switch_state[i] != switch_state_old[i]) 
  		{ // debounced button has changed state
  			// this code is executed once after change of state
  			digitalWrite(13,switch_state[i]);
        Joystick.pressButton(i);
  
  			switch_state_old[i] = switch_state[i]; // store new state such that the above gets done only once
        switch_state_count[i] = 0;
  		}
      else
      {
        switch_state_count[i]++;
        if(switch_state_count[i] > 3)
        {
          Joystick.releaseButton(i);
        }
      }
    }
    else
    {
      if (switch_state[i] != switch_state_old[i]) 
      { // debounced button has changed state
        // this code is executed once after change of state
        digitalWrite(13,switch_state[i]);
        if (switch_state[i]) 
        {
          Joystick.pressButton(i);
        }
        else 
        {
          Joystick.releaseButton(i);
        }
  
        switch_state_old[i] = switch_state[i]; // store new state such that the above gets done only once
      }
    }
	} //END read the switches

	for (byte i=0; i<MAX_ANALOG; i++) 
	{ 
	  // read analog inputs
		analog_value[i] = analogRead(analog_pin[i]);
		if (analog_value[i] < 256)
		{
		  analog_value[i] = analog_value[i] * 1.5;
		}
		else if (analog_value[i] < 768) 
		{
		  analog_value[i] = 256 + analog_value[i] / 2;
		}
		else
		{
		  analog_value[i] = 640 + (analog_value[i] - 768) * 1.5;
		}
   
		switch(i) 
		{
			case 0:
				Joystick.setXAxis(-511 + analog_value[0]);
			break;
			case 1:
				Joystick.setYAxis(511 - analog_value[1]);
			break;
			case 2:
				Joystick.setZAxis(511 - analog_value[2]);
			break;
			case 3:
				Joystick.setThrottle(analog_value[3]);
			break;
		}
	}

	Joystick.sendState();
	delay(10);
} // END LOOP

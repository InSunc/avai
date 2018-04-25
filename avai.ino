#include <NewPing.h>
#include <NewTone.h>
#include "typedef.h"

#define DEBOUNCE_TIME 20000
#define BUTTON_DEBOUNCE 90000

#define PLATFORM_TIMEOUT 10000
#define RAMP_TIMEOUT 100000


Motor motor[2];

u8 iPlatformOpen   = 17;
u8 iPlatformClosed = 16;
u8 iPlatformEnable = 15;
u8 iRampSensors    = 18;
u8 iProximityTrig  = 4;
u8 iProximityEcho  = 5;
u8 iChair          = 8;
u8 iBuzzer         = 12;
u8 iRampRelay      = 14;

u8 iButtonPin      = 3;
u8 iRampButton     = 13;

state actualState;
state nextState;

event action;
event prevAction;

u32 startTimer;
u32 readTimer = 0;

bool activated = false;
bool beep = false;

NewPing sonar(iProximityTrig, iProximityEcho, 35);


void setup()
{
	Serial.begin(9600);

	motor[0].iPinRight = 9;
	motor[0].iPinLeft  = 6;
	motor[1].iPinRight = 11;
	motor[1].iPinLeft  = 10;

	pinMode(7, INPUT);

	// PLATFORM MOTOR
	pinMode(motor[0].iPinRight, OUTPUT);
	pinMode(motor[0].iPinLeft,  OUTPUT);

	// RAMP MOTOR
	pinMode(motor[1].iPinRight, OUTPUT);
	pinMode(motor[1].iPinLeft,  OUTPUT);

	// SENSORS
	pinMode(iPlatformOpen,   INPUT);
	pinMode(iPlatformClosed, INPUT);
	pinMode(iRampSensors,    INPUT);
	pinMode(iRampButton,     INPUT);
	pinMode(iChair,          INPUT);
	pinMode(iButtonPin,      INPUT_PULLUP);

	pinMode(iRampRelay,      OUTPUT);
	digitalWrite(iRampRelay, LOW);

	pinMode(iPlatformEnable, OUTPUT);
	digitalWrite(iPlatformEnable, HIGH);

	
	actualState = getActualState();
	nextState = actualState;

	action = PAUSE;
	prevAction = getPrevAction();

	NewTone(iBuzzer,1532);
	delay(250);
	pinMode(iBuzzer,INPUT);
	delay(100);
	pinMode(iBuzzer,OUTPUT);
	NewTone(iBuzzer,932);
	delay(250);
	pinMode(iBuzzer,INPUT);
	beep = false;

}



void loop()
{
	digitalWrite(iPlatformEnable, HIGH);

	if(debounce(iChair, DEBOUNCE_TIME, 1))
	{
		Serial.println("CHAIR ON");

		if(debounce(iRampButton, DEBOUNCE_TIME, 1))
		{
			Serial.println("------------RAMP_BTN APASAT");
			actualState = getActualState();
			if((actualState == RAMP_OPEN) || (actualState == RAMP_CLOSED))
				vButtonClick();
		}

		if(action != PAUSE)
		{
			if(!activated)
				vGetNextState();

			vExecNextState();
		}
		else Serial.println(" RRR  P A U S E");
		delay(350);
	}
	else
	{
		if(debounce(iButtonPin, BUTTON_DEBOUNCE, 0))
		{
			Serial.println("------------REMOTE APASAT");
			actualState = getActualState();
			vButtonClick();
		}

		if(action != PAUSE)
		{
			if(!activated)
				vGetNextState();

			vExecNextState();
		}
		else Serial.println(".");
		delay(500);
	}

}

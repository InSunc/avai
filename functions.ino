void(* resetFunc) (void) = 0;


void vError(String msg)
{
	Serial.println(msg);
	pinMode(iBuzzer,OUTPUT);
	stopMotors();
	NewTone(iBuzzer,932);
	delay(2000);
	pinMode(iBuzzer,INPUT);
	delay(2000);
	resetFunc();
}


state getStateOf(element e)
{
	pinMode(iPlatformEnable, INPUT);

	digitalWrite(motor[0].iPinLeft, HIGH);
	bool platformOpenSensor  = false;

	digitalWrite(motor[0].iPinRight, HIGH);
	bool platformClosedSensor = false;

	readTimer = millis();
	while((millis() - readTimer) < 800)
	{
		Serial.println(millis()-readTimer);
		if(debounce(iPlatformOpen, DEBOUNCE_TIME, 1))
		{
			Serial.println("FOUND PLATFORM OPEN");
			platformOpenSensor = true;
		}

		if(debounce(iPlatformClosed, DEBOUNCE_TIME, 1))
		{
			Serial.println("FOUND PLATFORM CLOSED");
			platformClosedSensor = true;
		}

		if(platformOpenSensor || platformClosedSensor)
		{
			Serial.println("FOUND");
			break;
		}
	}

	digitalWrite(motor[0].iPinLeft, LOW);
	digitalWrite(motor[0].iPinRight, LOW);

	pinMode(iPlatformEnable, OUTPUT);



	bool rampSensors = debounce(iRampSensors, DEBOUNCE_TIME, 0);


	if((platformOpenSensor == true) && (platformOpenSensor == platformClosedSensor))
		vError("[ ERROR ] PLATFORM");

	if(((platformClosedSensor == true) && (rampSensors == false)) || (((platformOpenSensor == platformClosedSensor) && platformOpenSensor == false) && rampSensors == false))
	{
		if(((platformClosedSensor == true) && (rampSensors == false)))
			Serial.println("111111111111111111111111111111");
			
		if(((platformOpenSensor == platformClosedSensor) && platformOpenSensor == false) && rampSensors == false) {
			Serial.println("2222222222222222222222222");
			Serial.println(platformClosedSensor);
			Serial.println(platformOpenSensor);
			Serial.println(rampSensors);
			}
		

		vError("[ ERROR ] RAMP");
	}


	state resultState;

	switch(e)
	{
		case PLATFORM:

			if(platformOpenSensor == true)
				resultState = PLATFORM_OPEN;
			else
			{
				if(platformClosedSensor == true)
					resultState = PLATFORM_CLOSED;
			}
			break;

		case RAMP:
			if(rampSensors == true)
				resultState = RAMP_CLOSED;
			else
				resultState = RAMP_OPEN;
	}
	return resultState;
}



state getActualState()
{
	if(getStateOf(PLATFORM) == PLATFORM_OPEN)
	{
		Serial.println("GetActualState: PLATFORM_OPEN");
		if(getStateOf(RAMP) == RAMP_OPEN) {
			Serial.println("GetActualState: RAMP_OPEN");
			return RAMP_OPEN;
		}
		else
		{
			if(debounce(iChair,DEBOUNCE_TIME, 1) == true)
			{
				Serial.println("Final ActualState: RAMP_CLOSED");
				return RAMP_CLOSED;
			}
			else
			{
				Serial.println("Final ActualState: PLATFORM_OPEN");
				return PLATFORM_OPEN;
			}
		}
	}
	else
	{
		Serial.println("GetActualState: PLATFORM_CLOSED");
		return PLATFORM_CLOSED;
	}
}



event getPrevAction()
{
	switch(actualState)
	{
		case PLATFORM_CLOSED:
			Serial.println("prevAction = CLOSE");
			return CLOSE;

		case PLATFORM_OPEN:
			Serial.println("prevAction = CLOSE");
			return CLOSE;

		case RAMP_CLOSED:
			if(debounce(iChair,DEBOUNCE_TIME, 1) == true)
			{
				Serial.println("prevAction = CLOSE");
				return CLOSE;
			}
			else
			{
				Serial.println("prevAction = OPEN");
				return OPEN;
			}

		case RAMP_OPEN:
			Serial.println("prevAction = OPEN");
			return OPEN;
	}
}



void stopMotors()
{
	Serial.println("\t MOTORS STOPPPED");
	digitalWrite(motor[0].iPinRight, LOW);
	digitalWrite(motor[0].iPinLeft, LOW);
	digitalWrite(motor[1].iPinRight, LOW);
	digitalWrite(motor[1].iPinLeft, LOW);
	activated = false;
}



void vButtonClick()
{
	switch(action)
	{
		case CLOSE:
			Serial.println("Action = PAUSE");
			prevAction = CLOSE;
			action = PAUSE;
			stopMotors();
			break;

		case OPEN:
			Serial.println("Action = PAUSE");
			prevAction = OPEN;
			action = PAUSE;
			stopMotors();
			break;

		case PAUSE:
			switch(prevAction)
			{
				case OPEN:
					Serial.println("Action = CLOSE");
					action = CLOSE;
					break;

				case CLOSE:
					Serial.println("Action = OPEN");
					action = OPEN;
					break;
			
			}
			break;
	}
}



void vGetNextState()
{
	switch(action)
	{
		case OPEN:
			switch(actualState)
			{
				case PLATFORM_CLOSED:
					Serial.println("nextState = PLATFORM_OPEN");
					nextState = PLATFORM_OPEN;
					break;

				case PLATFORM_OPEN:
					Serial.println("nextState = RAMP_OPEN");
					nextState = RAMP_OPEN;
					break;

				case RAMP_CLOSED:
					if(debounce(iChair, DEBOUNCE_TIME, 1))
					{
						nextState = RAMP_OPEN;
						Serial.println("nextState = RAMP_OPEN");
					}
					else
					{
						if(actualState == nextState)
						{
							Serial.println("nextState = RAMP_OPEN");
							nextState = RAMP_OPEN;
						}
						else
						{
							Serial.println("nextState = PLATFORM_OPEN");
							nextState = PLATFORM_OPEN;
						}
					}
					break;

				case RAMP_OPEN:
					if(actualState == nextState)
					{
						Serial.println("nextState = ----> action = PAUSE");
						action = PAUSE;
					}
					else
					{
						Serial.println("nextState = RAMP_OPEN");
						nextState = RAMP_OPEN;
					}
					break;
			}
			break;

		case CLOSE:
			switch(actualState)
			{
				case PLATFORM_CLOSED:
					if(actualState == nextState)
					{
						Serial.println("nextState = ----> action = PAUSE");
						action = PAUSE;
					}
					else
					{
						Serial.println("nextState = PLATFORM_CLOSED");
						nextState = PLATFORM_CLOSED;
					}
					break;

				case PLATFORM_OPEN:
					if(actualState == nextState)
					{
						Serial.println("nextState = PLATFORM_CLOSED");
						nextState = PLATFORM_CLOSED;
					}
					else
					{
						Serial.println("nextState = RAMP_CLOSED");
						nextState = RAMP_CLOSED;
					}
					break;

				case RAMP_CLOSED:
					if(debounce(iChair, DEBOUNCE_TIME, 1))
					{
						Serial.println("nextState = RAMP_OPEN");
						nextState = RAMP_OPEN;
					}
					else
					{
						Serial.println("nextState = PLATFORM_CLOSED");
						nextState = PLATFORM_CLOSED;
					}
					break;

				case RAMP_OPEN:
					Serial.println("nextState = RAMP_CLOSED");
					nextState = RAMP_CLOSED;
					break;
			}
			break;
	}
}


void vExecNextState()
{
	switch(nextState)
	{
		case PLATFORM_CLOSED:
			if(!activated)
			{
				pinMode(motor[0].iPinLeft, OUTPUT);
				Serial.println("ACTIVATED - PLATFORM_LEFT");
				digitalWrite(motor[0].iPinLeft, HIGH);
				startTimer = millis();
				activated = true;
				break;
			}
			else
			{
				if(debounce(iPlatformClosed, DEBOUNCE_TIME, 1) == true /*|| (millis() - startTimer >= PLATFORM_TIMEOUT)*/)
				{
					Serial.println("STOPPED - PLATFORM_LEFT");
					digitalWrite(motor[0].iPinLeft, LOW);
					pinMode(motor[0].iPinLeft, INPUT);
					actualState = nextState;
					activated = false;
					action = PAUSE;
					prevAction = CLOSE;
				}
			}
			break;

		case PLATFORM_OPEN:
			if(!activated)
			{
				pinMode(motor[0].iPinRight, OUTPUT);
				Serial.println("ACTIVATED - PLATFORM_RIGHT");
				digitalWrite(motor[0].iPinRight, HIGH);
				startTimer = millis();
				activated = true;
				break;
			}
			else
			{
				if(debounce(iPlatformOpen, DEBOUNCE_TIME, 1) == true || (millis() - startTimer >= PLATFORM_TIMEOUT))
				{
					Serial.println("PLATFORM OPEN STOPPED - PLATFORM_RIGHT");
					digitalWrite(motor[0].iPinRight, LOW);
					pinMode(motor[0].iPinRight, INPUT);
					actualState = nextState;
					activated = false;

					if(millis() - startTimer >= PLATFORM_TIMEOUT)
					{
						action = PAUSE;
						prevAction = OPEN;
						actualState = PLATFORM_OPEN;
						nextState = PLATFORM_OPEN;
					}
				}
				else
				{
					if(sonar.ping_cm() < 30 && sonar.ping_cm() != 0)
					{
						Serial.println("PROXIMITY_STOPPED - PLATFORM_RIGHT");
						digitalWrite(motor[0].iPinRight, LOW);
						pinMode(motor[0].iPinRight, INPUT);
						action = PAUSE;
						prevAction = CLOSE;
						activated = false;
					}
				}
			}
			break;

		case RAMP_CLOSED:
			if(!activated)
			{
				digitalWrite(iRampRelay, HIGH);
				pinMode(motor[1].iPinLeft, OUTPUT);
				Serial.println("ACTIVATED - RAMP_LEFT");
				digitalWrite(motor[1].iPinLeft, HIGH);
				startTimer = millis();
				activated = true;
				break;
			}
			else
			{
				if(debounce(iRampSensors, DEBOUNCE_TIME, 0) || (millis() - startTimer >= RAMP_TIMEOUT))
				{
					Serial.println("STOPPED - RAMP_LEFT");
					digitalWrite(motor[1].iPinLeft, LOW);
					pinMode(motor[1].iPinLeft, INPUT);
					actualState = nextState;
					activated = false;
					digitalWrite(iRampRelay, LOW);

					if(millis() - startTimer >= RAMP_TIMEOUT)
					{
						Serial.println("RAMP_CLOSED TIMEOUT");
						action = PAUSE;
					}

					if(debounce(iChair, DEBOUNCE_TIME, 1))
					{
						action = PAUSE;
						prevAction = CLOSE;
					}
				}
			}
			break;

		case RAMP_OPEN:
			if(!activated)
			{
				pinMode(motor[1].iPinRight, OUTPUT);
				Serial.println("ACTIVATED - RAMP_RIGHT");
				digitalWrite(motor[1].iPinRight, HIGH);
				startTimer = millis();
				activated = true;
				break;
			}
			else
			{
				Serial.println(millis() - startTimer);
				if(millis() - startTimer >= 27500)
				{
					digitalWrite(iRampRelay, HIGH);
					Serial.println("RAMP RELAY TRANSMIS");
				}
				else
				{
					digitalWrite(iRampRelay, LOW);
					Serial.println("RELAY LOW");
				}
					


				if(millis() - startTimer >= RAMP_TIMEOUT)
				{
					Serial.println("STOPPED - RAMP_RIGHT" + (millis() - startTimer));
					digitalWrite(motor[1].iPinRight, LOW);
					pinMode(motor[1].iPinRight, INPUT);
					digitalWrite(iRampRelay, LOW);
					actualState = nextState;
					activated = false;
					action = PAUSE;
					prevAction = OPEN;
				}
				actualState = nextState;
			}
			break;
	}
}



bool debounce(int pin, int debounceLimit, int front ) {

    int i,debounceLimitIncrement;
    for(int i = 0;i < debounceLimit;i++) {

      int current = digitalRead(pin);
       if(front == current) {
           debounceLimitIncrement++;
       }
        else {
           debounceLimitIncrement = 0;
           return false;
        }
    }
     if(debounceLimitIncrement >= debounceLimit) {
       debounceLimitIncrement = 0;
       return true;
     }
}

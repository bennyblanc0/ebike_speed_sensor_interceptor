/*
Name:    ebike_speed_sensor_interceptor.ino
Created: 5/24/2018 6:09:59 PM
Author:  benja
*/

//Set global variables
unsigned long previousSense = 0;
unsigned long previousPulse = 0;
unsigned long currentSenseInterval = 100000;  //Initialise to a big number to be safe
int sensorPin = 12;
int motorPin = 4;
int groundPin = 8;
int logicHighPin = 11;
int sensorState = 0;
int minimumPulseInterval = 270; //30 Kmh on the speedo
int activeSense = 0;
int activePulse = 0;

//Runs once at startup
void setup() {
	pinMode(sensorPin, INPUT);
	pinMode(motorPin, OUTPUT);
	digitalWrite(motorPin, LOW);
	pinMode(groundPin, OUTPUT);
	digitalWrite(groundPin, LOW);
	pinMode(logicHighPin, OUTPUT);
	digitalWrite(logicHighPin, HIGH);
	Serial.begin(9600);
	Serial.println("Opening serial port >> Ebike sensor");
}

//IMPORTANT: We need to read every sense on the hall effect sensor so the code in loop() needs to be very lean
void loop() {
	pinMode(groundPin, OUTPUT);

	//Read from the hall effect sensor for any active senses and set the current sense interval as the time between the last 2 senses   
	sensorState = digitalRead(sensorPin);
	if (sensorState == HIGH) {
		activeSense = 1;
		currentSenseInterval = millis() - previousSense;
		Serial.println(String("S>") + (millis() - previousSense));
		previousSense = millis();
		//Hold until the read has completed so we don't read it a second time in the next loop
		while (sensorState == HIGH) {
			sensorState = digitalRead(sensorPin);
		}
		delay(9); //Had to put this in to stop double ups.  Seems the while loop breaks but will still enter the if above for some reason! 9ms is safe pause as we won't get revolutions faster than this.
	}

	//Complete any current pulses by making sure the electromagnet is now turned off if 20ms has elapsed since sendPulse() was called
	if (activePulse) {
		if (previousPulse <= (millis() - 100)) {
			digitalWrite(motorPin, LOW);
			activePulse = 0;
		}
	}

	//If we have an active sense to convert to a pulse then check if it is the right time fire it using the  
	if (activeSense) {
		if (currentSenseInterval >= minimumPulseInterval) {
			if (previousPulse <= (millis() - currentSenseInterval)) {
				sendPulse();
			}
		}
		else {
			if (previousPulse <= (millis() - minimumPulseInterval)) {
				sendPulse();
			}
		}
	}
}

void sendPulse() {
	if (!activePulse) {
		digitalWrite(motorPin, HIGH);
		Serial.println(String("P>") + (millis() - previousPulse));
		previousPulse = millis();
		activeSense = 0;
		activePulse = 1;
	}
}
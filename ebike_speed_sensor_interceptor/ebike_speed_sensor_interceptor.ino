/*
 Name:		ebike_speed_sensor_interceptor.ino
 Created:	5/24/2018 6:09:59 PM
 Author:	benja
*/

//Set global variables
unsigned long previousSense = 0;
unsigned long previousPulse = 0;
unsigned long currentSenseInterval = 100000;  //Initialise to a big number to be safe
int hallSensorPin = 2;
int electroMagnetPin = 3;
int hallSensorState = 0;
int minimumPulseInterval = 268; //30 Kmh on the speedo
int activeSense = 0;
int activePulse = 0;

void setup() {
	pinMode(hallSensorPin, INPUT);
	pinMode(electroMagnetPin, OUTPUT);
	Serial.begin(9600);  
}

void loop() {

	//IMPORTANT: We need to read every sense on the hall effect sensor so the code in loop() needs to be very lean
	//Read from the hall effect sensor for any active senses and set the current sense interval as the time between the last 2 senses   
	hallSensorState = digitalRead(hallSensorPin);
	if (hallSensorState == LOW) {
		activeSense = 1;
		currentSenseInterval = millis() - previousSense;
		previousSense = millis();
		//Hold until the read has completed so we don't read it a second time in the next loop
		while (hallSensorState == LOW) {
			hallSensorState = digitalRead(hallSensorPin);
		}
	}

	//Complete any current pulses by making sure the electromagnet is now turned off if 20ms has elapsed since sendPulse() was called
	if (activePulse) {
		if (previousPulse <= (millis() - 20)) {
			digitalWrite(electroMagnetPin, LOW);
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
	digitalWrite(electroMagnetPin, HIGH);
	Serial.println(millis()-previousPulse);
	previousPulse = millis();
	activeSense = 0;
	activePulse = 1;
}
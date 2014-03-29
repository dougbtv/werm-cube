#include <Wire.h>

const int ADDRESS_CONTROLLER1 = 3;
const int ADDRESS_CONTROLLER2 = 4;

void setup() {

	Serial.begin(9600);
	Wire.begin();

}

void loop() {

	Wire.requestFrom(ADDRESS_CONTROLLER1,1);
	while (Wire.available()) {
		byte controller1 = Wire.read();
		Serial.print("Controller 1: ");
		Serial.println(controller1);
	}

	Wire.requestFrom(ADDRESS_CONTROLLER2,1);
	while (Wire.available()) {
		byte controller2 = Wire.read();
		Serial.print("Controller 2: ");
		Serial.println(controller2);
	}

	Serial.println();


	delay(250);

}

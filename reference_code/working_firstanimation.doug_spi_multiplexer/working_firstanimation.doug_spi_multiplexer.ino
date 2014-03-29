// code for an 24 x 16 array (8 RGB LEDs, 16 columns) as an example
// not all code parts shown, just the highlights
// set up an array for the cathode drives

/* byte cathodeslower[ ] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20,  0x40, 0x80, 0,0,0,0,0,0,0,0};  // walking 1 for lower 8 NPNs 
byte cathodeshigher[ ] = {0,0,0,0,0,0,0,0,0x01, 0x02, 0x04, 0x08, 0x10, 0x20,  0x40, 0x80};  // walking 1 for upper 8 NPNs
byte anodes1 [16]; // array to hold 1st 3rd of RGB info
byte anodes2 [16]; // array to hold 2nd 3rd of RGB info
byte anodes3 [16]; // array to hold 3rd 3rd of RGB info
byte columncount;
byte oe = 2; */
#include <SPI.h>

const byte PIN_OUTPUTENABLE = A0;
const byte PIN_SLAVESELECT = 10;

const char PIN_GROUND[] = {2,3,4,5,6,7,8,9};

char current_layer = 0;
char last_layer = 7;


unsigned long updateTime=0;

bool once = false;

void setup(){

	Serial.begin(9600);

	// set the slaveSelectPin as an output:
	pinMode(PIN_SLAVESELECT, OUTPUT);


	pinMode(PIN_OUTPUTENABLE, OUTPUT);
	digitalWrite(PIN_OUTPUTENABLE, LOW);
	SPI.begin();

	Serial.println("did we do this?");

	// set your blinker...
 	pinMode(A1, OUTPUT);  

	// this is your ground/transistor array.
	for (int i = 0; i < 8; i++) {
		pinMode(PIN_GROUND[i], OUTPUT);
	}

	for (int i = 0; i < 8; i++) {
		digitalWrite(PIN_GROUND[i], LOW);
	}

   
	// Serial.println("are we getting killed...");
		byte test = 0x00;
		for (int i = 0; i < 8; i++) {
			SPI.transfer(test);
		}		
		// once = true;
		Serial.println("finished that.");
	

}


long next_update = 0;
long next_animation = 0;
const long interval = 1500;
const long animation_interval = 100000;

byte testing = 0x00;

void loop(){

	// delay(2000);
	// Serial.println("run marker...");

	if (!once) {
	}

/*	digitalWrite(A1, HIGH);   // set the LED on
	delay(500);              // wait for a second
	digitalWrite(A1, LOW);    // set the LED off
	delay(500);              // wait for a second */

	if (next_update <= micros()) {

		digitalWrite(PIN_OUTPUTENABLE, HIGH);

		// Turn off last layer.
		digitalWrite(PIN_GROUND[last_layer],LOW);

		// Turn on this layer.
		digitalWrite(PIN_GROUND[current_layer],HIGH);

		// Set that we're the last layer now.
		last_layer = current_layer;

		// Now increment the layer, or zip back to 0.
		current_layer++;
		if (current_layer >= 8) {
			current_layer = 0;
		}

		next_update = micros() + interval;

		digitalWrite(PIN_OUTPUTENABLE, LOW);


	}

	if (next_animation <= micros()) {

		testing++;
		for (int i = 0; i < 8; i++) {
			SPI.transfer(testing);
		}		

		next_animation += animation_interval;

		// once = true;
		// Serial.println("finished that.");
	

	}



	/*

	// set up for blink without delay operation
	if (millis() >=updateTime){
	// yes, set up for next update
	updateTime = updateTime +2; // update a column every 2 milliseconds, 16 columns at ~24 Hz rate
	digitalWrite (oe, HIGH);  // turn off display
	SPI.transfer (anodes1[columncount]); // update anodes
	SPI.transfer (anodes2[columncount]); // update anodes
	SPI.transfer (anodes3[columncount]); // update anodes
	SPI.transfer (cathodeslower[columncount]); // turn on a cathode
	SPI.transfer (cathodesupper[columncount]); // turn on a cathode
	digitalWrite (oe, LOW); / turn display back on
	columncount = columncount+1; // update for next column
	if (columncount == 16){columncount = 0;} // reset if reach the end
	} //end display refresh

	// now the fun part, for you!
	// read buttons, or whatever processing you plan to do for patterns, etc., 
	// and update anodes1, anodes2, anode3 as needed
	//

	*/
 
} 	// end void loop
 

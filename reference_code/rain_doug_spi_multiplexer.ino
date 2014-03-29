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

// -------------------------------- RAIN

int count = 0;

char rain[3][5];
	/* array (
		array(
			active,
			x,
			y,
			max_length,
			current_length,
		);
	); */

const int RAIN_ACTIVE = 0;
const int RAIN_X = 1;
const int RAIN_Y = 2;
const int RAIN_MAXLEN = 3;
const int RAIN_CURLEN = 4;

int currentdrop = 0;

char thincube[8][8];


// -------------------------------------- RAIN

void rainAnimation() {

	// set bit.
	// thincube[z][x] |= 1 << y;

	// So we have to cycle each raindrop, and nip it's top layer if it's hit it's maximum length.
	for (int i = 0; i < 3; i++) {
		// cout << "iteration: " << i << ": active: " << (int)rain[i][RAIN_ACTIVE] << endl;
		if (rain[i][RAIN_ACTIVE]) {
			rain[i][RAIN_CURLEN]++;
			if (rain[i][RAIN_CURLEN] > rain[i][RAIN_MAXLEN]) {
				// Now nips it's top layer back to zero.
				thincube[7][(int)rain[i][RAIN_X]] &= ~(1 << (int)rain[i][RAIN_Y]);
				// cout << (int)rain[i][RAIN_X] << " , " << (int)rain[i][RAIN_Y] << " <-------------" << endl;

				// printcube();
				rain[i][RAIN_ACTIVE] = 0;
				// cout << "!!!!!!!!!!!!!!!!!!!! nipped..." << endl;
			}
		}
	}

	// Now "rain the cube" by copying the layer from the one above.
	// namely, omitting the top layer.
	for (int z = 0; z < 7; z++) {
		for (int x = 0; x < 8; x++) {
			thincube[z][x] = thincube[z+1][x];
		}
	}

	// ------------ CREATE NEW RAINDROP.

	if (count == 0 || count % 15 == 0) {
	
		// Make a new drop if there isn't one set on this.
		if (!rain[currentdrop][RAIN_ACTIVE]) {	
			// cout << "current drop: " << currentdrop << endl;
			rain[currentdrop][RAIN_X] = (unsigned char)rand() % 7;
			rain[currentdrop][RAIN_Y] = (unsigned char)rand() % 7;
			rain[currentdrop][RAIN_MAXLEN] = 3; // rand() % 5;
			rain[currentdrop][RAIN_CURLEN] = 1;
			rain[currentdrop][RAIN_ACTIVE] = 1;
			// int y = rand() % 7;

			// cout << (int)rain[currentdrop][RAIN_X] << ", " << (int)rain[currentdrop][RAIN_Y] << endl;
			// cout << (int)rain[currentdrop][RAIN_MAXLEN] << " : maxlen" << endl;
			// cout << (int)rain[currentdrop][RAIN_CURLEN] << " : maxlen" << endl;

			thincube[7][(int)rain[currentdrop][RAIN_X]] |= 1 << (int)rain[currentdrop][RAIN_Y];


		}
		
		currentdrop++;
		if (currentdrop >= 3) {
			currentdrop = 0;
		}
	}


	// printcube();
	
	// cout << "cool" << endl;
	count++;

	// cout << " LOOP MARKER ------------------------------------ > " << count << endl;

	if (count > 10) { 
		exit(0);
	}

}

// --------------------------------------- END RAIN

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

		// send the data.
		for (int i = 0; i < 8; i++) {
			SPI.transfer(thincube[current_layer][i]);
		}		


		// Now increment the layer, or zip back to 0.
		current_layer++;
		if (current_layer >= 8) {
			current_layer = 0;
		}

		next_update = micros() + interval;

		digitalWrite(PIN_OUTPUTENABLE, LOW);


	}


	if (next_animation <= micros()) {

		rainAnimation();

		next_animation += animation_interval;

		/* testing++;
		for (int i = 0; i < 8; i++) {
			SPI.transfer(testing);
		}		

		next_animation += animation_interval; */

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
 

/* 
   this example from the NESpad Arduino library
   displays the buttons on the joystick as bits
   on the serial port - rahji@rahji.com
*/

#include <NESpad.h>
#include <Wire.h>


// put strobe/clock/data pin numbers here if necessary
// ie: NESpad nintendo = NESpad(4,5,6);
NESpad nintendo = NESpad(2,3,4); // default pins: 2,3,4

byte state = 0;
byte current_button = 0;
volatile long wait_after_send = 0; // Wait 2ms after sending a byte to send again.
// bool interrupted = FALSE;

const byte ADDRESS = 3;

const unsigned char BUTTON_UP = 	1;
const unsigned char BUTTON_LEFT = 	2;
const unsigned char BUTTON_DOWN = 	3;
const unsigned char BUTTON_RIGHT = 	4;
const unsigned char BUTTON_ZDOWN = 	5;
const unsigned char BUTTON_ZUP = 	6;
const unsigned char BUTTON_SELECT = 	7;
const unsigned char BUTTON_START =	8;

void setup() {
	Serial.begin(9600);  

	Wire.begin(ADDRESS);	
	Wire.onRequest(fillRequest);

}

void fillRequest() {

	// interrupted = TRUE;

	// Ok we got the request.
	// So let 'em know what button.
	Wire.write(current_button);

	
	// We're going to wait after a send
	// BUT only if we send a non-zero.
	if (current_button != 0) {
		wait_after_send = millis() + 2;
	}	

	// Now, we'll reset our current button.
	current_button = 0;


}

void loop() {
  
	state = nintendo.buttons();
 
	// shows the shifted bits from the joystick
	// buttons are high (1) when up 
	// and low (0) when pressed
	// Show padded zeros with tilde

	// Serial.println(~state, BIN);
	// delay(500);

	if (millis() > wait_after_send) {
	
		byte detected_button = determineButton(state);

		if (detected_button != 0) {

			// Only change when non-zero.
			// This way we'll latch the last pressed button.
			current_button = detected_button;

		}

		// And because I believe there's the possibility of an interrupt coming back here...
		// Fudge.

	} else {
		// In this case, we're still waiting after a send, so keep resetting button to 0.
		current_button = 0;
	}
}

byte determineButton(byte thestate) {

	// Cycle through each bit, and get the highest order.
	// Inverse logic, LOW = TRUE.

	// 00000000
	//  |||||BA
	//   |||Select
	//   ||Start
	//   |Up
	//   Down 
	//  Left
	// Right
	byte returns = 0;

	for (int i = 0; i < 8; i++) {
		if (bitRead(thestate,i)) {
			// Ok so we read the state, and we have the lowest order bit.
			switch (i) {
				case 0: returns = BUTTON_ZDOWN; break;
				case 1: returns = BUTTON_ZUP; 	break;
				case 2: returns = BUTTON_SELECT; break;
				case 3: returns = BUTTON_START; break;
				case 4: returns = BUTTON_UP; break;
				case 5: returns = BUTTON_DOWN; break;
				case 6: returns = BUTTON_LEFT; break;
				case 7: returns = BUTTON_RIGHT; break;				
			}

			// And slip out of the loop.
			break;
		}
	}	

	/*if (interrupted) {
		interrupted = FALSE;
		return 0;
	}*/

	return returns;

}

















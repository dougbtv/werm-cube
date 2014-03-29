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
#include <Wire.h>

PROGMEM const int ADDRESS_CONTROLLER1 = 3;
PROGMEM const int ADDRESS_CONTROLLER2 = 4;


PROGMEM const byte PIN_OUTPUTENABLE = A0;
PROGMEM const byte PIN_SLAVESELECT = 10;

PROGMEM const char PIN_GROUND[] = {2,3,4,5,6,7,8,9};

char current_layer = 0;
char last_layer = 7;


unsigned long updateTime=0;

bool once = false;

// -------------------------------- RAIN

int count = 0;

char rain[5][5];
	/* array (
		array(
			active,
			x,
			y,
			max_length,
			current_length,
		);
	); */

PROGMEM const int RAIN_ACTIVE = 0;
PROGMEM const int RAIN_X = 1;
PROGMEM const int RAIN_Y = 2;
PROGMEM const int RAIN_MAXLEN = 3;
PROGMEM const int RAIN_CURLEN = 4;

int currentdrop = 0;

char thincube[8][8];

// ------------------------------------- MODES

char mode = 2;

PROGMEM const char MODE_GAME = 1;
PROGMEM const char MODE_RAIN = 2;


// -------------------------------------- RAIN

bool flasherValue = false;

bool getBit(char number, int position) {
    unsigned char bitmask = 1 << position;
    return (number & bitmask) ? 1 : 0;
}


// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm


int cyclenum = 0;
int cyclemax = 125;

PROGMEM const int generateSegmentEvery = 10;


PROGMEM const unsigned char SIZE_Z = 8;
PROGMEM const unsigned char SIZE_X = 8;
PROGMEM const unsigned char SIZE_Y = 8;
PROGMEM const unsigned char SIZE_CUBE = 8;

PROGMEM const unsigned char BUTTON_UP  = 	1;
PROGMEM const unsigned char BUTTON_LEFT = 	2;
PROGMEM const unsigned char BUTTON_DOWN = 	3;
PROGMEM const unsigned char BUTTON_RIGHT = 	4;
PROGMEM const unsigned char BUTTON_ZDOWN = 	5;
PROGMEM const unsigned char BUTTON_ZUP = 	6;
PROGMEM const unsigned char BUTTON_SELECT = 	7;
PROGMEM const unsigned char BUTTON_START =	8;


// bool cube [SIZE_Z][SIZE_X][SIZE_Y];

PROGMEM const int SIZE_WORM = 16;


//                          z,x,y

class Worm {
	private:
		void commitVector(char,char,char);		// Actually commit setting the vector based on relativity, handled by setVector.
		

	public:
		signed char segment[SIZE_WORM][3]; 	// What cartesian coordinates make up this worm? 
		char length; 				// How long is the worm?
		signed char vector[3]; 			// = {0,0,-1};
		signed char relativity[3]; 		// What's the vector relative to the player?

		Worm() {}	
		Worm(char,char,char,char,char,char,char,char,char,char);
		bool containsPoint(char,char,char,bool);
		void move();
		void generateSegment();
		bool setVector(char);				// Calculate new vector by direction - handler for commitVector.

};

class Cube {

	public:
		Worm playerOne;
		Worm playerTwo;
		// bool coords [SIZE_Z][SIZE_X][SIZE_Y];
		char thincube[SIZE_Z][SIZE_X];


		void transfer(char);
		// void debug(signed char, Worm&, Worm&);
		void plotWorm(Worm&);
		void clear();
		// void renderThin();
		bool detectCollision(Worm&,Worm&,bool);
		void loop(char,char);

		// 
		void setPoint(bool,char,char,char);

		Cube() : playerOne(0,2,3,0,0,1,1,1,1,-1), playerTwo(0,5,4,0,0,-1,1,1,-1,1) {}

		// Cube(); 
		// ~Cube();	//  
			// playerTwo(0,5,4,0,0,-1,1,1,-1,1);
			
		
};

Cube cube;


void Cube::setPoint(bool value, char z, char x, char y) {

	// Don't set invalid points.
	// But essentially allow it to have things "go out of bounds"
	if (z < 0 || x < 0 || y < 0 || z > 7 || x > 7 || y > 7) {
		return;
	}

	// Then do your bit logic.
	if (value) {
		// Set it true.
		thincube[(int)z][(int)x] |= 1 << (int)y;
	} else {
		// Clear it.
		thincube[(int)z][(int)x] &= ~(1 << (int)y);
	}

}

void Cube::transfer(char zlayer) {

	// Serial.println(zlayer, DEC);

	for (int i = 0; i < 8; i++) {
		SPI.transfer(thincube[zlayer][i]);
	}

}

void Cube::loop(char p1_direction, char p2_direction) {
	cyclenum++;
	/*if (cyclenum >= cyclemax) {
		exit(0);
	}*/

	playerOne.setVector(p1_direction);
	playerTwo.setVector(p2_direction);
	
		

	if (cyclenum % generateSegmentEvery == 0) {
		playerOne.generateSegment();
		playerTwo.generateSegment();
	}
	
	playerOne.move();
	playerTwo.move();

	// "Render" the cube.
	// !bang
	clear();
	plotWorm(playerOne);
	plotWorm(playerTwo);
	
	// renderThin();
	// debug(0,playerOne,playerTwo);


	// Now that we moved, let's detect a collision.
	// Well check one against itself, and the other player.
	// And the other player against itself, and the first player.
	bool pone_collide = 		detectCollision(playerOne,playerOne,true);
	bool pone_collide_other =  	detectCollision(playerOne,playerTwo,false);
	bool ptwo_collide = 		detectCollision(playerTwo,playerTwo,true);
	bool ptwo_collide_other =  	detectCollision(playerTwo,playerOne,false);

	// Now let's condense these variables into one for each condition.
	if (pone_collide_other) {
		pone_collide = true;
	} 

	if (ptwo_collide_other) {
		ptwo_collide = true;
	}

	// Now that we know who has collided, we can determine win, loss or tie.
	if (pone_collide) {
		if (ptwo_collide) {
			// Tie.
			// // cout << "It's a tie!" << endl;
			// exit(0);
		} else {
			// Player two wins.
			// // cout << "Player two wins." << endl;
			// `exit(0);
		}
	} else {
		if (ptwo_collide) {
			// Player one wins.
			// // cout << "Player one wins." << endl;
			// exit(0);
		} else {
			// Game continues.
			// ...
		}
	}

	//if (pone_collision == true) {
	//	// cout << "PLAYER ONE COLLIDES!" << endl;
	//}


}


// Tell this:
// First, what worm head am I checking?
// Then, what worm am I looking for a collision with.
// Last, is that worm, the same as the first worm??
bool Cube::detectCollision(Worm& wormhead,Worm& wormtarget,bool within_self = true) {

	bool collision = false;

	// So take the head of this thing, which we just forced a move on.
	// And let's see if it's contained in a worm.

	collision = wormtarget.containsPoint(wormhead.segment[0][0],wormhead.segment[0][1],wormhead.segment[0][2],within_self);
	return collision;	

}


/*
void Cube::renderThin() {

	// Cycle the boolean cube and turn it into the thin cube.
	// char loaf = 127;
	// With the |= bitwise or operator... the format is:
	// byte |= value_to_set << position
	thincube[0][0] |= 1 << 0;
	// printByteBinary(thincube[0][0]);

	char temp = 0;
	char newval = 0;
	for (unsigned int z = 0; z < SIZE_Z; z++) {
		for (unsigned int x = 0; x < SIZE_X; x++) {
			temp = 0;
			for (unsigned int y = 0; y < SIZE_Y; y++) {
				newval = 0;
				if (coords[z][x][y]) {
					newval = 1;
				}
				temp |= newval << y;
									
			}
			thincube[z][x] = temp;		
		}
	}

	for (unsigned int z = 0; z < SIZE_Z; z++) {
		// // cout << "Layer " << z << endl;
		for (unsigned int x = 0; x < SIZE_X; x++) {
			// printByteBinary(thincube[z][x]);
		}
		// // cout << endl;

	}



}
*/



// If you don't want a specific layer, send this a -1 for the specific layer.
/*
void Cube::debug(signed char specificlayer, Worm& wormone, Worm& wormtwo) {

	// char what[] = "what?";
	// printIt(what);
	// printInt(sizeof(cube));

	char startz = 0;
	char endz = SIZE_Z;
	if (specificlayer > -1) {
		startz = specificlayer;
		endz = specificlayer+1;
	}

	// // cout << endl << "------------------------- Cycle " << cyclenum << endl;

	for (int z = startz; z < endz; z++) {
		// cout << "Layer (z-coord): " << z << endl;
		// cout << endl;
		for (unsigned int y = 0; y < SIZE_Y; y++) {
			if (y==0) {
				// cout << "    0 1 2 3 4 5 6 7" << endl;
				// cout << "  +----------------" << endl;
			}
			
			// cout << y << " | "; 

			char mark;
			for (unsigned int x = 0; x < SIZE_X; x++) {
				mark = 'O';
				if (coords[z][x][y]) { mark = 'X'; }
				// Determine if it's in worm 2...
				if (wormtwo.containsPoint(z,x,y,false)) { mark = 'Y'; }
				// cout << mark << " ";
			}

			// cout << endl;
			
		}
	}

}

*/


/*
Cube::Cube() {

	// cout << "Cube constructor\n";

	playerOne(0,2,3,0,0,1,1,1,1,-1); 
	playerTwo(0,5,4,0,0,-1,1,1,-1,1);
	

	// Cube cube = Cube();

}*/



void Cube::plotWorm(Worm& worm) {

	// Lay down the worm(s) onto the cube.
	for (int i = 0; i < worm.length; i++) {
		setPoint(true,worm.segment[i][0],worm.segment[i][1],worm.segment[i][2]);
	}

}


void Cube::clear() {

	for (int z = 0; z < 8; z++) {
		for (int x = 0; x < 8; x++) {
			thincube[z][x] = 0;
		}
	}
}



Worm::Worm (char z,char x,char y,char vz,char vx, char vy,char initlen,char rel_z, char rel_x, char rel_y) {
	// // cout << "Init worm! " << z << "," << x << "," << y << endl;
	// Set the first segment.
	segment[0][0] = z;
	segment[0][1] = x;
	segment[0][2] = y;

	// Set the relativity
	relativity[0] = rel_z; // 1; // Same for all players, going "up" is positive.
	relativity[1] = rel_x; // 1; // X going "right" is positive.
	relativity[2] = rel_y; // -1; // Y going "up" is negative.

	// Set the initial vector.
	vector[0] = vz;
	vector[1] = vx;
	vector[2] = vy;

	// Set the length.
	length = initlen;
	
	// // cout << (int)segment[0][0] << ",";
	// // cout << (int)segment[0][1] << ",";
	// // cout << (int)segment[0][2] << "------------------------ WHAT" << endl;
	// // cout << segment << "what?" << endl;
}

bool Worm::containsPoint(char z, char x, char y, bool myself) {

	bool contains = false;
	int start_i = 0;

	// In the case we're checking against ourself, we don't wanna check our own head.
	if (myself) {
		start_i++;
	}

	for (int i = start_i; i < length; i++) {
		if (segment[i][0] == z && segment[i][1] == x && segment[i][2] == y) {
			// // cout << "----------- COLLISION! --------------------" << endl;
			contains = true;
			break;
		}
	}

	return contains;

}

void Worm::generateSegment() {

	// Ok, we're ready to generate a new segment.
	// Should be as easy as adding to the length.
	if (length < (SIZE_WORM-1)) {
		length++;
	}

}

void Worm::move() {
	// Ok go ahead and move that worm along.
	// // cout << "-=-=-=-=-= " << sizeof(worm) << endl;

	// Move it's segments down to the ends.
	for (int i = (SIZE_WORM-1); i > 0; i--) {
		segment[i][0] = segment[i-1][0];
		segment[i][1] = segment[i-1][1];
		segment[i][2] = segment[i-1][2];
	}

	// Give it a new head, based on it's vector.
	segment[0][0] += vector[0];
	segment[0][1] += vector[1];
	segment[0][2] += vector[2];
	
	// But! You gotta flip it back to the 0th position or the Max-th position if it goes outside the bounds.
	for (int i = 0; i < 3; i++) { 		// We'll magic-number the 3. It's a 3d cube, I'm hard coding that.
		if (segment[0][i] >= SIZE_CUBE) {
			segment[0][i] = 0;
		}

		if (segment[0][i] < 0) {
			segment[0][i] = SIZE_CUBE-1;
		}

	}
	

}



// ---------------------- setVector
// --- Takes a direction as input 
// --- and computes new vector.
// --- Returns false if unchanged.
// --- True if vector is changed.

void Worm::commitVector(char z, char x, char y) {
	// If the current vector is in the same axis...
	// We can't do that, so just return without a move
	if (z != 0 && vector[0] != 0) { return; }
	if (x != 0 && vector[1] != 0) { return; }
	if (y != 0 && vector[2] != 0) { return; }

	vector[0] = z * relativity[0];	
	vector[1] = x * relativity[1];	
	vector[2] = y * relativity[2];	
}

bool Worm::setVector(char indirection) {

	// If it's a null direction, we'll return false [e.g. nothing changed]
	if (indirection == 0) {
		return false;
	}

	//. signed char newdir = 0;

	switch (indirection) {
		case BUTTON_UP:
			commitVector(0,0,1);
			break;
		case BUTTON_DOWN:
			commitVector(0,0,-1);
			break;
		case BUTTON_LEFT:
			commitVector(0,-1,0);
			break;
		case BUTTON_RIGHT:
			commitVector(0,1,0);
			break;
		case BUTTON_ZUP:
			commitVector(1,0,0);
			break;
		case BUTTON_ZDOWN:
			commitVector(-1,0,0);
			break;
	}


	return true;

}

/*
void printIt(char msg[]) {
	// cout << msg << endl;
}

void printInt(int inbound) {
	printf("%02d",inbound);
}
*/



char getInput(bool playerone = true) {

	char in;

	if (playerone) {

		Wire.requestFrom(ADDRESS_CONTROLLER1,1);
		while (Wire.available()) {
			in = Wire.read();
			//	Serial.print("Controller 1: ");
			//	Serial.println(controller1);
		}	

	} else {

		Wire.requestFrom(ADDRESS_CONTROLLER2,1);
		while (Wire.available()) {
			in = Wire.read();
			//	Serial.print("Controller 2: ");
			//	Serial.println(controller2);
		}

	}

	// cout << "[Player " << player_text << "] Direction:";
	// cin >> in;
	
	return in;


}
   


// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm


void printcube() {

	for (int z = 0; z < 8; z++) {
		// cout << "Z Layer: " << z << " ------------------ " << endl;
		Serial.print("-------------------------------- Z layer: ");
		Serial.println(z);
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				//setCube(thincube,true,z,x,y);

				bool mybit = getBit(cube.thincube[z][x],y);
				if (mybit) {
					Serial.print("1");
				} else {
					Serial.print("0");
				}
			}
			Serial.println();
			// cout << endl;
		}
		Serial.println();
		// cout << endl;
	}

	

}


void rainAnimation() {

	// Serial.println("check.");

/*	flasherValue = !flasherValue;
	digitalWrite(A1,flasherValue); */

	// set bit.
	// thincube[z][x] |= 1 << y;

	// So we have to cycle each raindrop, and nip it's top layer if it's hit it's maximum length.
	for (int i = 0; i < 5; i++) {
		// cout << "iteration: " << i << ": active: " << (int)rain[i][RAIN_ACTIVE] << endl;
		if (rain[i][RAIN_ACTIVE]) {
			rain[i][RAIN_CURLEN]++;
			if (rain[i][RAIN_CURLEN] > rain[i][RAIN_MAXLEN]) {
				// Now nips it's top layer back to zero.
				//cube.setPoint(false,7,rain[i][RAIN_X],rain[i][RAIN_Y]);				
				cube.thincube[7][(int)rain[i][RAIN_X]] &= ~(1 << (int)rain[i][RAIN_Y]);
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
			cube.thincube[z][x] = cube.thincube[z+1][x];
		}
	}

	// ------------ CREATE NEW RAINDROP.

	if (count == 0 || count % 4 == 0) {
	
		// Make a new drop if there isn't one set on this.
		if (!rain[currentdrop][RAIN_ACTIVE]) {	
			// cout << "current drop: " << currentdrop << endl;
			rain[currentdrop][RAIN_X] = (unsigned char)rand() % 7;
			rain[currentdrop][RAIN_Y] = (unsigned char)rand() % 7;
			rain[currentdrop][RAIN_MAXLEN] = (rand() % 5) + 2;
			rain[currentdrop][RAIN_CURLEN] = 1;
			rain[currentdrop][RAIN_ACTIVE] = 1;
			// int y = rand() % 7;

			// cout << (int)rain[currentdrop][RAIN_X] << ", " << (int)rain[currentdrop][RAIN_Y] << endl;
			// cout << (int)rain[currentdrop][RAIN_MAXLEN] << " : maxlen" << endl;
			// cout << (int)rain[currentdrop][RAIN_CURLEN] << " : maxlen" << endl;
			
			// cube.setPoint(true,7,rain[currentdrop][RAIN_X],rain[currentdrop][RAIN_Y]);				
			cube.thincube[7][(int)rain[currentdrop][RAIN_X]] |= 1 << (int)rain[currentdrop][RAIN_Y];


		}
		
		currentdrop++;
		if (currentdrop >= 5) {
			currentdrop = 0;
		}
	}


	
	// cout << "cool" << endl;
	count++;

	// cout << " LOOP MARKER ------------------------------------ > " << count << endl;

	if (count > 10) { 
		printcube();
		Serial.println();		
		delay(2000);
		exit(0);
	}
}

// --------------------------------------- END RAIN

void setup(){

	Serial.begin(9600);

	Wire.begin();


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
// looked good for rain
// const long animation_interval = 50000;
long animation_interval = 90000;

byte testing = 0xbb;

void loop(){

	

	// delay(2000);
	// Serial.println("run marker...");

	if (!once) {
	}

/*	digitalWrite(A1, HIGH);   // set the LED on
	delay(500);              // wait for a second
	digitalWrite(A1, LOW);    // set the LED off
	delay(500);              // wait for a second */

	if (next_animation <= micros()) {

		char p1_direction;// = getInput();
		char p2_direction; // = getInput(false);

		switch (mode) {
			case MODE_GAME:
				p1_direction = getInput();
				p2_direction = getInput(false);
				cube.loop(p1_direction,p2_direction);
				break;
			
			case MODE_RAIN:
				rainAnimation();
				// animation_interval = 250000;
				break;
		}

		

		next_animation += animation_interval;

		/* testing++;
		for (int i = 0; i < 8; i++) {
			SPI.transfer(testing);
		}		

		next_animation += animation_interval; */

		// once = true;
		// Serial.println("finished that.");
	

	}

	if (next_update <= micros()) {

		digitalWrite(PIN_OUTPUTENABLE, HIGH);

		// Turn off last layer.
		digitalWrite(PIN_GROUND[last_layer],LOW);

		// Re-enable.
		digitalWrite(PIN_OUTPUTENABLE, LOW);

		// send the data.
		// cube.transfer(current_layer);

		for (int v = 0; v < 8; v++) {
			SPI.transfer(cube.thincube[current_layer][v]);
		}
		
		// Turn on this layer.
		digitalWrite(PIN_GROUND[current_layer],HIGH);

		// Set that we're the last layer now.
		last_layer = current_layer;

		// Now increment the layer, or zip back to 0.
		current_layer++;
		if (current_layer >= 8) {
			current_layer = 0;
		}

		// When's our next update?
		next_update = micros() + interval;



	}

 
} 

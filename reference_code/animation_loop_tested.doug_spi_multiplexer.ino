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

// ---------------------- debug constants
const bool DEBUG = false;
// ----------------------

const int ADDRESS_CONTROLLER1 = 3;
const int ADDRESS_CONTROLLER2 = 4;


const byte PIN_OUTPUTENABLE = A0;
const byte PIN_SLAVESELECT = 10;

const char PIN_GROUND[] = {2,3,4,5,6,7,8,9};

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
// !mode
char mode = 3;
char mode_animation = 1;

const char MODE_WAIT = 0;
const char MODE_GAME = 1;
const char MODE_DEMO = 2;
const char MODE_ANIMATION = 3;
const char MODE_DEATHSEQUENCE = 4;

const char MODE_PLAYER1_ACK = 20;
const char MODE_PLAYER2_ACK = 21;

const char MODE_DEBUG = 100;

// ---------- animation modes

const char MODE_ANIMATION_FUNNELGROW = 	1;
const char MODE_ANIMATION_RAIN = 	2;
const char MODE_ANIMATION_GROWCUBE = 	3;
const char MODE_ANIMATION_FUNNEL = 	4;
const char MODE_ANIMATION_STARFIELD = 	5;
const char MODE_ANIMATION_FUNNYTUBE = 	6;

const char MODE_ANIMATION_MAX = 6;

// -- winners

// -- winner variables
bool winner_playerone = false;
bool winner_tie = false;
bool gameover = false;

// -------------------------------------- RAIN

bool flasherValue = false;



// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm
// ------------------------------------------------------------------------------ Werm


int cyclenum = 0;
int cyclemax = 125;

int generateSegmentEvery = 5;


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

// ------- animation constants

const char GROWCUBE_BASECOORD = 3;
const char GROWCUBE_BASESIZE = 2;
const char FUNNEL_MAX = 3;

unsigned char growcube_currentsize = GROWCUBE_BASESIZE;
unsigned char growcube_offset = 0;

signed char growcube_growdirection = -1;
char funnel_next[2] = {0,0};
char funnel_current[2] = {0,0};
char funnel_size = 1;
signed char funnel_direction = 1;

// ----------------------------

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
		void loseSegment();
		bool setVector(char);				// Calculate new vector by direction - handler for commitVector.

};

class Cube {

	public:
		Worm playerOne;
		Worm playerTwo;
		bool coords [SIZE_Z][SIZE_X][SIZE_Y];
		char thincube[SIZE_Z][SIZE_X];


		// -- animation specif
		

		

		// end animation



		void setPoint(bool,char,char,char);

		void rainAnimation();
		void transfer();
		void debug(signed char, Worm&, Worm&);
		void plotWorm(Worm&);
		void clear();
		void renderThin();
		bool detectCollision(Worm&,Worm&,bool);
		void loop(char,char);
		void deathseq();
		bool getBit(char, char, char);
		void drawPlaneLine(char , char , char , char , char );
		void drawRectOnPlane(char , char , char , char , char );
		void drawZLine(char , char , char , char );
		void drawVolumeCube(char , char , char , char );

void growCubeVariables();
void growCubeHandler();
void funnyTubeHandler();
void funnelHandler(bool);
void starfieldHandler();


			// Worm::Worm (char z,char x,char y,char vz,char vx, char vy,char initlen,char rel_z, char rel_x, char rel_y) {
		Cube() : playerOne(4,3,8,0,0,-1,1,1,1,-1), playerTwo(5,4,0,0,0,1,1,1,-1,1) {}

		// Cube(); 
		// ~Cube();	//  
			// playerTwo(0,5,4,0,0,-1,1,1,-1,1);
			
		
};

Cube* cube;

// ----------------------------------------- begin animations


void Cube::growCubeVariables() {

	// Grow the cube or shrink it down.
	if (growcube_currentsize == 8 || growcube_currentsize == GROWCUBE_BASESIZE) {
		growcube_growdirection *= -1;
	}
	
	growcube_currentsize += (growcube_growdirection * 2);
	growcube_offset += growcube_growdirection;

}

void Cube::growCubeHandler() {

	// Clear the cube before you plot what you want.	
	clear();

	drawVolumeCube(growcube_currentsize,(GROWCUBE_BASECOORD - growcube_offset),(GROWCUBE_BASECOORD - growcube_offset),(GROWCUBE_BASECOORD - growcube_offset));

	growCubeVariables();
	

	// drawRectOnPlane(4,0,0,5,5);
	
	
}


void Cube::funnyTubeHandler() {


	// Rain the tube downwards.
	for (int z = 0; z < 7; z++) {
		for (int x = 0; x < 8; x++) {
			thincube[z][x] = thincube[z+1][x];
		}
	}

	// Now, set a new top.
	// So let's nip that top back to zeros.
	for (int i = 0; i < 8; i++) {
		thincube[7][i] = 0;
	}

	// Now, we can draw our next rectangle there.
	drawRectOnPlane(
		7, 
		(GROWCUBE_BASECOORD - growcube_offset), 
		(GROWCUBE_BASECOORD - growcube_offset), 
		(GROWCUBE_BASECOORD - growcube_offset) + (growcube_currentsize-1), 
		(GROWCUBE_BASECOORD - growcube_offset) + (growcube_currentsize-1)
	);

	// And handle how the size grows / shrinks
	growCubeVariables();
	
}


void Cube::funnelHandler(bool do_grow = false) {

	// Rain the funnel downwards.
	for (int z = 0; z < 7; z++) {
		for (int x = 0; x < 8; x++) {
			thincube[z][x] = thincube[z+1][x];
		}
	}

	// So let's nip that top back to zeros.
	for (int i = 0; i < 8; i++) {
		thincube[7][i] = 0;
	}

	// Let's plot our rectangle.
	if (funnel_size == 1) {
		// We're just plotting a point.
		setPoint(true,7,funnel_current[0],funnel_current[1]);
	} else {
		// We're drawing a rectangle.
		drawRectOnPlane(7,funnel_current[0],funnel_current[1],funnel_current[0]+(funnel_size-1),funnel_current[1]+(funnel_size-1));	
	}
	

	// If the two points are the same, we need a next point.
	if (funnel_current[0] == funnel_next[0] && funnel_current[0] == funnel_next[0]) {
		funnel_next[0] = (rand() % 8);
		funnel_next[1] = (rand() % 8);
	}

	// cout << "Current: " << (int)funnel_current[0] << ", " << (int)funnel_current[1] << endl;
	// cout << "Next:    " << (int)funnel_next[0] << ", " << (int)funnel_next[1] << endl;

	// So, where can we move?
	signed char xmove = 0;
	signed char ymove = 0;	

	if (funnel_current[0] > funnel_next[0]) {
		// We can move down x.
		xmove = -1;
	}

	if (funnel_current[0] < funnel_next[0]) {
		// We can move up x.
		xmove = 1;
	}

	if (funnel_current[1] > funnel_next[1]) {
		// We can move down y.
		ymove = -1;
	}

	if (funnel_current[1] < funnel_next[1]) {
		// We can move up y.
		ymove = 1;
	}

	// Move where we can.
	funnel_current[0] += xmove;
	funnel_current[1] += ymove;

	// If we're in the grow animation, we'll handle that.
	if (do_grow) {
		// So we wanna grow this rectangle.
		// How big can we get with our NEW coordinates?
		char max = 0;
		char xmax = (7 - funnel_current[0]) + 1; 
		char ymax = (7 - funnel_current[1]) + 1;
	
		// We want the lesser of these two.
		if (xmax < ymax) {
			max = xmax;		
		} else {
			max = ymax;
		}	

		// We don't want a max greater than our total constant max.
		if (max > FUNNEL_MAX) {
			max = FUNNEL_MAX;
		}

		// So if we apply our direction to our size: Is it legal against the max?
		if ((funnel_size + funnel_direction) <= max) {

			// Ok it's less than the max.
			// But is it less than zero?
			if ((funnel_size + funnel_direction) <= 0) {
				// We're less than zero. We need to change directions.
				funnel_direction *= -1;
			} else {
				// We're below the max, but, we're greater than zero. We can apply this.
			}			

		} else {

			// We're over the max.
			funnel_direction *= -1;

		}

		funnel_size += funnel_direction;
			
		

	}

	// cout << "Funnel size: " << (int)funnel_size << endl;


	

}

void Cube::starfieldHandler() {

	// Alright so this we don't want to rain from the top, but, go side-to-side.
	// We'll conveniently align this with bytes.
	// Rain the funnel sideways.
	for (int x = 0; x < 7; x++) {
		for (int z = 0; z < 8; z++) {
			thincube[z][x] = thincube[z][x+1];
		}
	}

	// Wipe that plane clean.
	for (int j = 0; j < 8; j++) {
		thincube[j][7] = 0;
	}

	// Now let's create a random few stars.
	char numstars = (unsigned char)rand() % 5;

	// cout << "Number stars: " << (int)numstars << endl;

	for (char e = 0; e < numstars; e++) {
		// Now, given the number of stars, create new points.
		// !bang
		setPoint(true,(rand() % 8),7,(rand() % 8));
	}


}

// ------------------------------------------------------------------------ begin geometry


void Cube::drawPlaneLine(char zlayer, char startx, char starty, char endx, char endy) {

	if (starty == endy) {
		// It's a line across x.
		// Walk x values, and set them.
		for (int i = startx; i <= endx; i++) {
			setPoint(true,zlayer,i,starty);
		}
	} else if (startx == endx) {
		// It's a line down y.
		for (int i = starty; i <= endy; i++) {
			setPoint(true,zlayer,startx,i);
		}
	}

}

void Cube::drawRectOnPlane(char zlayer, char startx, char starty, char endx, char endy) {

	// Draw four lines on this.
	// Let's start with the x lines.

	// Horizontal
	drawPlaneLine(zlayer, startx, starty, endx, starty);
	drawPlaneLine(zlayer, startx, endy, endx, endy);
	// Vertical
	drawPlaneLine(zlayer, startx, starty, startx, endy);
	drawPlaneLine(zlayer, endx, starty, endx, endy);


}

void Cube::drawZLine(char startz, char endz, char x, char y) {

	// Traverse up to end z.
	for (int i = startz; i <= endz; i++) {
		setPoint(true,i,x,y);
	}

}

// Create a true cube with a height/width/depth of size
// Starting at it's lowest coordinate, and building from there.


void Cube::drawVolumeCube(char size, char startz, char startx, char starty) {

	// We need to account for the 0th position.
	size--;

	// So first we need it's top and bottom rectangles.
	// Bottom first.
	drawRectOnPlane(startz,startx,starty,(startx+size),(startz+size));
	// Now the top.
	drawRectOnPlane((startz+size),startx,starty,(startx+size),(startz+size));

	// Then we'll add the "legs" which are the z-lines at each corner.
	drawZLine(startz,(startz+size),startx,starty);
	drawZLine(startz,(startz+size),(startx+size),starty);
	drawZLine(startz,(startz+size),startx,(starty+size));
	drawZLine(startz,(startz+size),(startx+size),(starty+size));


}

// ----------------------------------------------------------------------- geometry
// ----------------------------------------------------------------------- geometry
// ----------------------------------------------------------------------- geometry
// ----------------------------------------------------------------------- geometry

void Cube::deathseq() {

	

	if (playerOne.length == 0 || playerTwo.length == 0) {
		// done.
		// !bang
		gameover = true;
		if (winner_tie) {
			// ...
			cube->clear();
			mode = MODE_ANIMATION;
		} else {
			if (winner_playerone) {
				mode = MODE_PLAYER1_ACK;
			} else {
				mode = MODE_PLAYER2_ACK;
			}
		}

	} else {

		Serial.println("death bapper!");
		Serial.println(playerOne.length,DEC);
		Serial.println(playerTwo.length,DEC);

		clear();

		// Go and shrink down the losing worm, then... flash a winner's side.
		if (winner_tie) {
			// We're just gonna do a special flash for this, hard to test.
			playerOne.loseSegment();
			playerTwo.loseSegment();
		} else {
			if (winner_playerone) {
				playerOne.loseSegment();
			} else {
				playerTwo.loseSegment();
			}
		}

		plotWorm(playerOne);
		plotWorm(playerTwo);

		renderThin();

	}

}

bool Cube::getBit(char z, char x, char position) {

    char number = thincube[z][x];

    unsigned char bitmask = 1 << position;
    return (number & bitmask) ? 1 : 0;
}


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


void Cube::rainAnimation() {

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

	if (count == 0 || count % 4 == 0) {
	
		// Make a new drop if there isn't one set on this.
		if (!rain[currentdrop][RAIN_ACTIVE]) {	
			// cout << "current drop: " << currentdrop << endl;
			rain[currentdrop][RAIN_X] = (unsigned char)(rand() % 8);
			rain[currentdrop][RAIN_Y] = (unsigned char)(rand() % 8);
			rain[currentdrop][RAIN_MAXLEN] = (rand() % 5) + 2;
			rain[currentdrop][RAIN_CURLEN] = 1;
			rain[currentdrop][RAIN_ACTIVE] = 1;
			// int y = rand() % 7;

			// cout << (int)rain[currentdrop][RAIN_X] << ", " << (int)rain[currentdrop][RAIN_Y] << endl;
			// cout << (int)rain[currentdrop][RAIN_MAXLEN] << " : maxlen" << endl;
			// cout << (int)rain[currentdrop][RAIN_CURLEN] << " : maxlen" << endl;

			thincube[7][(int)rain[currentdrop][RAIN_X]] |= 1 << (int)rain[currentdrop][RAIN_Y];


		}
		
		currentdrop++;
		if (currentdrop >= 5) {
			currentdrop = 0;
		}
	}


//	printcube();
	
	// cout << "cool" << endl;
	count++;

	// cout << " LOOP MARKER ------------------------------------ > " << count << endl;

	/* if (count > 10) { 
		exit(0);
	} */


}

void Cube::transfer() {

	for (int i = 0; i < 8; i++) {
		SPI.transfer(thincube[current_layer][i]);
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

	// "Render" the cube->
	clear();
	plotWorm(playerOne);

	if (mode != MODE_DEMO) {
		plotWorm(playerTwo);
	}

	renderThin();
	// debug(0,playerOne,playerTwo);


	// Now that we moved, let's detect a collision.
	// Well check one against itself, and the other player.
	// And the other player against itself, and the first player.
	if (mode != MODE_DEMO) {

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
				mode = MODE_DEATHSEQUENCE;
				winner_tie = true;
				winner_playerone = false;

			} else {
				// Player two wins.
				// // cout << "Player two wins." << endl;
				// `exit(0);
				mode = MODE_DEATHSEQUENCE;
				winner_tie = false;
				winner_playerone = false;

			}
		} else {
			if (ptwo_collide) {
				// Player one wins.
				// // cout << "Player one wins." << endl;
				// exit(0);
				mode = MODE_DEATHSEQUENCE;
				winner_tie = false;
				winner_playerone = true;

			} else {
				// Game continues.
				// ...
			}
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



void Cube::renderThin() {

	// Cycle the boolean cube and turn it into the thin cube->
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




// If you don't want a specific layer, send this a -1 for the specific layer.
void Cube::debug(signed char specificlayer, Worm& wormone, Worm& wormtwo) {


	/* char what[] = "what?";
	printIt(what);
	printInt(sizeof(cube)); */

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
/*
Cube::Cube() {

	// cout << "Cube constructor\n";

	playerOne(0,2,3,0,0,1,1,1,1,-1); 
	playerTwo(0,5,4,0,0,-1,1,1,-1,1);
	

	// Cube cube = Cube();

}*/



void Cube::plotWorm(Worm& worm) {

	// Lay down the worm(s) onto the cube->
	for (int i = 0; i < worm.length; i++) {
		coords[(int)worm.segment[i][0]][(int)worm.segment[i][1]][(int)worm.segment[i][2]] = true;
	}

}


void Cube::clear() {
	for (unsigned int z = 0; z < SIZE_Z; z++) {
		for (unsigned int x = 0; x < SIZE_X; x++) {
			for (unsigned int y = 0; y < SIZE_Y; y++) {
				coords[z][x][y] = false;
			}
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			thincube[i][j] = 0;
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

void Worm::loseSegment() {

	// Ok, we're ready to generate a new segment.
	// Should be as easy as adding to the length.
	if (length > 0) {
		length--;
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


void printIt(char msg[]) {
	// cout << msg << endl;
}

void printInt(int inbound) {
	printf("%02d",inbound);
}



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


	if (DEBUG) {
		if (in == BUTTON_START) {
			Serial.println("---------------- INTENTIONAL DEATH");
			printcube();		
			delay(2000);
			exit(0);
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

				bool mybit = cube->getBit(z,x,y);
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

// --------------------------------------- END RAIN

void setup(){

	cube = new Cube();

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

// -------------------------------------- These are our MULTIPLEXING variables.
// -------------------------------------- They are used inside the loop to multiplex the image.

long next_update = 0;
const long interval = 1500;

// ---------------------------------------- Here we have some of the animation invervals / timers.

// looked good for rain
// const long animation_interval = 50000;
long next_animation = 0;
long animation_interval = 500000;

byte testing = 0xbb;

char debugz = 7;
char debugx = 0;
char debugy = 0;

// ------------- These variables are for "Player Acknowledgement", e.g. to press start to enter a game.

char playerack_position = 0;
char playerack_side = 7;	
char playerack_count = 0;
char playerack_p1taps = 0;

// --- newgame countdown
bool countdown_on = true;
long countdown_millis = 10000;
signed char countdown_ticker = 5;




void rebootTheCube() {
	delete cube;			
	cube = new Cube();
	cube->clear();

	// reset player acks.
	playerack_position = 0;
	playerack_side = 7;	
	playerack_count = 0;
	playerack_p1taps = 0;

	countdown_on = true;
	countdown_millis = 10000;
	countdown_ticker = 5;

	winner_playerone = false;
	winner_tie = false;

	gameover = false;
	
	growcube_currentsize = GROWCUBE_BASESIZE;
	growcube_offset = 0;

	growcube_growdirection = -1;
	funnel_next[0] = 0;
	funnel_next[1] = 0;
	funnel_current[0] = 0;
	funnel_current[1] = 0;
	funnel_size = 1;
	funnel_direction = 1;
			
}

void waitAcknowledge(bool p1) {
	if (p1) {
		playerack_p1taps++;
		mode = MODE_PLAYER1_ACK;		
	} else {
		mode = MODE_PLAYER2_ACK;		
	}

	playerack_count++;
}

void handleStart(bool p1) {

	switch (mode) {

		case MODE_WAIT:
			waitAcknowledge(p1);
			break;

		case MODE_DEMO:
			rebootTheCube(); 
			mode = MODE_ANIMATION;
			break;

		case MODE_ANIMATION:
			rebootTheCube(); 
			waitAcknowledge(p1);
			break;
		default: 
			break;
	}

}

long NEXT_ANIMATION_MILLIS = 15000;
long next_animation_flipper = 0;
bool next_animation_once = false;
long select_button_debounce = 0;

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

		// Re-enable.
		digitalWrite(PIN_OUTPUTENABLE, LOW);

		// send the data.
		cube->transfer();
		
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


	
	if (next_animation <= micros()) {

		char p1_direction;// = getInput();
		char p2_direction; // = getInput(false);

		p1_direction = getInput();
		p2_direction = getInput(false);

		// HANDLE START BUTTONS.
		if (p1_direction == BUTTON_START) {
			handleStart(true);
		}

		if (p2_direction == BUTTON_START) {
			handleStart(false);
		}

		if (p1_direction == BUTTON_SELECT || p2_direction == BUTTON_SELECT || millis() > next_animation_flipper) {
			
			if (millis() > select_button_debounce) {			

				if (p1_direction == BUTTON_SELECT || p2_direction == BUTTON_SELECT) {
					NEXT_ANIMATION_MILLIS = 45000;
					select_button_debounce = millis() + 1000; // wait for a second after select pressed.
				} else {
					NEXT_ANIMATION_MILLIS = 15000;
				}

				if (!next_animation_once) {
					if (mode == MODE_ANIMATION) {
						mode_animation++;
						rebootTheCube();
						if (mode_animation > MODE_ANIMATION_MAX	) {
							mode_animation = 1;
						}
						next_animation_flipper = millis() + NEXT_ANIMATION_MILLIS;	
					}
				} else {
					next_animation_once = true;
				
				}

			}
		}


		switch (mode) {
			
					
			case MODE_DEATHSEQUENCE:
				cube->deathseq();
				break;

			case MODE_GAME:
			case MODE_DEMO:
				if (!countdown_on) {
					animation_interval = 500000;
					cube->loop(p1_direction,p2_direction);
				} else {
					animation_interval = 750000;
					// Do the cube countdown
					cube->clear();
					for (int i = countdown_ticker-1; i > -1; i--) {
						cube->setPoint(true,i,0,0);
						cube->setPoint(true,i,7,7);
					}
					countdown_ticker--;
					if (countdown_ticker < 0) {
						countdown_on = false;
						cube->clear();
					}
				}
				break;

			case MODE_ANIMATION:
				if (gameover) {
					gameover = false;
					rebootTheCube();
				}

				switch (mode_animation) {
					case MODE_ANIMATION_RAIN:
						animation_interval = 50000;
						cube->rainAnimation();
						break; 
					case MODE_ANIMATION_GROWCUBE:
						animation_interval = 200000;
						cube->growCubeHandler();
						break;
					case MODE_ANIMATION_FUNNYTUBE:
						animation_interval = 125000;
						cube->funnyTubeHandler();
						break;
					case MODE_ANIMATION_FUNNEL:
						animation_interval = 75000;
						cube->funnelHandler();
						break;
					case MODE_ANIMATION_FUNNELGROW:
						animation_interval = 75000;
						cube->funnelHandler(true);			
						break;
					case MODE_ANIMATION_STARFIELD:
						animation_interval = 100000;
						cube->starfieldHandler();
						break;
				}

				// 
				break;

			case MODE_WAIT:
				if (playerack_count > 1) {
					if (playerack_p1taps > 1) {
						mode = MODE_DEMO;
					} else {
						mode = MODE_GAME;
					}
				}
				break;

			case MODE_PLAYER1_ACK:
			case MODE_PLAYER2_ACK:
				// Serial.println("player one ack...");
				animation_interval = 100000;

				if (mode == MODE_PLAYER2_ACK) { 
					playerack_side = 0; 
				}				

				cube->clear();
				for (int g = 0; g < 8; g++) {
					cube->setPoint(true,playerack_position,g, playerack_side);
				}

				if (playerack_position < 8) {
					playerack_position++;
				} else {
					playerack_position = 0;
					if (!gameover) {
						mode = MODE_WAIT;
					} else {
						gameover = false;
						cube->clear();
						rebootTheCube();
						mode = MODE_ANIMATION;
					}
				}

				break;



			case MODE_DEBUG:
				animation_interval = 25000;
								
				// !bang
				cube->setPoint(true,debugz,debugx,debugy);
				
				if (debugx < 7) { 
					debugx++; 
				} else {
					debugx = 0;
					if (debugy < 7) {
						debugy++;
					} else {
						// Turn off this layer and decrement z if necessary.
						if (debugz > 0) {
							cube->clear();
							debugz--;
							debugx = 0;
							debugy = 0;
						} else {
							cube->clear();
							debugz = 7;
							debugx = 0;
							debugy = 0;
						}
					}
				}
		
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



	
 
}

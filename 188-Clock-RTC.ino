//make change from desktop
// left leds x=0
// top leds y=0
//rgb 0 = 6 o'clock, increment clockwise
//change from laptop 2
#include <Adafruit_NeoPixel.h>
#include "CharTable.h"
#include <avr/pgmspace.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <DS3231.h>
#include <Wire.h>
#include "Images.h"
#include <LEDMatrixDriver.hpp>

//WS2812 RGB LEDS
#define PIN            6 //2 on nano, 7 on mega cricket
#define NUMPIXELS      12
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint32_t Forecolor = 0x000800;
uint8_t Brange = 6; //1, 3, 5 //set the brightness for each chanel in getrand();
uint8_t green; //used in getrand
uint8_t red;   //used in getrand
uint8_t blue;  //used in getrand

//DS3231 RTC
#define SETCLOCK 0
bool Century = false;
bool h12 = true; //false=24 mode , true=12hr mode
bool PM = false; //flase = PM, true = AM
DS3231 Clock;  // RealTime Clock - i2C - DS3231 ( Data = A4 Clock = A5)


//16x16 LED MATRIX
const int LEDMATRIX_SEGMENTS = 4;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;
const uint8_t LEDMATRIX_CS_PIN = 7;
#define numCols 16
#define numRows 16
bool buffTemp[16][16]; //screen buffer-ish //screen buffer-ish (bool uses same amount of ram as byte)! this is ineficient this should be  byte[32]
//uint8_t buffTemp1[32];


//maps library to hardware// depends on board
int X1[16] = {7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0};
int Y2[16] = {8,15,14,13,12,11,10,9,24,31,30,29,28,27,26,25};
int Y3[16] = {0,7,6,5,4,3,2,1, 16,23,22,21,20,19,18,17};
int myOffset = 8; //  "center" of screen. width/2 = 16/2 = 8
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN); //MOSI = 11 , SCK = 13

//rgb LEDS "columns" from left to right
byte columnArray[7][2] = {
	{3,3},
	{2,4},
	{1,5},
	{0,6},
	{7,11},
	{8,10},
	{9,9},
};

int cycles = 1000;


void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  lmd.setEnabled(true);
  lmd.setIntensity(0);   // 0 = low, 10 = high
  Wire.begin(); // Start the I2C interface
  Serial.begin(9600); 	// Start the serial interface
  
  if(SETCLOCK){ //factory date and 00:00 alarms
	Clock.setYear(20); //2020
	Clock.setMonth(5); //May
	Clock.setDate(4); //the 4th
	Clock.setDoW(1); //day of week
	Clock.setHour(8); //8pm
	Clock.setMinute(29);
	Clock.setSecond(0);
	Serial.println(Clock.getYear(), DEC);
	Serial.println(Clock.getMonth(Century), DEC);
	Serial.println(Clock.getDate(), DEC);
	Serial.print(Clock.getHour(h12, PM), DEC);
	Serial.print(':');
	Serial.print(Clock.getMinute(), DEC);
	Serial.print(':');
	Serial.print(Clock.getSecond(), DEC);
//     for (int x=0; x<8; x++){
//     EEPROM.write(x, 0); //write 0hrs and 0mins to the 4 alarms  
//     }
  }  
}





void loop(){
ColorWipe(0,0);
lmd.clear();
lmd.display();

cycles = 1000;
nBalls();
ColorWipe(0,0);
lmd.clear();//lmd.display();

SecondTicker();

lmd.clear();
lmd.display();

SineTemp();
ScrollTime();

// sprintf(mTime,"%d%d",Clock.getHour(h12, PM),Clock.getMinute() ); //doesnt quite fit. may need to get creative need to make a 5x1 cahacter for "1" and ":" so they dont use 3 spaces
// printString(mTime,0,6);
// lmd.display();
// delay(1000);

SecondSine();

cycles = 300;
nBalls();
ColorWipe(0,0);
lmd.clear();
lmd.display();

cycles = 40;
for(int i=0; i<10; i++) { //Loop until cycles
	nGrid();
}


// lmd.clear();
// lmd.display();
// drawBitmap(bitmap4);
// delay(1000);
// for(int i=0; i<6; i++){
// 	rotateScreen(1,-1); //number of rotations
// 	rotateScreen(1,1); //number of rotations
// }
// lmd.clear();
// lmd.display();



for(int i = 0; i<10; i++){ //count with slide loop animation
	char myString[3];
	sprintf(myString,"%d",i);
	lmd.clear();
	printStringWithShiftL(myString, 30,5);
	rotateScreen(1,1);
	printStringWithShiftL("    ", 30,5);
}



for(int i=0; i<12; i++){ //spinning randoms
	for(int i=1; i<=200; i++){
	for(int i=0; i<15; i++) SetLEDxy(random(0,numCols),random(0,numCols),0);
	for(int i=0; i<5; i++) SetLEDxy(random(0,numCols),random(0,numCols),1);
	lmd.display();
	delay(5);
	}
	rotateScreen(.25,1);
	getrand();
	pixels.setPixelColor(0,Forecolor);
	pixels.show();
	RotRGBRight();

}  	
	    
	  
for(int i=1; i<=600; i++){ //rainning
	if(i%50==0){
		RotRGBRight();
		getrand();
		pixels.setPixelColor(0,Forecolor);	
		pixels.show();	
	}
SetLEDxy(random(0,numCols),0,1);
ShiftLEDDown();
BlankRow(0);
delay(20);	
}



lmd.clear();
lmd.display();
drawBitmap(bitmap1);
delay(1000);
rotateScreen(8,-1); //number of rotations
lmd.clear();
lmd.display();


drawBitmap(bitmap1); //draw a bitmap
delay(3000);
for(int i=0; i<numCols-1; i++){ //shift off screen
	ShiftLEDDown();
	BlankRow(0);
	delay(40);
}
lmd.clear();
lmd.display();


for(int x=13; x>7; x--){ //spiral in ON
  Spiral(1,x,x); //WIP
}
for(int x=13; x>7; x--){ //spiral in OFF
  Spiral(0,x,x); //WIP
}
lmd.clear();
lmd.display();


//draw an + shape and spin it
 for(int xx =0; xx<8; xx++) { //draw an image (line)
  SetLEDxy(8,8-xx,1);
  SetLEDxy(8,xx+8,1);
  SetLEDxy(xx+8,8,1);
  SetLEDxy(8-xx,8,1);
  lmd.display();
  delay(50);
}

rotateScreen(15,1); //number of rotations
delay(200);
lmd.clear();
lmd.display();


printStringWithShiftL(" BE ACTIVATED 2020     ", 50,5); //Send Lscrolling Text (send car array)  
for(int i = 0; i<10; i++){
	char myString[3];
	sprintf(myString,"%d  ",i);
	printStringWithShiftL(myString, 40,i);
}
lmd.clear();
lmd.display();
  
  

// for(int x=0;x<5;x++){ //some random rgb loops
//   for(int i=0;i<NUMPIXELS;i++){
//     getrand();
//     pixels.setPixelColor(i, Forecolor); // Moderately bright green color.
//     pixels.show(); // This sends the updated pixel color to the hardware.
//     delay(50); // Delay for a period of time (in milliseconds).
//   }
// }
// 
// 
// 
// for(int x=0;x<3;x++){ //some random rgb loops
//   for(int i=0;i<NUMPIXELS;i++){
//     getrand();
//     pixels.setPixelColor(i, Forecolor); // Moderately bright green color.
//     pixels.show(); // This sends the updated pixel color to the hardware.
//     delay(50); // Delay for a period of time (in milliseconds).
//   }
//    for(int i=0;i<NUMPIXELS;i++){
//   pixels.setPixelColor(0, 0); // Moderately bright green color.
//   RotRGBRight();
//   pixels.show();
//   delay(50);
//  }
// }


} //end main loop


void SecondSine(){
for(int i = 0; i<10; i++){ //sine wave seconds
	int mSec = Clock.getSecond();
	int j = numCols-1;
	char mString[3];
	while (mSec==Clock.getSecond()); //wait till the time is different
	sprintf(mString,"%d",Clock.getSecond());
	for(float i = 0; i<=2.2; i+=.1){
		printString(mString, j--,(int)4*sin(M_PI*i-2)+5);
		lmd.display();
		delay(30); //trying to sync this up with the clock
		lmd.clear();
	}
}
}

void ScrollTime(){
for(int i=0; i<10; i++){
	char mTime[5];
	sprintf(mTime,"%d:%02d:%02d    ",Clock.getHour(h12, PM),Clock.getMinute(), Clock.getSecond() );
	printStringWithShiftL(mTime,40,6);
}
}

void SineTemp(){
for(int i = 0; i<10; i++){ //sine wave tempurature
	int mSec = Clock.getSecond();
	int j = numCols-1;
	char mString[3];
	getrand();
	while (mSec==Clock.getSecond()); //wait till the time is different
	sprintf(mString,"%02d", (int)((Clock.getTemperature()*9/5)+32));
	for(float i = 0; i<=2.2; i+=.1){
		printString(mString, j--,(int)4*sin(M_PI*i-2)+5);
		printString("F",7,11);
		lmd.display();
		pixels.setPixelColor(columnArray[(int)(6-floor(i*10/3))][0],Forecolor);
		pixels.setPixelColor(columnArray[(int)(6-floor(i*10/3))][1],Forecolor);
		if(i>1.1){
			pixels.setPixelColor(columnArray[(int)(floor(i*10/3))][0],0);
			pixels.setPixelColor(columnArray[(int)(floor(i*10/3))][1],0);
		}
		pixels.show();
		delay(30); //trying to sync this up with the clock
		lmd.clear();
		//ColorWipe(0,0);
	}
	ColorWipe(0,0);
}
}

void SecondTicker(){ //this is bugged. making a change
	for(int i=0; i<5; i++){ //5*12 = 60 seconds
		getrand();
		char mTimepos[] = {6,7,8,9,10,11,0,1,2,3,4,5};
		char p=0;
		storeScreen();
		for(float mAngle = -M_PI/2; mAngle<(M_PI*3/2)-(2*M_PI/12); mAngle+=(2*M_PI/12)){
			for(int m=0; m<8; m++){
				for(int i=7; i<=8; i++) SetLEDxy((int)(m*cos(mAngle))+7,(int)(m*sin(mAngle))+i,1);
				for(int i=7; i<=8; i++) SetLEDxy((int)(m*cos(mAngle))+8,(int)(m*sin(mAngle))+i,1);
			}
			pixels.setPixelColor(mTimepos[p++],Forecolor);
			lmd.display();
			pixels.show();
			int mSec = Clock.getSecond();
			while (mSec==Clock.getSecond()); //wait 1 second
			lmd.clear();

		}
		ColorWipe(0,0);
	}
}


void nGrid(){ //make a snake head pop upin a random spot and move it arround the screen randomly
	//lmd.clear();
	char numBalls = 30;//random(10,30); //number of balls
	char Limitx = numCols-1; //width
	char Limity = numRows-1;//height
	char Headx[numBalls]; //head postion holder
	char Heady[numBalls]; //head postion holder
	char Dirx[numBalls]; //-1 or 1
	char Diry[numBalls]; //-1 or 1
	char Tailx[numBalls]; //tailx psotion. is not really 'seen'. exists to reduce "flickering"
	char Taily[numBalls]; //taily position
	uint8_t myExit = 0;
	for(char c=0; c<numBalls; c++){ //loop through all arrays to set intials
		Headx[c] = random(0,Limitx+1); //pick n number of random x start points
		Heady[c] = random(0,Limity+1); //pick n number of random y start points
		if(random(0,2)){
			Dirx[c] = 1;//pow(-1,random(1,2));
			Diry[c] = 1;//pow(-1,random(1,2));
			}else{
			Dirx[c] = -1;//pow(-1,random(1,2));
			Diry[c] = -1;//pow(-1,random(1,2));
		}
	}
	for(int i=0; i<=cycles; i++){
		for(char c=0; c<numBalls; c++){
			if(Headx[c]==0) Dirx[c]=1;
			if(Headx[c]==Limitx) Dirx[c]=-1;
			if(Heady[c]==0) Diry[c]=1;
			if(Heady[c]==Limity) Diry[c]=-1;
			
			Tailx[c] = Headx[c];
			Taily[c] = Heady[c];
			if(c%2==0){
				Headx[c]+=Dirx[c];
				}else{
				Heady[c]+=Diry[c];
			}
			SetLEDxy(Headx[c],Heady[c],1);
			SetLEDxy(Tailx[c],Taily[c],0);
			lmd.display();
			delayMicroseconds(500);
			
		}
	}
	if(random(0,1000)==500){
		printStringWithShiftL(" STARIOS", 40,6); //Send Lscrolling Text (send car array)
	}
for(int z=0; z<numBalls; z++) SetLEDxy(Headx[z],Heady[z],0); //needed to clear the stuck balls
lmd.display();	
}

//using lots of memory
void nBalls(){ //make a snake head pop upin a random spot and move it around the screen randomly
	char numBalls = 5; //if this is too high the program will crash
	char Limitx = numCols-1; //width
	char Limity = numRows-1;//height
	char Headx[numBalls]; //head potion holder
	char Heady[numBalls]; //head postion holder
	char Dirx[numBalls]; //-1 or 1
	char Diry[numBalls]; //-1 or 1
	char Tailx[numBalls]; //tailx position. is not really 'seen'. exists to reduce "flickering"
	char Taily[numBalls]; //taily position
	char ballhit[12];
	char ledmap[12]={0,1,2,3,4,5,6,7,8,9,10,11};
	for(char c=0; c<numBalls; c++) ballhit[c]=0; //clear all the flags
	
	char mString[3];
	for(char c=0; c<numBalls; c++){ //loop through all arrays to set intials
		Headx[c] = random(0,Limitx+1); //pick n number of random x start points
		Heady[c] = random(0,Limity+1); //pick n number of random y start points
		Dirx[c] = 1;//pow(-1,random(1,2));
		Diry[c] = 1;//pow(-1,random(1,2));
	}
	for(int i=0; i<=cycles; i++){
		for(char c=0; c<numBalls; c++){
			if(Headx[c]==0) Dirx[c]=1;
			if(Headx[c]==Limitx) Dirx[c]=-1;
			if(Heady[c]==0) Diry[c]=1;
			if(Heady[c]==Limity) Diry[c]=-1;
			Tailx[c] = Headx[c];
			Taily[c] = Heady[c];
			Headx[c]+=Dirx[c];
			Heady[c]+=Diry[c];			
			SetLEDxy(Headx[c],Heady[c],1);
			SetLEDxy(Tailx[c],Taily[c],0);
			lmd.display();
			//delayMicroseconds(500); //speed
			delay(5);
			
		}
		//temperature here
		sprintf(mString,"%02d", (int)((Clock.getTemperature()*9/5)+32));
		printString(mString,2,6);
		printString("F",11,6);
		//enable the rgb leds if they are 'hit'
		for(char c=0; c<12; c++){ //check each led activation location one at a time
			char xmap1[12]={7,  4, 1,0,1,4, 7,11,14,15,14,11}; //activation locations X
			char ymap1[12]={15,14,11,7,4,1, 0,1,  4, 7,11,14}; //activation locations Y
			char xmap2[12]={8,  3, 1,0,1,3, 8,12,14,15,14,12}; //activation locations X
			char ymap2[12]={15,14,12,8,4,1, 0,1,  3, 8,12,14}; //activation locations Y
			for(char i=0; i<numBalls; i++){ //loop trough all balls to see if any are touching activation location
				if (Headx[i] == xmap1[c] && Heady[i] == ymap1[c] )ballhit[c]=1; //if any one of them are touching then write a 1 to that led being touched
				if (Headx[i] == xmap2[c] && Heady[i] == ymap2[c] )ballhit[c]=1; //if any one of them are touching then write a 1 to that led being touched
			}
		}
		for(char c=0; c<12; c++){ //loop thru the leds again
			if(ballhit[c]){ //if actived then turn the led to a color
				getrand();
				pixels.setPixelColor(ledmap[c],Forecolor);
				ballhit[c]=0; //clear the activated
			}else{
				pixels.setPixelColor(ledmap[c],0);
			}
		}
		pixels.show();

		
		if(random(0,20) < 2){ //get a random number after all the balls have been updated
			for(int x=0; x<numBalls; x++){ //loop through each ball
				if(Headx[x]<numCols-2 && Headx[x]>=2){
					for(int z=0; z<numBalls; z++) SetLEDxy(Headx[z],Heady[z],0); //needed to clear the stuck balls
					if(random(0,2)){ //this applys a shift to the balls to change the trajecories up
						Tailx[x] = Headx[x];
						Headx[x]++;
						}else{
						Tailx[x] = Headx[x];
						Headx[x]--;
					}
				}
			}
		}
	}
for(int z=0; z<numBalls; z++) SetLEDxy(Headx[z],Heady[z],0); //needed to clear the stuck balls //needed? this only needed if nballs is in a forloop and screen is not cleared?
lmd.display();
}




void rotateScreen(float angPercent, int dir){ //rotates all pixels theta rads counter clockwise
//dir: 1 for CCW, -1 for CW
storeScreen();
 int angleIndex = 0;
 for(float i=0; i<=2*angPercent; i+=.05){
  for(int x=0; x<16; x++){
    for(int y=0; y<16; y++){
        int myXT = (int) (myOffset+(((x-myOffset)*cos(M_PI*i))-(dir)*((y-myOffset)*sin(M_PI*i))));
        int myYT = (int) (myOffset+(((x-myOffset)*(dir)*sin(M_PI*i))+((y-myOffset)*cos(M_PI*i))));
        SetLEDxy(myXT,myYT,buffTemp[x][y]);               
	  }
    }
	lmd.display();
  }
  if(ceilf(angPercent) == angPercent) printStored(); //if angle to rotate is integer value then fix rotation error by printing the tempbuff
}

// void rotateScreen1(float angPercent, int dir){ //rotates all pixels theta rads counter clockwise
// 	//dir: 1 for CCW, -1 for CW
// 	storeScreen(); //copy the values from the buffer to a temp buffer location
// 	int angleIndex = 0;
// 	for(float i=0; i<=2*angPercent; i+=.05){
// 		for(int x=0; x<16; x++){
// 			for(int y=0; y<16; y++){
// 				int myXT = (int) (myOffset+(((x-myOffset)*cos(M_PI*i))-(dir)*((y-myOffset)*sin(M_PI*i))));
// 				int myYT = (int) (myOffset+(((x-myOffset)*(dir)*sin(M_PI*i))+((y-myOffset)*cos(M_PI*i))));
// 				SetLEDxy(myXT,myYT,getBuffPixel(x, y));
// 			}
// 		}
// 		lmd.display();
// 	}
// 	if(ceilf(angPercent) == angPercent) printStored1(); //if angle to rotate is integer value then fix rotation error by printing the tempbuff
// }
// 


// void storeScreen1(){ //copys library display buffer to a new array
// uint8_t* addr =  lmd.getFrameBuffer();
//   for(int i=0; i<32; i++){
//   *(buffTemp1+i) = *(addr+i);
//   }
// }
// 
// void printStored1(){ //copies tempbuffer to display buffer
// 	uint8_t* addr =  lmd.getFrameBuffer();
// 	for(int i=0; i<32; i++){
// 		*(addr+i)=*(buffTemp1+i);
// 	}
// 	lmd.display();
// }
// 
// 
// bool getBuffPixel(int16_t x, int16_t y){
// if(x<8)
// 		uint16_t B = Y2[y] >> 3;		//byte
// 		uint8_t* p = buffTemp1 + X1[x]*4 + B;
// 		uint16_t b = 7 - (Y2[y] & 7);		//bit
// 		return *p & (1 << b);
// 	}else{
// 		uint16_t B = Y3[y] >> 3;		//byte
// 		uint8_t* p = buffTemp1 + X1[x]*4 + B;
// 		uint16_t b = 7 - (Y3[y] & 7);		//bit
// 		return *p & (1 << b);
// }
// }





void storeScreen(){ //copy every mono led state to a buffer called bitmap
  for(int x=0; x<16; x++){
    for(int y=0; y<16; y++){
      buffTemp[x][y] = GetLEDxy(x,y);
    }
  }
}

void printStored(){ //print the buffer called bitmap
  for(int x=0; x<16; x++){
    for(int y=0; y<16; y++){
      SetLEDxy(x,y,buffTemp[x][y]);
    }
  }
lmd.display();
}




void SetLEDxy(int x, int y, bool c){ //just pass in cartesian (x,y) pairs
    if(x<8){
    lmd.setPixel(Y2[y], X1[x], c);
    }else{
    lmd.setPixel(Y3[y], X1[x], c);
    }
}

bool GetLEDxy(int x, int y){ //just pass in cartesian (x,y) pairs
    if(x<8){
      return lmd.getPixel(Y2[y], X1[x]);
    }else{
      return lmd.getPixel(Y3[y], X1[x]);
    }
}



void drawBitmap(char* image){ //draws a bitmap from a hex array defined at top
	byte Tempbuff = 0;
	int mySide = 1;
	int myYY = 0;
	for(int x=0; x<32; x++){
		Tempbuff = pgm_read_byte(&(image[x]));
		uint8_t b = 0b10000000;
		if(x%2==0){
			mySide = 0;
			if(x) myYY++;
			}else{
			mySide = 8;
		}
		for (int k = 0; k < 8; k++) {
			if (!(Tempbuff&b)) SetLEDxy(mySide+k,numRows-1-myYY,1);
			b = b >> 1;
		}
	}
	lmd.display();
}


void Spiral(int c, int myX, int myY ){ //rotates all pixels theta rads counter clockwise and leaves a "trail"
  for(float i=0; i<2; i+=.01){
    int myXT = (int) (myOffset+(((myX-myOffset)*cos(M_PI*i))-((myY-myOffset)*sin(M_PI*i))));
    int myYT = (int) (myOffset+(((myX-myOffset)*sin(M_PI*i))+((myY-myOffset)*cos(M_PI*i))));
    SetLEDxy(myXT,myYT,c);
    lmd.display();
    //delay(1);
  }
}

void FillScreen(bool c){
    for(int x=0; x<16; x++){
    for(int y=0; y<16; y++){
    SetLEDxy(x,y,c);
    }
  }
}

//Extract characters for Static Text
void printString(char* s, int x, int y){
	while (*s != 0) {
		printChar(*s, x, y);
		s++;
		x+=4;
	}
}

// Put extracted character on Display
void printChar(char c, int x, int y){
	byte mybuff;
	if (c < 32) return; //error check for ascii values less than ASCII  < ' ' (space) = 32 >
	c -= 32;			//align the char ascii with the CharTable
	for (int j = 0; j < 3; j++) {
		mybuff = pgm_read_byte(&(CH[3*c+j]));
		uint8_t b = 0b00000001;
		for (int k = y; k < y+5; k++) {
			if (mybuff&b) SetLEDxy(j+x,k,1);
			b = b << 1;
		}

	}
}

//Extract characters for Scrolling text -PHASE THESE OUT? same as above but with shiftL()
void printStringWithShiftL(char* s, int shift_speed, int textpos) {
  while (*s != 0) {
    printCharWithShiftL(*s, shift_speed, textpos);
    s++;
  }
}

// Put extracted character on Display
void printCharWithShiftL(char c, int shift_speed, int textpos) {
byte mybuff;
  if (c < 32) return; //error check for ascii values less than ASCII  < ' ' (space) = 32 >
  c -= 32; //align the char ascii with the CharTable
  for (int j = 0; j <= 2; j++) {
	mybuff = pgm_read_byte(&(CH[3*c+j]));
    uint8_t b = 0b00000001;
    for (int k = textpos; k < textpos+5; k++) {
      if (mybuff&b) SetLEDxy(numCols-1,k,1);
      b = b << 1;
    }
    lmd.display();
    ShiftLEDLeft(); //shift every column one column left (one space between letters)
    BlankColumn(numCols-1);
    delay(shift_speed);
  }
  ShiftLEDLeft(); // (one space after word)
}

//shift mono leds Left function
void ShiftLEDLeft () {   //Shift columns right 1 palce
  for ( int x = 1; x <= numCols-1; x++) { //read one behind left
    for (int y = 0; y <= numRows-1; y++) { //read each Led value
      SetLEDxy(x-1,y,GetLEDxy(x,y));
    }
  }
  lmd.display();
}

//shift mono leds Left function
void ShiftLEDDown () {   //Shift columns right 1 palce
	for (int y = numRows-2; y >= 0; y--) { //read one behind left
		for (int x = 0; x <= numCols-1; x++) { //read each Led value
			SetLEDxy(x,y+1,GetLEDxy(x,y));
		}
	}
	lmd.display();
}

void BlankColumn(int c) {    //wipes the coulmn 0 to clear out jjunk, to get ready for shifting
  for (int w = 0; w <= numRows-1; w++) {
	  SetLEDxy(c,w,0);
  }
}

void BlankRow(int c) {    //wipes the coulmn 0 to clear out jjunk, to get ready for shifting
	for (int w = 0; w <= numCols-1; w++) {
		SetLEDxy(w,c,0);
	}
}

void RotRGBRight(){ //shifts all RGB pixels right 1 place
    for(int i=NUMPIXELS-1; i>0; i--){
    pixels.setPixelColor(i, pixels.getPixelColor(i-1)); // Moderately bright green color.
  }
}

void ColorWipe(uint32_t c, int s){
	for(int i=0;i<NUMPIXELS;i++){
		pixels.setPixelColor(i, c); // Moderately bright green color.
		pixels.show(); // This sends the updated pixel color to the hardware.
		delay(s); // Delay for a period of time (in milliseconds).
	}
}

void getrand() { // trying to make it //the goal is to get a random forecolor that is not white, then find the opposite of
      switch (random(0, 3)) {
        case 0:
          green = (random(0, Brange + 1) * 2);
          red = (random(0, Brange + 1) * 2);
          blue = 0;
          if (green == 0 || red == 0) { //prevents all black values
            blue = 1;
          };
          break;

        case 1:
          green = (random(0, Brange + 1) * 2);
          blue = (random(0, Brange + 1) * 2);
          red = 0; //32
          if (green == 0 || blue == 0) {
            red = 1;
          };
          break;

        case 2:
          red = (random(0, Brange + 1) * 2);
          blue = (random(0, Brange + 1) * 2);
          green = 0; //32
          if (red == 0 || blue == 0) {
            green = 1;
          };
          break;
      }
      Forecolor = (65536 * green) + (256 * red) + (1 * blue);
}

void LedTest(int s){
	for(int x=0; x<16; x++){
		for(int y=0; y<16; y++){
			SetLEDxy(y,x,1);
			SetLEDxy(x,y,1);
			lmd.display();
			delay(s);
			SetLEDxy(y,x,0);
			SetLEDxy(x,y,0);
			delay(s);
			lmd.display();
		}
	}
	for(int x=numCols-1; x>=0; x--){
		for(int y=numRows-1; y>=0; y--){
			SetLEDxy(y,x,1);
			SetLEDxy(x,y,1);
			lmd.display();
			delay(s);
			SetLEDxy(y,x,0);
			SetLEDxy(x,y,0);
			delay(s);
			lmd.display();
		}
	}
}

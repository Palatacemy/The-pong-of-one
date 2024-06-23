#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3

//input
int IUP = A0; //vertical input pin
int ILR = A1; //horisontal input pin
int SWPIN = 2; //press input pin
int MID = 0; //mid
int UPMID = 0; //calibrated vertical
int LRMID = 0; //calibrated horisontal

//reset button
bool RESETPENDING = false;

//for plate
int VA = 0; //current vertical location
int HA = 0; // current horisontal location
int VAINP = 0; //vertical input
int HAINP = 0; //horisontal input
int VDIR = 0; //direction of vertical movement
int HDIR = 0; //direction of horisontal movement
int DEF[2] = {4, 2}; //defaults

//for ball
int BVA = 0; //location
int BHA = 0;
int BVDIR = 0; //direction of movement
int BHDIR = 0;
int OBVA = 0; //old coordinates
int OBHA = 0;
int BDEF[2] = {4, 9}; //defaults
bool IMC = false; //impact cooldown

//print assets
byte pss[8] = {B10000000, B11000000, B10100000, B10010000, B10001000, B10000100, B10000010, B10000001};
byte bpss[8] = {B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001};

//initialise matrix
MD_MAX72XX dis = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {

  //pin setup
  pinMode(SWPIN, INPUT);

  //calibrate
  LRMID = analogRead(ILR);
  UPMID = analogRead(IUP);

  //game setup
  VA = DEF[0];
  HA = DEF[1];
  BVA = BDEF[0];
  BHA = BDEF[1];
  BVDIR = 1;
  BHDIR = 1;

  //display stuff
  dis.begin();
  dis.control(MD_MAX72XX::INTENSITY, 0);
  dis.clear();
}

void loop() {

  //delete plate and ball
  deletePlate(VA, HA);
  deleteBall(BVA, BHA);
  deleteBall(OBVA, OBHA);

  //read input
  VAINP = analogRead(IUP);
  HAINP = analogRead(ILR);
  /*
  RESETPENDING = digitalRead(SWPIN);
  */

  //reset
  if(RESETPENDING == true){
  VA = DEF[0];
  HA = DEF[1];
  BVA = BDEF[0];
  BHA = BDEF[1];
  RESETPENDING = false;
  }

  //set old coordinates
  OBVA = BVA;
  OBHA = BHA;

  //calculate plate direction
  if(VAINP < UPMID - MID){
  VDIR = -1;
  }
  if(VAINP > UPMID - MID){
  VDIR = 1;
  }
  if(HAINP < LRMID - MID){
  HDIR = -1;
  }
  if(HAINP > LRMID - MID){
  HDIR = 1;
  }

  //move plate
  if(VA + VDIR >= 2 && VA + VDIR <= 7){
  VA += VDIR;
  }
  if(HA + HDIR >= 1 && HA + HDIR <= 7){
  HA += HDIR;
  }

  //move ball
  if(BVA + BVDIR >= 0 && BVA + BVDIR <= 7){
  BVA += BVDIR;
  }
  else{
    if(BVA + BVDIR < 0){
    BVDIR = 1;
    BVA = 0;
    BVA += BVDIR;
    }
    if(BVA + BVDIR > 7){
    BVDIR = -1;
    BVA = 7;
    BVA += BVDIR;
    }
  }
  
  if(BHA + BHDIR >= 1 && BHA + BHDIR <= 23){
    if(BHA == HA && (BVA == VA || BVA == VA - 1 || BVA == VA - 2)){
    BHDIR *= -1;
    }
  BHA += BHDIR;
  }
  else{
    if(BHA + BHDIR < 1){
    BHDIR = 1;
    BHDIR = 1;
    BVA = BDEF[0];
    BHA = BDEF[1];
    }
    if(BHA + BHDIR > 23){
    BHDIR = -1;
    BHA = 23;
    BHA += BHDIR;
    printWin();
    }
  }

  //print plate and ball
  printWall();
  printPlate(VA, HA);
  printBall(BVA, BHA);
  printBall(OBVA, OBHA);
  
  delay(225);
}


void printWin(){
for(int i = 0; i < 8; i++){
dis.setRow(0, 0, i, B00001111);
}
delay(100);
for(int i = 0; i < 8; i++){
dis.setRow(0, 0, i, B00000000);
}
}

void printWall(){
for(int i = 0; i < 8; i++){
dis.setRow(3, 3, i, B10000000);
}
}

void printPlate(int VA, int HA){
for(int i = VA; i > VA - 3; i--){
dis.setRow(3, 3, i, pss[HA % 8]);
}
}

void deletePlate(int VA, int HA){
for(int i = VA; i > VA - 3; i--){
dis.setRow(3, 3, i, B00000000);
}
}

void printBall(int BVA, int BHA){
int board = 3 - (BHA - (BHA % 8)) / 8;
if(BHA <= 7){
dis.setRow(board, board, BVA, pss[BHA % 8]);
}
else{
dis.setRow(board, board, BVA, bpss[BHA % 8]);
}
}

void deleteBall(int BVA, int BHA){
int board = 3 - (BHA - (BHA % 8)) / 8;
dis.setRow(board, board, BVA, B00000000);
}

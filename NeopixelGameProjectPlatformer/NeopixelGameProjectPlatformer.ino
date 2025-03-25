#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6

int walkticks = 0;
int tempData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int pulseNumber = 1;
const int A_BUTTON         = 0;
const int B_BUTTON         = 1;
const int SELECT_BUTTON    = 2;
const int START_BUTTON     = 3;
const int UP_BUTTON        = 4;
const int DOWN_BUTTON      = 5;
const int LEFT_BUTTON      = 6;
const int RIGHT_BUTTON     = 7;
const int levelWd = 32;

//===============================================================================
//  Pin Declarations
//===============================================================================
// Declare the pin numbers for the NES controller
int nesData       = 4;    // The data pin for the NES controller
int nesClock      = 2;    // The clock pin for the NES controller
int nesLatch      = 3;    // The latch pin for the NES controller

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

uint16_t COLORS[6] = {
  matrix.Color(0, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 100, 0), matrix.Color(255, 255, 0), matrix.Color(255, 0, 0)
};

// TYPE data key: {interfere with player physics}
# define END_OF_LEVEL_BLOCK 2
int TYPES[3][1] = {
   {1},   //Terrain  
   {1},   //Enemy
   {-1}    //End of level
};

#define ENV_LENGTH 6
// Object data key: {color, x, y, width, height, typex}
int env[ENV_LENGTH][6] = {
{1, 0, 14, 16, 2, 1},
{2, 3, 11, 5, 1, 1},
{2, 7, 10, 5, 1, 1},
{2, 11, 9, 5, 1, 1},
{2, 19, 14, 16, 2, 1},
{5, 31, 8, 1, 8, 2},
};

//Player variables
//{color, x, y, vertical speed, direction}  //Direction - 1: Right 0: Left
int player[5] = {3, 5, 5, 0, 1};

const int gravity = 1;

int enemies[1][4] = {
  {0, 5, 14, 1}
};

#define MAX_PROJ_ALLOWED 8
#define PROJ_FIRE_DELAY 500
long lastProjFireTime = 0;
int visibleProjCount = 0;
//{active, color, x, y, type, speed}
int proj[MAX_PROJ_ALLOWED][6] = {
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1},
  {0, 4, 0, 0, 0, 1}
};


int leftXCoord = 0;

//GAME CLOCK
long nextScreenRefresh = 0;
const long GAME_REFRESH_DELAY = 100; 

void setup() {
  Serial.begin(9600);
  
  // Set appropriate pins to inputs and outputs
  pinMode(nesData, INPUT);
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);
  
  // Set initial states for clock and latch
  digitalWrite(nesClock, LOW);
  digitalWrite(nesLatch, LOW);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(matrix.Color(255, 0, 0));
}

void checkNESController() {
  for (int i = 0; i < 8; i++) {
        tempData[i] = 0; // Set each element to 0
    }
  // Quickly pulse the nesLatch pin so that the register grabs the current state of the buttons
  digitalWrite(nesLatch, HIGH);
  digitalWrite(nesLatch, LOW);

  // Shift in the button states for each button
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  pulseNumber += 1;
  
  if (digitalRead(nesData) == LOW)
    tempData[8-pulseNumber] = 1; 
/*
  for (int i = 0; i < 8; i++) {
    // Print the value of myArray[i]
    Serial.print(tempData[i]);
  }
  Serial.println("");
   */ 
  pulseNumber = 1;
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
}

void addProj(int direction) {

  //Serial.println(visibleProjCount);
  lastProjFireTime = millis();
  
  if (checkPixelType(player[1] + 1,player[2]) == -1 && visibleProjCount < MAX_PROJ_ALLOWED) {

    //Loop over the proj array to find a proj that is inactive proj[x][0] = 0
    for (int i = 0; i < MAX_PROJ_ALLOWED; i++) {
      
      if (proj[i][0] == 0) {
        //fire Projectile
        proj[i][0] = 1;
        proj[i][1] = 4;
        proj[i][2] = player[1]+direction;
        proj[i][3] = player[2];
        proj[i][4] = 0;
        proj[i][5] = direction;
        //proj[visibleProjCount] = {1, 4, player[1]+1, player[2], 0, 1};
        visibleProjCount++;

        return;
      }
    }
    
  }

}

#define INTRO 1

int game_state = 0;

void loop() {

  switch(game_state) {
    case INTRO:
      break;
    case PLAYING:
      break;
    case PAUSED:
      break;
    case END_LEVEL:
      break;
    default:
      break;
  
  }

  checkNESController();
  

  //Update LCD Panel

  if (millis() >= nextScreenRefresh) {
    for (int i = 0; i < MAX_PROJ_ALLOWED; i++) {
      if(proj[i][0] == 1) {

        //Check to see if the next block to the left or right (depending on direction) is -1, true move, false set the proj to inactive
        if (checkPixelType(proj[i][2] + proj[i][5], proj[i][3]) == -1 && proj[i][2] >= 0 && proj[i][2] < levelWd) {
          //Serial.println(String("Proj can move") + String(i));
          proj[i][2]+=proj[i][5];
        } else {
          proj[i][0] = 0;
          visibleProjCount--;
        }
        //delay(10);
        
        
      }
    }


    if (tempData[6] == 1 && millis() >= lastProjFireTime + PROJ_FIRE_DELAY) {
      
      
      addProj(player[4]);

      
  
    }
    
    //Vertical movement physics
    if (tempData[7] == 1 && checkPixelType(player[1],player[2]+1) != -1) { //A button
      player[3] = 4;
    }

    //if player speed is negative, 
      //check the block below, if we can move into it, update player y value, otherwise set player vertical speed to 1  speeds of 0 and 1 mean player does not move
    if (player[3] < 0 && checkPixelType(player[1],player[2]+1) == -1) {
      player[2]++;
    }
    //If player speed is 2 or greater
      //check the block above, if we can move into it, update player y value, otherwise set the player vertical speed to 1
    if (player[3] >= 2 && checkPixelType(player[1],player[2]-1) == -1) {
      player[2]--;
    }

    //Subtract 1 from player vertical speed
    player[3]--;
    
    walkticks += 1;
    if(walkticks > 1) {
      walkticks = 0;


      //Player movement
      if (tempData[0] == 1) { //Right
        player[4] = 1;
        if (player[1] < levelWd - 1 && checkPixelType(player[1]+1,player[2]) == -1) {
          player[1] = player[1] += 1;
        }
        
        if (player[1] - leftXCoord > 8 && leftXCoord + 16 < levelWd) {
          leftXCoord++;
        }
      }
      if (tempData[1] == 1) { //Left
        player[4] = -1;
        if (player[1] > 0 && checkPixelType(player[1]-1,player[2]) == -1) {
            player[1] = player[1] -= 1;
           
        }
        
       
        if (player[1] - leftXCoord < 9 && leftXCoord > 0) {
          
          leftXCoord--;
        }
      }
      /*
      if (tempData[3] == 1) { //Up
        if(player[2] > 0 && checkPixelType(player[1],player[2]-1) == -1) {
        player[2] = player[2] -= 1;
       
      }
      }
      if (tempData[2] == 1) { //Down
        if(player[2] < 15 && checkPixelType(player[1],player[2]+1) == -1) {
        player[2] = player[2] += 1;
        }
      }
      */
    }

    
    //Update the LCD Panel
    updateLEDPanel();
    //Set the time of the next LCD panel update
    nextScreenRefresh = millis() + GAME_REFRESH_DELAY;
  }

  
  /*
  //update camera again
  if(player[1] > 7) {
  if(player[1] < (levelWd-8)) {
  leftXCoord = player[1] - 7;
  } else {
    leftXCoord = levelWd-8;
  };
  }else {
    leftXCoord = 0;
  }


  //update camera again again
  if(player[1] > 7) {
  if(player[1] < (levelWd-16)) {
  leftXCoord = player[1] - 7;
  } else {
    leftXCoord = levelWd-24;
  };
  }else {
    leftXCoord = 0;
  }
  */
  delay(1);

}

int checkPixelType(int checkX, int checkY) {

  for (int i = 0; i < ENV_LENGTH; i++) {
    for (int x = env[i][1]; x < env[i][3] + env[i][1]; x++) {
      for (int y = env[i][2]; y < env[i][4] + env[i][2]; y++) {
        //matrix.drawPixel(x-leftXCoord, y, COLORS[env[i][0]]);
        if (checkX == x && checkY == y) {
          return env[i][5];
        }
      }
    }
  }

  return -1;
}

void updateLEDPanel() {
  matrix.fillScreen(0);
  //Draw the ENV
  for (int i = 0; i < ENV_LENGTH; i++) {
    for (int x = env[i][1]; x < env[i][3] + env[i][1]; x++) {
      for (int y = env[i][2]; y < env[i][4] + env[i][2]; y++) {
        matrix.drawPixel(x-leftXCoord, y, COLORS[env[i][0]]);
        
      }
    }
  }
//remember Noah:
//{active, color, x, y, type, speed}
  for (int i = 0; i < MAX_PROJ_ALLOWED; i++) {
  if(proj[i][0] == 1) {
  matrix.drawPixel((proj[i][2]-leftXCoord), proj[i][3], COLORS[proj[i][1]]);
  }
  }



  //leftXCoord += 1;  

    //Draw the player
  matrix.drawPixel((player[1]-leftXCoord), player[2], COLORS[player[0]]);

    matrix.show();
}


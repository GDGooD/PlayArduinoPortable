#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include "LowPower.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define BUTTON_L A2
#define BUTTON_R 2

void wakeUpHandler(){
  
}


void screensaverBall(){
  byte circleRadius = 2 + random(8);
  uint32_t hitCount = 0;
  byte ballSpeed = 2 + random (5);
  boolean w = random(1);
  boolean h = random(1);
  int8_t x = circleRadius + 2 + random(display.width() - circleRadius - 4);
  int8_t y = circleRadius + 2 + random(display.height() - circleRadius - 4);
  
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  
  while(true){
    if (digitalRead(BUTTON_R) or digitalRead(BUTTON_L))
      return;
    display.drawCircle(x, y, circleRadius, WHITE);
    //display.setCursor(0, 0);
    //display.println(hitCount);
    display.display();
    display.clearDisplay();
    if (x >= display.width() - circleRadius - 1 or x <= circleRadius){
      w = !w;
      //hitCount++;
    }
    if (y >= display.height() - circleRadius - 1 or y <= circleRadius){
      h = !h;    
      //hitCount++;
    }
    x = x + 2*ballSpeed*w - ballSpeed;
    y = y + 2*ballSpeed*h - ballSpeed;   
  }
}






//////////////
//////////////
///invaders///
//////////////
//////////////

const unsigned char shipBitmap [] PROGMEM{
  0x92, 0xba, 0xfe, 0xba, 0x82    
};

const unsigned char alien1Bitmap1 [] PROGMEM{
  0x70, 0xf8, 0xa8, 0xf8, 0xa8    
};

const unsigned char alien1Bitmap2 [] PROGMEM{
  0x70, 0xf8, 0xa8, 0xf8, 0x50    
};

const unsigned char alien2Bitmap1 [] PROGMEM{
  0x50, 0xa8, 0x70, 0x70, 0xa8   
};

const unsigned char alien2Bitmap2 [] PROGMEM{
  0x50, 0xa8, 0x70, 0x70, 0x50    
};

const unsigned char alien3Bitmap1 [] PROGMEM{
  0xf8, 0xa8, 0xf8, 0x50, 0x90    
};

const unsigned char alien3Bitmap2 [] PROGMEM{
  0xf8, 0xa8, 0xf8, 0x50, 0x48    
};

uint32_t spaceinvaders(){
  int8_t shipPosition = display.width()/2;
  int8_t shipMovCumulative = 0;
  #define SHIPSPEED 5
  #define SHIPWIDTH 3
  #define SHIPHEIGHT 5
  
  byte bulletX = shipPosition;
  byte bulletY = 63-SHIPHEIGHT;
  byte bulletSpeed;

  #define ALIENSX 8
  #define ALIENSY 4
  #define ALIENSXDISTANCE 5
  #define ALIENSYDISTANCE 4
  #define ALIENWIDTH 5
  #define ALIENHEIGHT 5
  
  byte aliens[ALIENSX][ALIENSY];
  byte aliensSpeed = 0;
  int8_t aliensPosX = 3;
  int8_t aliensPosY = -ALIENHEIGHT*ALIENSY-ALIENSYDISTANCE*ALIENSY;
  boolean aliensDirection = 0;
  boolean aliensDirectionIsChanged = 0;
  boolean aliensAnim = 0;
  double aliensMovCumulative = 0;

  byte aliensMaxL = 0;
  byte aliensMaxR = ALIENSX-1;
  byte aliensMaxB = ALIENSY-1;
  boolean checkL = 1;
  boolean checkWin = 1;

  byte checkExit = 0;

  byte level = 0;

  uint32_t score = 0;

  display.clearDisplay();
  
  while(true){

    //////////////////
    ///level config///
    //////////////////
    
    if (checkWin){
      //aliensSpeed++;
      level++;
      for (byte i=0; i < ALIENSX; i++){
        for (byte j=0; j < ALIENSY; j++){
          aliens[i][j] = 1+int(level/10);
        }
      }
      aliensPosX = 3;
      aliensPosY = -ALIENHEIGHT*ALIENSY-ALIENSYDISTANCE*ALIENSY;
      aliensMaxL = 0;
      bulletSpeed = 5;
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(20, 20);
      display.print("Level ");
      display.print(level);
      display.display();
      delay(2000);
    }


    //////////
    ///Exit///
    //////////

    if(digitalRead(BUTTON_L) and digitalRead(BUTTON_R)){
      checkExit++;
      if (checkExit > 40)
        return score;
    } 
    else{
      checkExit = 0;
    }


    display.clearDisplay();


    
    /////////////////
    ///ship moving///
    /////////////////
    
    if(digitalRead(BUTTON_L)){
      shipMovCumulative -= 2;
      if (shipMovCumulative > 0)
        shipMovCumulative = shipMovCumulative/4;      
    }
    if(digitalRead(BUTTON_R)){
      shipMovCumulative += 2;
      if (shipMovCumulative < 0) 
        shipMovCumulative = shipMovCumulative/4;
    }
    if((!digitalRead(BUTTON_L) and !digitalRead(BUTTON_R)) or (digitalRead(BUTTON_L) and digitalRead(BUTTON_R))){
      if (shipMovCumulative < 0) 
        shipMovCumulative = shipMovCumulative/2;
      if (shipMovCumulative > 0)
        shipMovCumulative = shipMovCumulative/2;
    }

    if (shipMovCumulative < -(SHIPSPEED))
      shipMovCumulative = -(SHIPSPEED);
    if (shipMovCumulative > (SHIPSPEED))
      shipMovCumulative = (SHIPSPEED);

    //if (abs(shipMovCumulative) > SHIPSPEED
      
    if (shipPosition+shipMovCumulative < 0){
      shipPosition = 0;
      shipMovCumulative = 0;
    }
    if (shipPosition+shipMovCumulative > 127){
      shipPosition = 127;
      shipMovCumulative = 0;
    }
     
    shipPosition += shipMovCumulative;

    ///////////////////
    ///alien borders///
    ///////////////////
    
    checkL = 1;
    checkWin = 1;
    aliensMaxB = 0;
    for (byte i=0; i < ALIENSX; i++){
      for (byte j=0; j < ALIENSY; j++){
        if (aliens[i][j] != 0){
          if (ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY+ALIENHEIGHT > 62 - SHIPHEIGHT)
            return score; //game over
          checkL = 0;
          checkWin = 0;
          aliensMaxR = i;
          if (j > aliensMaxB){
            aliensMaxB = j;
          }
        }
      }
      if (checkL)
        aliensMaxL = i + 1;
    }

    ///////////////////
    ///aliens moving///
    ///////////////////
    
    if (ALIENHEIGHT*aliensMaxB+ALIENSYDISTANCE*aliensMaxB+aliensPosY < 0)
      aliensPosY++;
    aliensMovCumulative += sqrt(0.005*level)*12;
    if(int(aliensMovCumulative/1)){
      aliensPosX += int(aliensMovCumulative/1)*(1-2*aliensDirection);
      aliensMovCumulative -= int(aliensMovCumulative/1);
      aliensAnim = !aliensAnim;
    }
    if (((ALIENWIDTH+ALIENSXDISTANCE)*aliensMaxL+aliensPosX > 1) and ((ALIENWIDTH+ALIENSXDISTANCE)*aliensMaxR+aliensPosX+ALIENWIDTH < (display.width()-1)))
      aliensDirectionIsChanged = 0;
    if(!aliensDirectionIsChanged){  
      if (((ALIENWIDTH+ALIENSXDISTANCE)*aliensMaxL+aliensPosX <= 1) or ((ALIENWIDTH+ALIENSXDISTANCE)*aliensMaxR+aliensPosX+ALIENWIDTH >= (display.width()-1))){
        aliensDirectionIsChanged = 1;
        aliensDirection = !aliensDirection;
        aliensPosY++;
      }
    }

    ////////////
    ///bullet///
    ////////////

    display.drawLine(bulletX, bulletY, bulletX, bulletY-bulletSpeed, WHITE);
    
    if (bulletY <= 1 or bulletY >= 64){
      bulletX = shipPosition;
      bulletY = 65 - SHIPHEIGHT;   
    }
    else{
      for (byte i=0; i < ALIENSX; i++){
        for (byte j=0; j < ALIENSY; j++){
          if (bulletX < ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX or bulletX > ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX+ALIENWIDTH)
            continue;
          if (bulletY > ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY and bulletY - bulletSpeed < ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY+ALIENHEIGHT){
            if (aliens[i][j] == 0)
              continue;
            aliens[i][j] -= 1;
            display.fillCircle(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX + ALIENWIDTH/2, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY+ALIENHEIGHT/2 , 5, WHITE);
            score += level;
            bulletX = shipPosition;
            bulletY = 65 - SHIPHEIGHT; 
            break;
          }
        }
      }
      bulletY -= bulletSpeed;
    }


    /////////////
    ///Drawing///
    /////////////
    
    display.drawBitmap(shipPosition - 3, 63 - SHIPHEIGHT, shipBitmap, 7, 5, WHITE);
    
    //display.drawLine(bulletX, bulletY, bulletX, bulletY+bulletSpeed, WHITE); //moved to bullets

    for (byte j=0; j < ALIENSY; j++){
      for (byte i=0; i < ALIENSX; i++){
        if (aliens[i][j] == 0)
          continue;
        //display.drawRect(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, ALIENWIDTH, ALIENHEIGHT, WHITE);
        switch(j){
          case 0:
            if ((aliensAnim+i)%2)
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien1Bitmap1, 5, 5, WHITE);
            else
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien1Bitmap2, 5, 5, WHITE);
            break;
          case 1:
            if ((aliensAnim+i)%2)
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien2Bitmap1, 5, 5, WHITE);
            else
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien2Bitmap2, 5, 5, WHITE);
            break;
          case 2:
            if ((aliensAnim+i)%2)
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien3Bitmap1, 5, 5, WHITE);
            else
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien3Bitmap2, 5, 5, WHITE);
            break;
          default:
            if ((aliensAnim+i)%2)
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien1Bitmap1, 5, 5, WHITE);
            else
              display.drawBitmap(ALIENWIDTH*i+ALIENSXDISTANCE*i+aliensPosX, ALIENHEIGHT*j+ALIENSYDISTANCE*j+aliensPosY, alien1Bitmap2, 5, 5, WHITE);
            break;
        }
      }
    }
    
    display.display();
    
  }
}


///////////
///////////
///Snake///
///////////
///////////

const unsigned char appleBitmap [] PROGMEM{
0x38, 0x08, 0x3C, 0x4E, 0x5E, 0x7E, 0x7E, 0x3C, 
};

uint32_t snakeGame(){
  #define FIELD_SIZE_X 16
  #define FIELD_SIZE_Y 8
  uint16_t field[FIELD_SIZE_X][FIELD_SIZE_Y];
  
  byte appleX = 7;
  byte appleY = 7;
  boolean isAppleWasEaten = true;

  #define SNAKE_SPEED 5
  byte snakeDirection = 0; //0 up 1 right 2 down 3 left
  byte snakePrevDirection = snakeDirection;
  uint16_t snakeSize = 2;
  byte headX = 7;
  byte headY = 8;
  
  byte tailX = headX;
  byte tailY = headY;
  byte tailPrevX = tailX;
  byte tailPrevY = tailY;
  byte tailDirection = 1;
  boolean isTailWasAnimated = true;
  
  boolean isKeyWasPressed = false;

  uint32_t score = 0;

  for (uint16_t i=0;i<FIELD_SIZE_X;i++){
    for (uint16_t j=0;j<FIELD_SIZE_Y;j++){
      field[i][j]=0;
    }
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 20);
  display.print(F("Ready?"));
  display.display();
  delay(2000);
  
  while(true){

    if (score == 125)
      return 200;
    
    snakePrevDirection = snakeDirection;

    if (isAppleWasEaten){
      while(true){
        appleX = random(FIELD_SIZE_X-1);
        appleY = random(FIELD_SIZE_Y-1);
        if (field[appleX][appleY] == 0)
          break;
      }
      isAppleWasEaten = false;
    }
    
    ///////////////////
    ///snake moving ///
    ///and collision///
    ///////////////////

    switch (snakeDirection){
      case 0:
        if (headY == 0)
          return score;
        if (field[headX][headY-1] > 1)
          return score;
        headY--;
        break;
      case 1:
        if (headX == FIELD_SIZE_X-1)
          return score;
        if (field[headX+1][headY] > 1)
          return score;
        headX++;
        break;
      case 2:
        if (headY == FIELD_SIZE_Y-1)
          return score;
        if (field[headX][headY+1] > 1)
          return score;
        headY++;
        break;
      case 3:
        if (headX == 0)
          return score;
        if (field[headX-1][headY] > 1)
          return score;
        headX--;
        break;    
    }

    

    field[headX][headY] = snakeSize+1;
    
    if ((appleX == headX) and (appleY == headY)){ //catch apple
      for (uint16_t i=0;i<FIELD_SIZE_X;i++){
        for (uint16_t j=0;j<FIELD_SIZE_Y;j++){
          if (field[i][j] != 0)
            field[i][j]++;
        }
      }  
      isAppleWasEaten = true;
      score++;
      snakeSize++;      
    }

    for (uint16_t i=0;i<FIELD_SIZE_X;i++){
      for (uint16_t j=0;j<FIELD_SIZE_Y;j++){
        if (field[i][j] != 0)
          field[i][j]--;
      }
    }

    tailPrevX = tailX;
    tailPrevY = tailY;

    if (tailY > 0)
      if (field[tailX][tailY-1] == 1){
        tailDirection = 0;
        tailY--;
      }
    if (tailX < FIELD_SIZE_X)
      if (field[tailX+1][tailY] == 1){
        tailDirection = 1;
        tailX++;
      }
    if (tailY < FIELD_SIZE_Y) 
      if (field[tailX][tailY+1] == 1){
        tailDirection = 2;
        tailY++;
      }
    if (tailX > 0)
      if (field[tailX-1][tailY] == 1){
        tailDirection = 3;
        tailX--;
      }

    /////////////////////////
    ///Drawing and control///
    /////////////////////////

    for(byte k=0;k<8;k++){
      
      if ((!isKeyWasPressed)){
        if (digitalRead(BUTTON_L)){
          isKeyWasPressed = true;
          if (snakePrevDirection == 0)
            snakeDirection = 3;
          else
            snakeDirection = snakePrevDirection - 1;
        }
        if (digitalRead(BUTTON_R)){
          isKeyWasPressed = true;
          if (snakePrevDirection == 3)
            snakeDirection = 0;
          else
            snakeDirection = snakePrevDirection + 1;
        }
      }

      if ((!digitalRead(BUTTON_L)) and (!digitalRead(BUTTON_R)))
        isKeyWasPressed = false;
  
      display.clearDisplay();   
  
      for (uint16_t i=0;i<FIELD_SIZE_X;i++){
        for (uint16_t j=0;j<FIELD_SIZE_Y;j++){
          display.drawPixel(i*8, j*8, WHITE);
          if (i == FIELD_SIZE_X-1)
            display.drawPixel(i*8+7, j*8, WHITE);
          if (j == FIELD_SIZE_Y-1)
            display.drawPixel(i*8, j*8+7, WHITE);
            
          if (field[i][j] != 0 and field[i][j] != snakeSize)
            display.fillRect(i*8, j*8, 8, 8, WHITE);       
        }
      }
      display.drawPixel(127, 63, WHITE);
      switch (snakePrevDirection){
        case 0:
          display.fillRect(headX*8, headY*8+(8-k),8,k,WHITE);
          break;
        case 1:
          display.fillRect(headX*8, headY*8,k,8,WHITE);
          break;
        case 2:
          display.fillRect(headX*8, headY*8,8,k,WHITE);
          break;
        case 3:
          display.fillRect(headX*8+(8-k), headY*8,k,8,WHITE);
          break;
      }

      if(field[tailX][tailY] == 1 and !((tailX == tailPrevX) and (tailY == tailPrevY)))
      switch (tailDirection){
        case 0:
          display.fillRect(tailX*8, tailY*8+8,8,8-k,WHITE);
          break;
        case 1:
          display.fillRect(tailX*8-8+k, tailY*8,8-k,8,WHITE);
          break;
        case 2:
          display.fillRect(tailX*8, tailY*8-8+k,8,8-k,WHITE);
          break;
        case 3:
          display.fillRect(tailX*8+8, tailY*8,8-k,8,WHITE);  
          break;
      }

      isTailWasAnimated = false;
  
      display.drawBitmap(appleX*8, appleY*8, appleBitmap, 8, 8, WHITE);
      display.display();
    }
  }
}

///////////////
///////////////
///Ping Pong///
///////////////
///////////////

uint32_t pongGame(){

  #define BOARD_WIDTH 12 - 1
  #define BOARD_CUMULATIVE_SPEEDUP 1
  #define BOARD_MAX_SPEED 2

  #define BOARD1_X 10
  #define BOARD2_X 117
  
  
  int8_t board1Position = 8;
  int8_t board2Position = 8;

  int8_t board1MovCumulative = 0;
  int8_t board2MovCumulative = 0;

  #define BALL_SPEED 4.5
  #define BALL_START_X 63
  #define BALL_START_Y 31

  double ballPosX = 64;
  double ballPosY = 32;
  double ballDistanceRemaining = BALL_SPEED;
  double ballDistanceBuff = 0;
  double ballAngle = -60;
  boolean ballDirection = 1; // 0 - left, 1 - right

  boolean isCollisionCalced = false;

  boolean isAI = true;
  boolean control2 = false;

  double buffY;
  double buffX2;
  double buffY2;

  uint32_t score1 = 0;
  uint32_t score2 = 0;


  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 40);
  display.print(F("PvAI"));
  display.setCursor(60, 20);
  display.print(F("or"));
  display.setCursor(90, 40);
  display.print(F("PvP"));
  display.display();
  delay(1000);
  while (true){
    delay(100);
    if (digitalRead(BUTTON_R)){
      isAI = false;
      break;
    }
    if  (digitalRead(BUTTON_L)){
      isAI = true;
      break;
    }
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 20);
  display.print(F("Ready?"));
  display.display();
  delay(2000);

  while(true){

    if (score1 == 30 or score2 == 30){
      return 30;
    } 

    ///////////////////////////////
    ///Ball moving and collision///
    ///////////////////////////////

    ballDistanceRemaining = BALL_SPEED;
    while(ballDistanceRemaining > 0){

      if (ballPosX < 0){
        ballPosX = BALL_START_X-30;
        ballPosY = BALL_START_Y;
        ballDistanceRemaining = 1;
        score2++;
        ballAngle = (75 + random(30));
      }
      else if (ballPosX > 127){
        ballPosX = BALL_START_X+30;
        ballPosY = BALL_START_Y;
        ballDistanceRemaining = 1;
        score1++;
        ballAngle = -1*(75 + random(30));
      }
      /*
      if ((ballPosX + (ballDistanceRemaining*sin((M_PI*ballAngle)/180)) <= BOARD1_X) and (ballPosX > BOARD1_X)){
        ballDistanceBuff = ((ballPosX - BOARD1_X)/sin((M_PI*ballAngle)/180));
        ballPosX = BOARD1_X;
        ballPosY -= (ballDistanceBuff*cos((M_PI*ballAngle)/180));
        //if (((board1MovCumulative*(ballDistanceRemaining-ballDistanceBuff)/(BALL_SPEED))+board1Position < ballPosY ) and ((board1MovCumulative*(ballDistanceRemaining-ballDistanceBuff)/(BALL_SPEED))+board1Position+BOARD_WIDTH > ballPosY))
        if (board1Position < ballPosY and board1Position+BOARD_WIDTH > ballPosY)
          ballAngle *= -1;
      }   
      */ 

      buffX2 = ballPosX + (ballDistanceBuff*sin((M_PI*ballAngle)/180));
      buffY2 = ballPosY - (ballDistanceBuff*cos((M_PI*ballAngle)/180));
      if (buffX2 <= BOARD1_X and ballPosX >= BOARD1_X){
        buffY = ((BOARD1_X - ballPosX) * (buffY2-ballPosY))/(buffX2-ballPosX)+ballPosY;
        if (buffY > board1Position and buffY < board1Position + BOARD_WIDTH and buffX2 <= BOARD1_X and ballPosX >= BOARD1_X){
          ballAngle = 45+(90*((buffY-board1Position)/double(BOARD_WIDTH)));
          ballPosX = BOARD1_X+1;
          ballPosY = buffY;
          ballDistanceRemaining = 0;
        }
      }
      if (buffX2 >= BOARD2_X and ballPosX <= BOARD2_X){
        buffY = ((BOARD2_X - ballPosX) * (buffY2-ballPosY))/(buffX2-ballPosX)+ballPosY;
        if (buffY > board2Position and buffY < board2Position + BOARD_WIDTH and buffX2 >= BOARD2_X and ballPosX <= BOARD2_X){
          ballAngle = -45-(90*((buffY-board2Position)/double(BOARD_WIDTH)));
          ballPosX = BOARD2_X-1;
          ballPosY = buffY;
          ballDistanceRemaining = 0;
        }
      }
      
      if ((ballPosY - (ballDistanceRemaining*cos((M_PI*ballAngle)/180)) > 63)){ //bottom wall
        ballDistanceBuff = -((63 - ballPosY)/cos((M_PI*ballAngle)/180));
        ballPosX += (ballDistanceBuff*sin((M_PI*ballAngle)/180));
        ballPosY -= (ballDistanceBuff*cos((M_PI*ballAngle)/180));
        ballAngle = (1-2*(ballAngle>0))*180 - ballAngle;
        ballDistanceRemaining -= ballDistanceBuff;
      }
      else if ((ballPosY - (ballDistanceRemaining*cos((M_PI*ballAngle)/180)) < 0)){ //top wall
        ballDistanceBuff = ((ballPosY)/cos((M_PI*ballAngle)/180));
        ballPosX += (ballDistanceBuff*sin((M_PI*ballAngle)/180));
        ballPosY -= (ballDistanceBuff*cos((M_PI*ballAngle)/180));
        ballAngle = (1-2*(ballAngle>0))*180 - ballAngle;
        ballDistanceRemaining -= ballDistanceBuff;
      }
      else{
        ballPosX += (ballDistanceRemaining*sin((M_PI*ballAngle)/180));
        ballPosY -= (ballDistanceRemaining*cos((M_PI*ballAngle)/180));
        ballDistanceRemaining = 0;
        ballDistanceBuff = BALL_SPEED;
      }
       //Serial.println("---");
    }   
    
    
    
    ///////////////////
    ///Boards moving///
    ///////////////////
    
    if(digitalRead(BUTTON_L)){
      board1MovCumulative -= BOARD_CUMULATIVE_SPEEDUP;
      if (board1MovCumulative > 0)
        board1MovCumulative = board1MovCumulative/4; 
    }
    else{
      board1MovCumulative += BOARD_CUMULATIVE_SPEEDUP;
      if (board1MovCumulative < 0)
        board1MovCumulative = board1MovCumulative/4; 
    }             
    if (board1MovCumulative < -(BOARD_MAX_SPEED))
      board1MovCumulative = -(BOARD_MAX_SPEED);
    if (board1MovCumulative > (BOARD_MAX_SPEED))
      board1MovCumulative = (BOARD_MAX_SPEED);
      
    if (board1Position + board1MovCumulative < 0){
      board1Position = 0;
      board1MovCumulative = 0;
    }
    if (board1Position + BOARD_WIDTH + board1MovCumulative > 63){
      board1Position = 63 - BOARD_WIDTH;
      board1MovCumulative = 0; 
    }

    if (isAI)
      control2 = (ballPosY-1+random(2)) < board2Position+3;
    else 
      control2 = digitalRead(BUTTON_R);
    
    if(control2){
      board2MovCumulative -= BOARD_CUMULATIVE_SPEEDUP;
      if (board2MovCumulative > 0)
        board2MovCumulative = board2MovCumulative/4; 
    }
    else{
      board2MovCumulative += BOARD_CUMULATIVE_SPEEDUP;
      if (board2MovCumulative < 0)
        board2MovCumulative = board2MovCumulative/4; 
    }             
    if (board2MovCumulative < -(BOARD_MAX_SPEED))
      board2MovCumulative = -(BOARD_MAX_SPEED);
    if (board2MovCumulative > (BOARD_MAX_SPEED))
      board2MovCumulative = (BOARD_MAX_SPEED);
      
    if (board2Position + board2MovCumulative < 0){
      board2Position = 0;
      board2MovCumulative = 0;
    }
    if (board2Position + BOARD_WIDTH + board2MovCumulative > 63){
      board2Position = 63 - BOARD_WIDTH;
      board2MovCumulative = 0; 
    }

    board1Position += board1MovCumulative;
    board2Position += board2MovCumulative;


  
    /////////////
    ///Drawing///
    /////////////

    display.clearDisplay();

    display.drawLine( BOARD1_X, board1Position, BOARD1_X, board1Position + BOARD_WIDTH, WHITE);
    display.drawLine( BOARD2_X, board2Position, BOARD2_X, board2Position + BOARD_WIDTH, WHITE);


    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 0);
    display.print(score1);
    display.setCursor(97, 0);
    display.print(score2);

    for (byte i = 0;i<21;i++){
      display.drawPixel(63, 3*i, WHITE);
    }


    display.drawPixel(int(ballPosX), int(ballPosY), WHITE);

    display.display();


    
  }
  return 0;
}

void greeting(){
  for (uint8_t i = 0; i<80; i += 5){
    display.fillCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
  for (uint8_t i = 0; i<80; i += 3){
    display.clearDisplay();
    display.fillRect(0,0, display.width(), display.height(), WHITE);  
    display.fillCircle(display.width()/2, display.height()/2, i, BLACK);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(34, 20);
    display.print(F("HELLO!"));
    display.display();
  }
  delay(2000);
}

void shutdown(){
  uint8_t holdtimer = 0;
  //shutdown animation
  for (uint8_t i = 0; i<80; i += 5){
    display.fillCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
  for (uint8_t i = 0; i<80; i += 3){
    display.clearDisplay();
    display.fillRect(0,0, display.width(), display.height(), WHITE);  
    display.fillCircle(display.width()/2, display.height()/2, i, BLACK);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(44, 20);
    display.print(F("BYE!"));
    display.display();
  }
  delay(2000);
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_DISPLAYOFF); //off display
  
  //shutdown and wakeup
  while(true){
    attachInterrupt(0, wakeUpHandler, HIGH);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);//enter into low power mode
    detachInterrupt(0);
    //out of low power mode
    holdtimer = 0;
    while (digitalRead(BUTTON_L) and digitalRead(BUTTON_R)){ //press LR for 2 sec to return
      delay(7);
      holdtimer++;
      if (holdtimer > 200){      
        display.ssd1306_command(SSD1306_DISPLAYON);
        delay(250); //display has some wakeup time, so we pause for smoothly startup animation
        return;
      }
    }
  }
}



void about(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 17);
  display.print(F("Created by")); 
  display.setCursor(18, 27);
  display.print(F("Dmitry Kropenko")); 
  display.setCursor(6, 47);
  display.print(F("k.dmitriu@gmail.com")); 
  display.display();
  delay(1000);
  while(true){
    delay(100);
    if (digitalRead(BUTTON_L) or digitalRead(BUTTON_R))
      return;
  }
}

void mainmenu(){
  byte menuselect = 0;
  boolean isKeyPressed = false;
  byte ssTimeout = 0;

  greeting();
       
  while(true){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    for (byte i = 0;i<3;i++){
      display.setCursor(13,i*20);
      switch(i+menuselect){
        case 1:
          display.print(F("INVADERS"));
          break;
        case 2:
          display.print(F("PONG"));
          break;
        case 3:
          display.print(F("SNAKE"));
          break;
        case 4:
          display.print(F("ABOUT"));
          break;
        case 5:
          display.print(F("SHUTDOWN"));
          break;
        default:
          display.print(F(" "));
      } 
    }
    display.setCursor(0, 20);
    display.print(F(">"));
    display.display();


    while(true){
      delay(100);
      if (digitalRead(BUTTON_R) and !isKeyPressed){
        isKeyPressed = true;
        switch(menuselect+1){
          case 1:
            gameover(spaceinvaders());
            break;
          case 2:
            gameover(pongGame());
            break;
          case 3:
            gameover(snakeGame());
            break;
          case 4:
            about();
            break;
          case 5:
            shutdown();
            menuselect = 0;
            greeting();
            break;
          default:
            display.print(F(" "));
        }
        break;
      }
      if (digitalRead(BUTTON_L) and !isKeyPressed){
        menuselect++;
        isKeyPressed = true;
        if (menuselect == 5)
          menuselect = 0;
        break;
      }
      if (!digitalRead(BUTTON_L) and !digitalRead(BUTTON_R)){
        isKeyPressed = false;
        ssTimeout++;
        if (ssTimeout > 250){
          screensaverBall();
          ssTimeout = 0;
          isKeyPressed = true;
          break;
        }
      }
    }
  }
}

void gameover(uint32_t score){
  uint8_t intlength = 1;
  uint32_t calclength = score;
  while ( calclength /= 10 )
   intlength++;
  for (uint8_t i = 0; i<80; i += 5){
    display.fillCircle(display.width()/2, display.height()/2, i, WHITE);
    //delay(10);
    display.display();
  }
  for (uint8_t i = 0; i<80; i += 3){
    display.clearDisplay();
    display.fillRect(0,0, display.width(), display.height(), WHITE);  
    display.fillCircle(display.width()/2, display.height()/2, i, BLACK);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 1);
    display.print(F("GAME OVER"));
    display.setCursor(34, 20);
    display.print(F("SCORE"));
    display.setCursor(64-6*intlength, 40);
    display.print(score);
    display.display();
  }

  while(true){
    if (digitalRead(BUTTON_L) or digitalRead(BUTTON_R))
      return;
    delay(50);
  }
  
}


void setup()   {    
  pinMode(BUTTON_L, INPUT);
  pinMode(BUTTON_R, INPUT);
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  randomSeed(analogRead(0));
}


void loop() {
  mainmenu();
}

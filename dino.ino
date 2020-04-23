#include "FastLED.h"
char m[8][8];
char dinoPos;
char dinoJump;
char dinoHealth;

char offset;

bool cactusExists;
char cactusPosX;
char cactusHeight;

bool appleExists;
char applePosX;
char applePosY;
char gameStatus;

#define LED_PIN     2 // GPIO pin for RGB LEDs.
#define NUM_LEDS    64 // Number of LEDs connected.
#define BRIGHTNESS  64 // Default LED brightness.
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
                   
char dino[12][8]={ {0,0,1,1,0,0,0,0},
                   {0,0,1,1,0,0,0,0},
                   {1,1,1,0,0,0,0,0},
                   {0,0,1,0,0,0,0,0},
                   {0,0,1,1,0,0,0,0},
                   {0,0,1,1,0,0,0,0},
                   {1,1,1,0,0,0,0,0},
                   {0,1,0,0,0,0,0,0},
                   {0,0,0,0,1,0,0,0},
                   {0,0,1,1,1,0,0,0},
                   {0,0,0,0,1,1,0,0},
                   {0,0,0,0,1,1,0,0}};

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(D7,INPUT);
  attachInterrupt(digitalPinToInterrupt(D7), ISR_button, RISING);
  dinoPos=0;
  dinoJump=0;
  offset=0;
  dinoHealth=0;
  gameStatus=0;
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();
  FastLED.show();
  randomSeed(analogRead(0));
  
  }

ICACHE_RAM_ATTR void ISR_button(){
  delayMicroseconds(50);
  if(digitalRead(D7)){   
  if ((gameStatus==1)&&(dinoJump==0)) {dinoJump=1;}
  if (gameStatus==0) {dinoHealth=3;gameStatus=1;}
  if (gameStatus==2) {gameStatus=0;}
  
  }
}

void doRandomShit(){
  switch(random(10)){
    case 5: if (!cactusExists){createCactus();} break;
    case 7: if ((!appleExists)&&(dinoHealth<5)){createApple();}break;
    default: break;
  }
}

void checkCollisions(){
  
  if (cactusExists){
    for (int i=6;i<8;i++){
      for (int j=0;j<4;j++){
        if ((m[i][j]==1)&&(cactusPosX==j)&&(dinoPos<=cactusHeight)&&(cactusExists)){
          cactusExists=false;
          dinoHealth--;
          if(dinoHealth==0){gameStatus=2;cactusExists=false;appleExists=false;} //dino dies
          
        }
      }
    }
  }
 
  if(appleExists){
    for(int i=1;i<8;i++){
      for(int j=0;j<4;j++){
        if ((m[i][j]==1)&&(applePosY==i)&&(applePosX==j)){
          dinoHealth++;
          appleExists=false;
        }
      }
    }

  }
  

}
void createCactus(){
  cactusPosX=7;
  cactusHeight=1+random(2);
  cactusExists=true;
}

void createApple(){
  applePosX=7;
  applePosY=1+random(3);
  appleExists=true;
}


void loop() {
  // put your main code here, to run repeatedly:

  delay(200);
  memset(m,0,64);
  
  if (gameStatus==0){   
    memcpy(m+4,dino,32*sizeof(char));
    
  }
  
  if (gameStatus==1){
   
  

  if ((dinoJump==1)&&(dinoPos<3)){
    dinoPos++;
  }
  if ((dinoJump==2)&&(dinoPos>0)){
    dinoPos--;
    if (dinoPos==0){dinoJump=0;}
  }
  if(dinoPos>=3) {dinoJump=2;}

    
  if ((dinoJump==0)&&(offset==0)){offset=4;}
    else if ((dinoJump==0)&&(offset==4)){offset=0;}

  if (cactusExists){
    if (cactusPosX>0){
      cactusPosX--;
    }
    else{cactusExists=false;}
  }

  if (appleExists){
    if (applePosX>0){
      applePosX--;
    }
    else {appleExists=false;}
  }
  
  doRandomShit();
  
  
  memcpy(m+4-dinoPos,dino+offset,32*sizeof(char));
  checkCollisions();
  //dinoPos+=dinoJump;
  }


  if (gameStatus==2){
    
    memcpy(m+4,dino+8,32*sizeof(char));
    
  }
  prepareFrame();
  printFrame();
}

void prepareFrame(){
  FastLED.clear();
  for (int i=0;i<dinoHealth;i++){
    m[0][7-i]=4;
  }
  if (cactusExists){
    for (int i=0;i<cactusHeight;i++){
      m[7-i][cactusPosX]=2;
    }
  }
  if (appleExists){
    m[applePosY][applePosX]=3;
  }
  for (int i=0;i<8;i++){
    for (int j=0;j<8;j++){
      switch (m[i][j]){
        case 1:   leds[8*i+j]=CRGB::Green; break;
        case 2:   leds[8*i+j]=CRGB::Red; break;
        case 3:   leds[8*i+j]=CRGB::Yellow; break;
        case 4:   leds[8*i+j]=CRGB::Blue; break;
        default:  leds[8*i+j]=CRGB::Black; break; 
      }
    }
  }
}

void printFrame(){
   FastLED.show();
}

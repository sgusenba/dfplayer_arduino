#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Bounce2.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

int current_folder = 2;
int play=0;
int firststart = 0;
int pinOffSet = 2; //adjust to your pin layout
Bounce bouncer = Bounce();
bool bts[6];


void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30


   for (int i = 0 ; i <6; i++) {
    pinMode(i+pinOffSet, INPUT_PULLUP);
    bts[i] = digitalRead(i+pinOffSet);
  }
  pinMode(12,INPUT_PULLUP);
  bouncer.attach(12);       
  play = 0;
  firststart = 0;
}

void loop()
{
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  checkbt();
}

void playFolder(int folder) {
  myDFPlayer.loopFolder(folder);
}

void checkbt() {
  for (int i = 0; i < 6; i++) {
    bool a = digitalRead(i+pinOffSet);
    if (bts[i] != a) {
      bts[i] = a;
      if (!a) {
        playFolder(i+1);
      }
    }
  }
  bouncer.update();
  bool fell = bouncer.fell();
  if(fell) {
    if(play==0) {
      play = 1;
      if(firststart==0) {
          firststart=1;
          playFolder(current_folder);
        }
        else {
          myDFPlayer.start();
        }
    }
    else {
      play=0;
      myDFPlayer.pause();
    }
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}

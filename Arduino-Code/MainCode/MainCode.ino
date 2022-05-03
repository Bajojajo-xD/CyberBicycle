// ! <-- Include required libraries --> !

#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Clcd.h>
#include <virtuabotixRTC.h>
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <IRremote.h>
void writeIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}
int readFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}
// --> Include required libraries <--

// ! <-- Settings --> !

// Run RTC-setTime to set current time into RTC

int
circMetric = 2148, // Wheel circumference (in milimeters)
speedRefreshFrequency = 500, // How often refresh speed on LCD (in miliseconds)
autoLightsRefreshFrequency = 5000; // How often refresh sensors info and change LEDs brightness and mode

char 
S7_address[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11}, // Temperature sensor address, !! RUN "TemperatureSensor-scan" to get it !!

F1_address = 0x3C,   // OLED Screen address, !! RUN "I2C-scan" to get your address !!
F2_address = 0x27;//,   // LCD Screen address, !! RUN "I2C-scan" to get your address !!

//S11_onOffBtn = , S11_menu = , 
//S11_testBtn = , S11_plusBtn = , S11_backBtn = , 
//S11_prevBtn = , S11_startStopBtn = , S11_nextBtn = , 
//S11_0btn = , S11_minusBtn = , S11_clearBtn = , 
//S11_1btn = , S11_2btn = , S11_3btn = ,
//S11_4btn = , S11_5btn = , S11_6btn = ,
//S11_7btn = , S11_8btn = , S11_9btn = ;
// IR remote buttons, !! RUN "IR-check" to get values !!

// --> Settings <--

// ! <-- Define pins --> !


int
S1 = 3,    // Button 1
S2 = 4,    // Button 2
S3 = 5,    // Button 3
S4 = 6,    // Button 4
S5 = 6,    // Light detector
S6 = 7,    // Rain/snow detector
S7_pin = 7,// Temperature sensor
S8 = 8,    // Reed switch - wheel
S9 = 9,    // Reed switch - left
S10 = 10;  // Reed switch - right
char
Y1 = A0,     // Relay
S11_pin = A1,// IR receiver
VD1 = A2;    // Voltage check

// --> Define pins <--

// ! <-- Communication-needed devices settings --> !

int
// Screen 1 (F1)
F1_width = 128,
F1_height = 64,

// Screen 2 (F2)
F2_width = 16,
F2_height = 2,

// Real-Time Clock (RTC1)
RTC1_DAT = 12,     // IO
RTC1_CLK = 13,     // SCLK
RTC1_RST = 11,     // CE

//LED 1 (H1)
H1_pin = 0,
H1_numOfLeds = 51,

//LED 2 (H2)
H2_pin = 1, 
H2_numOfLeds = 6,

//LED 3 (H3)
H3_pin = 2, 
H3_numOfLeds = 3;

// Link F1 to OLED lib
Adafruit_SSD1306 F1(F1_width, F1_height, &Wire, -1);

// Link F2 to LiquidCrystal lib
hd44780_I2Clcd F2(F2_address);    

 // Link RTC1 to VirtuabotixRTC lib  
virtuabotixRTC RTC1(RTC1_CLK, RTC1_DAT, RTC1_RST);           

// Link LEDs
Adafruit_NeoPixel H1(H1_numOfLeds, H1_pin, NEO_GRB + NEO_KHZ800); // Link H1 to NeoPixel lib
Adafruit_NeoPixel H2(H2_numOfLeds, H2_pin, NEO_GRB + NEO_KHZ800); // Link H2 to NeoPixel lib
Adafruit_NeoPixel H3(H3_numOfLeds, H3_pin, NEO_GRB + NEO_KHZ800); // Link H3 to NeoPixel lib

// Link S7 to DS18B20 lib
OneWire onewire(S7_pin);
DallasTemperature S7(&onewire);   

// Link S11 to IRremote lib
IRrecv S11(S11_pin); 
decode_results (S11_results);

// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !

  // Init F1
  F1.begin(SSD1306_SWITCHCAPVCC, F1_address); 
  F1.display();

  delay(500);
  
  // Init F2
  F2.begin(F2_width, F2_height); 
  F2.clear(); F2.backlight(); F2.setCursor(0,0); F2.print("Initializing...");

  // Init LEDs
  H1.begin(); H2.begin(); H3.begin(); 
  H1.clear(); H2.clear(); H3.clear();

  S7.begin();        // Init S7
  RTC1.updateTime(); // Init RTC1
  S11.enableIRIn();  // Enable IR receiver

  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT); pinMode(S4, INPUT); pinMode(S8, INPUT); pinMode(S9, INPUT); pinMode(S10, INPUT); // Init inputs
  pinMode(Y1, OUTPUT); // Init outputs
  digitalWrite(Y1, LOW);  // Turn off relay

  delay(1000);
  F2.setCursor(0,1); F2.print("Modules ready"); // Print F2 is ready
  F1.clearDisplay(); F1.setTextSize(2); F1.setTextColor(WHITE); F1.setCursor(20, 15); F1.print("Modules"); F1.setCursor(35, 35); F1.print("ready"); F1.display(); //Print Everything ready
  // --> Devices and pins init <--

}

// ! <-- Define useful vars --> !

float
wheelRotationStart = (3.6 * circMetric), tripDist = readFromEEPROM(10), btnHoldCounter = 0, speedToZeroCounter = 0, speedRefreshCounter = 0, autoLightsRefreshCounter = 0, toogleLeftSignalCounter = 0, toogleRightSignalCounter = 0, turnSignalAnimationCounter = 0, frontLedAnimationCounter = 0, backLedAnimationCounter = 0, mainLedAnimationCounter = 0,
avgSpeedArray[9];

int 
S1_lastState = 0, S2_lastState = 0, S3_lastState = 0, S4_lastState = 0, S8_lastState = 0, S9_lastState = 0, S10_lastState = 0, hazardLights_lastState = LOW,
chargeLevel,
lightLevel, rainLevel,
btnMode = 0, speedometerMode = 0, 
mainLedGlowType = 0, backLedGlowType = 0, frontLedGlowType = 0,
mainLedBrightness = 0, backLedBrightness = 0, frontLedBrightness = 0, xH1 = 0, xH2 = 0, xH3 = 0,
currentSpeed, maxSpeed = readFromEEPROM(20), avgSpeed = readFromEEPROM(30), avgSpeedFirstElementScale = readFromEEPROM(40), avgSpeedPushToNum = 0, totalDist = readFromEEPROM(50);

bool 
chargerActive = false,
mainLedOn = false, backLedOn = false, frontLedOn = false, 
leftSignal = false, rightSignal = false, hazardLights = false,
turnSignals = true, autoLights = true;

//Display info on F1
void F1Refresh() {
      if(btnMode = 0) {

      }
      else if(btnMode == 1) {

      }
      else if(btnMode == 2) {

      }
      else if(btnMode == 3) {

      }
      else if(btnMode == 4) {
        
      }
}
// --> Define useful vars <--

void loop() {

  // ! <-- Check accumulator voltage level --> !


  if(analogRead(VD1) * (5.0 / 1023.0) > 4.06) chargeLevel = 100;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.98) chargeLevel = 90;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.92) chargeLevel = 80;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.87) chargeLevel = 70;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.82) chargeLevel = 60;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.79) chargeLevel = 50;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.77) chargeLevel = 40;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.74) chargeLevel = 30;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.68) chargeLevel = 20;
  else if(analogRead(VD1) * (5.0 / 1023.0) > 3.45) chargeLevel = 10;
  else chargeLevel = 5;
  // --> Check accumulator voltage level <--   

  // ! <-- Update RTC --> !


  RTC1.updateTime();  
  // --> Update RTC <-- 

  // ! <-- Wheel rotate detect, calculate and refresh speed --> !


  if(digitalRead(S8) != S8_lastState) {
    S8_lastState = digitalRead(S8);
    speedToZeroCounter = millis();
    if(digitalRead(S8) == HIGH) {
      currentSpeed = (3.6 * circMetric) / (millis() - wheelRotationStart);
      if(currentSpeed > maxSpeed) maxSpeed = currentSpeed;
      wheelRotationStart = millis();
      speedToZeroCounter = 0;
      tripDist = tripDist + (circMetric / 1000);
      totalDist = int(totalDist + (circMetric / 1000));
      if(currentSpeed > 1) {
        avgSpeedArray[avgSpeedPushToNum + 1] = currentSpeed;
        avgSpeed = ((avgSpeedFirstElementScale * avgSpeedArray[0]) + avgSpeedArray[1] + avgSpeedArray[2] + avgSpeedArray[3] + avgSpeedArray[4] + avgSpeedArray[5] + avgSpeedArray[6] + avgSpeedArray[7] + avgSpeedArray[8]) / (avgSpeedFirstElementScale + 8);
        if(avgSpeedArray[9]) {
          avgSpeedArray[0] = (avgSpeed * 9) + avgSpeedArray[9] / 10;
          avgSpeedFirstElementScale = avgSpeedFirstElementScale + 8;
          avgSpeedPushToNum = 0;
          avgSpeedArray[9] = 0;
        }
      };
    };
  };
  if(millis() - speedToZeroCounter >= 2000) {
      currentSpeed = 0;
      wheelRotationStart = (3.600 * circMetric);
    }
  // --> Wheel rotate detect, calculate and refresh speed <--
  
  // ! <-- Refresh speed on lcd and send data to EEPROM --> !
  if(millis() - speedRefreshCounter >= speedRefreshFrequency) {
    speedRefreshCounter = millis();
    F2.setCursor(0,0); F2.print(currentSpeed); F2.print(" km/h"); F2.setCursor(9, 0); F2.print("| "); F2.print(int(S7.getTempCByIndex(0))); F2.print(" °C"); F2.display();
    // Store info in EEPROM
    if(readFromEEPROM(50) != totalDist) writeIntoEEPROM(50, totalDist);  
    if(readFromEEPROM(40) != avgSpeedFirstElementScale) writeIntoEEPROM(40, avgSpeedFirstElementScale);
    if(readFromEEPROM(30) != avgSpeed) writeIntoEEPROM(30, avgSpeed);
    if(readFromEEPROM(20) != maxSpeed) writeIntoEEPROM(20, maxSpeed);
    if(readFromEEPROM(10) != tripDist) writeIntoEEPROM(10, tripDist);
  };

  // --> Refresh speed on lcd and send data to EEPROM <-- 

  // ! <-- Detect S4 and change buttons mode --> !
    
  
  if(digitalRead(S4) != S4_lastState) {
    S4_lastState = digitalRead(S4);
    if(digitalRead(S4) == HIGH) {
      btnMode++;
      if(btnMode == 5) btnMode = 0;
    }
    F1Refresh();
  };
  // --> Detect S4 and change buttons mode <--

  // ! <-- Detect buttons in mode 0 / speedometer --> !
  

  if(btnMode == 0) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      btnHoldCounter = 0;
      F1Refresh();
      if(digitalRead(S1) == HIGH) {
        // Button 1 clicked, start timer
        btnHoldCounter = millis();
      };
    };
    // Reset speedometer
    if(digitalRead(S1) == HIGH && millis() - btnHoldCounter >= 3000) {
      maxSpeed = 0; tripDist = 0; avgSpeed = 0; avgSpeedFirstElementScale = 0; avgSpeedPushToNum = 0; memset(avgSpeedArray, 0, sizeof(avgSpeedArray));
    }

    //Button 2 and 3
    if(digitalRead(S2) != S2_lastState || digitalRead(S3) != S3_lastState) {
      S2_lastState = digitalRead(S2);
      S3_lastState = digitalRead(S3);
      if(digitalRead(S2) == HIGH) {
        speedometerMode++;
        if(speedometerMode == 4) speedometerMode = 0;
      }
      else if(digitalRead(S2) == HIGH) {
        speedometerMode = speedometerMode - 1;
        if(speedometerMode == -1) speedometerMode = 3;
      };
      if(speedometerMode == 0) {
        F2.setCursor(0, 1); F2.print("Trip "); F2.print(int(tripDist)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes);
      }
      else if(speedometerMode == 1) {
        F2.setCursor(0, 1); F2.print("Dist "); F2.print(int(totalDist)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes);
      }
      else if(speedometerMode == 2) {
        F2.setCursor(0, 1); F2.print("Vmax "); F2.print(int(maxSpeed)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes); 
      }
      else if(speedometerMode == 3) {
        F2.setCursor(0, 1); F2.print("AvgV "); F2.print(int(avgSpeed)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes); 
      };
      F1Refresh();
    };
  }; 
  // --> Detect buttons in mode 0 / speedometer <--

  // ! <-- Detect buttons in mode 1 / main led --> !  


  if(btnMode == 1) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      if(digitalRead(S1) == HIGH) {
        // Turn main led on/off
        if(mainLedOn) {
          mainLedOn = false; 
        }
        else mainLedBrightness = 2; mainLedGlowType = 0; mainLedOn = true; xH1 = 0; mainLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S2) == HIGH) {
        // Change main led glow type
        mainLedGlowType++; xH1 = 0; mainLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH) {
        // Change main led brightness
        mainLedBrightness++; xH1 = 0; mainLedAnimationCounter = millis();
        if(mainLedBrightness == 5) mainLedBrightness = 0;
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 1 / main led <--  

  // ! <-- Detect buttons in mode 2 / front led --> !  


  if(btnMode == 2) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      if(digitalRead(S1) == HIGH && autoLights == false) {
        // Turn front led on/off
        if(frontLedOn) {
          frontLedOn = false; 
        }
        else frontLedBrightness = 2; frontLedGlowType = 0; frontLedOn = true; xH2 = 0; frontLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S2) == HIGH && autoLights == false) {
        // Change front led glow type
        frontLedGlowType++; xH2 = 0; frontLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH && autoLights == false) {
        // Change front led brightness
        frontLedBrightness++; xH2 = 0; frontLedAnimationCounter = millis();
        if(frontLedBrightness == 5) frontLedBrightness = 0;
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 2 / front led <-- 

  // ! <-- Detect buttons in mode 3 / back led --> !  


  if(btnMode == 3) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      if(digitalRead(S1) == HIGH && autoLights == false) {
        // Turn back led on/off
        if(backLedOn) {
          backLedOn = false; 
        }
        else backLedBrightness = 2; backLedGlowType = 0; backLedOn = true; xH3 = 0; backLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S2) == HIGH && autoLights == false) {
        // Change back led glow type
        backLedGlowType++; xH3 = 0; backLedAnimationCounter = millis();
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH && autoLights == false) {
        // Change back led brightness
        backLedBrightness++; xH3 = 0; backLedAnimationCounter = millis();
        if(backLedBrightness == 5) backLedBrightness = 0;
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 3 / back led <-- 

  // ! <-- Detect buttons in mode 4 / smart functions --> !  


  if(btnMode == 4) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      if(digitalRead(S1) == HIGH) {
        // Turn turn signals and braking led on/off
        if(turnSignals) {
          turnSignals = false; 
        }
        else turnSignals = true;
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S1) == HIGH) {
        // Turn auto driving lights on/off
        if(autoLights) {
          autoLights = false; 
        }
        else autoLights = true;
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH) {
        // Turn on/off USB port
        if(chargerActive) {
          chargerActive = false; 
          digitalWrite(Y1, LOW);
        }
        else {
          chargerActive = true;
          digitalWrite(Y1, HIGH);
        };
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 4 / smart functions <-- 

  // ! <-- Toogle turn signals --> !


  if(turnSignals = true) {
    // Left signal    
    if(digitalRead(S9) != S9_lastState) {
      S9_lastState = digitalRead(S9);
      if(digitalRead(S9) == HIGH) {
        // Check if it was <400ms 2x click
        if(millis() - toogleLeftSignalCounter <= 400) {
          if(leftSignal) leftSignal = false;
          else if(!hazardLights){
            leftSignal = true; H2.clear(); H3.clear(); rightSignal = false; turnSignalAnimationCounter = millis(); xH1 = 0; xH2 = 0; xH3 = 0;
          };
        };
        toogleLeftSignalCounter = millis();
        hazardLights_lastState = LOW;
      };    
    };

    // Right signal
    if(digitalRead(S10) != S10_lastState) {
      S10_lastState = digitalRead(S10);
      if(digitalRead(S10) == HIGH) {
        // Check if it was <400ms 2x click
        if(millis() - toogleRightSignalCounter <= 400) {
          if(rightSignal) rightSignal = false;
          else if(!hazardLights) {
            H2.clear(); H3.clear(); rightSignal = true; leftSignal = false; turnSignalAnimationCounter = millis(); xH1 = 0; xH2 = 0; xH3 = 0;
          };
        };
        toogleRightSignalCounter = millis();
        hazardLights_lastState = LOW;
      };      
    };      

    // Hazard lights
    if(millis() - toogleLeftSignalCounter >= 2000 && millis() - toogleRightSignalCounter >= 2000 && digitalRead(S9) == HIGH && digitalRead(S10) == HIGH && hazardLights_lastState == LOW) {
      hazardLights_lastState = HIGH;
      if(hazardLights) hazardLights = false;
      else {
        H2.clear(); H3.clear(); hazardLights = true; rightSignal = false; leftSignal = false; turnSignalAnimationCounter = millis(); xH1 = 0; xH2 = 0; xH3 = 0;
      };
    };    
  }    
  // --> Toogle turn signals <--   

  // ! <-- IR receiver --> !




  // --> IR receiver <--

  // ! <-- Auto driving lights --> !


  if(autoLights && millis() - autoLightsRefreshCounter >= autoLightsRefreshFrequency) {
    autoLightsRefreshCounter = millis();
    lightLevel = (1023-analogRead(S5))/10.23;
    rainLevel = analogRead(S6)/10.23;
    if(lightLevel < 10) {
      frontLedOn = true; backLedOn = true; frontLedBrightness = 4; backLedBrightness = 4; frontLedGlowType = 0; backLedGlowType = 0;    
    }
    else if(lightLevel < 60 || rainLevel > 5) {
      frontLedOn = true; backLedOn = true; frontLedBrightness = 2; backLedBrightness = 2; frontLedGlowType = 1; backLedGlowType = 1;     
    }
    else {
      frontLedOn = false; backLedOn = false;
    }
  };
  // --> Auto driving lights <--   

  // ! <-- Main LED glowing types --> !


  if(mainLedOn) {
    if(mainLedGlowType == 0) {
      for(int i=0; i < H1_numOfLeds; i++) {
        H1.setPixelColor(i, H1.Color((mainLedBrightness * 50) + 55, (mainLedBrightness * 50) + 55, (mainLedBrightness * 50) + 55));
      };
    }
    else if(mainLedGlowType == 1) {
      for(int i=0; i < H1_numOfLeds; i++) {
        H1.setPixelColor(i, H1.Color((mainLedBrightness * 50) + 55, 0, 0));
      };
    }   
    else if(mainLedGlowType == 2) {
      for(int i=0; i < H1_numOfLeds; i++) {
        H1.setPixelColor(i, H1.Color(0, (mainLedBrightness * 50) + 55, 0));
      };
    }  
    else if(mainLedGlowType == 3) {
      for(int i=0; i < H1_numOfLeds; i++) {
        H1.setPixelColor(i, H1.Color(0, 0, (mainLedBrightness * 50) + 55));
      };
    }  
    else if(mainLedGlowType == 4) {
      for(int i=0; i < H1_numOfLeds; i++) {
        H1.setPixelColor(i, H1.Color((mainLedBrightness * 50) + 55, (mainLedBrightness * 50) + 55, 0));
      };
    } 
    H1.show();
  }
  else H1.clear();
  // --> Main LED glowing types <--   

  // ! <-- Turn signals glowing types --> !
  
  
  // Hazard lights
  if(hazardLights || leftSignal || rightSignal) {
    frontLedAnimationCounter = 0; backLedAnimationCounter = 0;
  }    
  if(hazardLights) {    
    if(millis() - turnSignalAnimationCounter < 200) {
      H2.setPixelColor(3, H2.Color(255, 165, 0)); H2.setPixelColor(2, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(1, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 400) {
      H2.setPixelColor(4, H2.Color(255, 165, 0)); H2.setPixelColor(1, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(0, H3.Color(255, 165, 0)); H3.setPixelColor(2, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 600) {
      H2.setPixelColor(5, H2.Color(255, 165, 0)); H2.setPixelColor(0, H2.Color(255, 165, 0)); H2.show(); H3.clear();
    }
    else if(millis() - turnSignalAnimationCounter < 700) {
      H3.setPixelColor(0, H3.Color(255, 165, 0)); H3.setPixelColor(2, H3.Color(255, 165, 1)); H3.setPixelColor(2, H3.Color(255, 165, 3)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 800) {
      H2.clear(); H3.clear(); turnSignalAnimationCounter = millis();
    };
  }
  // Left signal
  else if(leftSignal) {
    if(millis() - turnSignalAnimationCounter < 200) {
      H2.setPixelColor(3, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(0, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 400) {
      H2.setPixelColor(4, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(1, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 600) {
      H2.setPixelColor(5, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(2, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 800) {
      H2.clear(); H3.clear(); turnSignalAnimationCounter = millis();
    };
  }
  // Right signal
  else if(rightSignal) {
    if(millis() - turnSignalAnimationCounter < 200) {
      H2.setPixelColor(2, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(2, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 400) {
      H2.setPixelColor(1, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(1, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 600) {
      H2.setPixelColor(0, H2.Color(255, 165, 0)); H2.show(); H3.setPixelColor(0, H3.Color(255, 165, 0)); H3.show();
    }
    else if(millis() - turnSignalAnimationCounter < 800) {
      H2.clear(); H3.clear(); turnSignalAnimationCounter = millis();
    };
  };
  // --> Turn signals glowing types <-- 


  // ! <-- Front LED glowing types --> !
  

  if(frontLedOn && !rightSignal && !leftSignal && !hazardLights) {
    // Animation 1
    if(frontLedGlowType == 0) {
      for(int i=0; i < H2_numOfLeds; i++) {
        H2.setPixelColor(i, H2.Color((frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55));
      };
    }
    // Animation 2
    else if (frontLedGlowType == 1) {
      // Usage of x example

      // if(millis() - frontLedAnimationCounter >= (xH2 * 500)) {
      //   for(int i=0; i < H2_numOfLeds; i++) {
      //     H2.setPixelColor(i, H2.Color(((frontLedBrightness * 50) + 55) * xH2, ((frontLedBrightness * 50) + 55) * xH2, ((frontLedBrightness * 50) + 55) * xH2));
      //   };
      //   xH2++;
      //   if(xH2 == 2) xH2 = 0; frontLedAnimationCounter = millis();
      // }

      // Same but uses less program memory
      if(millis() - frontLedAnimationCounter >= 500 && millis() - frontLedAnimationCounter <= 1000) {
        for(int i=0; i < H2_numOfLeds; i++) {
           H2.setPixelColor(i, H2.Color((frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55));
        }
      }
      if(millis() - frontLedAnimationCounter >= 1000) {
        for(int i=0; i < H2_numOfLeds; i++) {
           H2.setPixelColor(i, H2.Color(0, 0, 0));
        }
        frontLedAnimationCounter = millis();
      }
    }
    else if (frontLedGlowType == 2) {
      if(xH2 < H2_numOfLeds*2) {
        if(millis() - frontLedAnimationCounter > xH2 * 100) {
          if(xH2 < H2_numOfLeds) {
            H2.setPixelColor(xH2, H2.Color((frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55));
          } 
          else {
            H2.setPixelColor(xH2 - H2_numOfLeds, H2.Color(0, 0, 0));
          };
          xH2++;
        };
      }
      else {
        if(millis() - frontLedAnimationCounter > (xH2 - H2_numOfLeds*2) * 100) {
          if((xH2 - H2_numOfLeds*2) < H2_numOfLeds) {
            H2.setPixelColor(H2_numOfLeds - (xH2 - H2_numOfLeds*2), H2.Color((frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55, (frontLedBrightness * 50) + 55));
          } 
          else {
            H2.setPixelColor(H2_numOfLeds - (xH2 - H2_numOfLeds*2) - H2_numOfLeds, H2.Color(0, 0, 0));
          };
          xH2++;
          if(xH2 == H2_numOfLeds*4) {
            xH2 = 0;
            frontLedAnimationCounter = millis();            
          };
        }
      }
    };
    H2.show();
  }
  else if(!hazardLights && !leftSignal && !rightSignal) H2.clear();

  // --> Front LED glowing types <-- 

  // ! <-- Back LED glowing types --> !
  
  
  if(backLedOn && !rightSignal && !leftSignal && !hazardLights) {
    // Animation 1    
    if(backLedGlowType == 0) {
      for(int i=0; i < H3_numOfLeds; i++) {
        H3.setPixelColor(i, H3.Color((frontLedBrightness * 50) + 55, 0, 0));
      };
    }
    // Animation 2    
    else if (backLedGlowType == 1) {
      if(millis() - backLedAnimationCounter >= 500 && millis() - backLedAnimationCounter < 1000) {
        for(int i=0; i < H3_numOfLeds; i++) {
           H3.setPixelColor(i, H3.Color((backLedBrightness * 50) + 55, 0, 0));
        }
      };
      if(millis() - backLedAnimationCounter >= 1000) {
        for(int i=0; i < H3_numOfLeds; i++) {
           H3.setPixelColor(i, H3.Color(0, 0, 0));
        }
        backLedAnimationCounter = millis();
      };
    }
    // Animation 3
    else if (backLedGlowType == 2) {
      if(millis() - backLedAnimationCounter >= 300 && millis() - backLedAnimationCounter < 400) {
        H3.setPixelColor(H3_numOfLeds * xH3, H3.Color((backLedBrightness * 50) + 55, 0, 0));
      };
      if(millis() - backLedAnimationCounter >= 400 && millis() - backLedAnimationCounter < 500) {
        H3.setPixelColor(H3_numOfLeds * xH3, H3.Color(0, 0, 0));
      };
      if(millis() - backLedAnimationCounter >= 500 && millis() - backLedAnimationCounter < 600) {
        H3.setPixelColor(H3_numOfLeds * xH3, H3.Color((backLedBrightness * 50) + 55, 0, 0));
      };
      if(millis() - backLedAnimationCounter >= 600) {
        H3.setPixelColor(H3_numOfLeds * xH3, H3.Color(0, 0, 0));
        xH3++; 
        if(xH3 == 2) xH3 = 0;
        backLedAnimationCounter = millis();
      };
    }
    H3.show();
  }
  else if(!hazardLights && !leftSignal && !rightSignal) H3.clear();

  // --> Front LED glowing types <-- 

}
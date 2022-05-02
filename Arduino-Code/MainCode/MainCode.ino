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
speedRefreshFrequency = 500; // How often refresh speed on LCD (in miliseconds)

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


  F1.begin(SSD1306_SWITCHCAPVCC, F1_address); // Init F1
  F1.display(); // Print F1 is ready

  delay(500);
  
  F2.begin(F2_width, F2_height); // Init F2
  F2.clear(); F2.backlight(); F2.setCursor(0,0); F2.print("Initializing...");

  H1.begin(); H2.begin(); H3.begin(); // Init LEDs

  S7.begin(); // Init S7
  RTC1.updateTime(); // Init RTC1
  S11.enableIRIn();    // Enable IR receiver

  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT); pinMode(S4, INPUT); pinMode(S8, INPUT); pinMode(S9, INPUT); pinMode(S10, INPUT); // Init inputs
  pinMode(Y1, OUTPUT); // Init outputs
  digitalWrite(Y1, LOW);  // Turn off relay

  delay(1000);
  F2.setCursor(0,1); F2.print("Modules ready"); // Print F2 is ready
  F1.clearDisplay(); F1.setTextSize(2); F1.setTextColor(WHITE); F1.setCursor(15, 15); F1.println("Modules"); F1.setCursor(30, 35); F1.print("ready"); F1.display(); //Print Everything ready
  // --> Devices and pins init <--

}

// ! <-- Define useful vars --> !

float
wheelRotationStart = (3.600 * circMetric), tripDist = 0, btnHoldCounter = 0, speedToZeroCounter = 0, refreshSpeedCounter = 0;

int 
S1_lastState = 0, S2_lastState = 0, S3_lastState = 0, S4_lastState = 0, S8_lastState = 0, 
btnMode = 0, speedometerMode = 0, 
mainLedGlowType = 0, backLedGlowType = 0, frontLedGlowType = 0,
mainLedBrightness = 0, backLedBrightness = 0, frontLedBrightness = 0,
currentSpeed, maxSpeed, totalDist = readFromEEPROM(1);

bool 
chargerActive = false, mainLedOn = false, backLedOn = false, frontLedOn = false, turnSignals = true, autoLights = true;

//Display info on F1
void F1Refresh() {
      if (btnMode = 0) {

      }
      else if (btnMode == 1) {

      }
      else if (btnMode == 2) {

      }
      else if (btnMode == 3) {

      }
      else if (btnMode == 4) {
        
      }
}
// --> Define useful vars <--

void loop() {
  RTC1.updateTime();  

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
    };
    if(millis() - speedToZeroCounter == 2000) {
      currentSpeed = 0;
      wheelRotationStart = (3.600 * circMetric);
    }
  };
  // --> Wheel rotate detect, calculate and refresh speed <--
  
  // ! <-- Refresh speed on lcd and send data to EEPROM --> !
  if(millis() - refreshSpeedCounter >= speedRefreshFrequency) {
    refreshSpeedCounter = millis();
    F2.setCursor(0,0); F2.print(currentSpeed); F2.print(" km/h"); F2.setCursor(9, 0); F2.print("| "); F2.print(int(S7.getTempCByIndex(0))); F2.print(" °C"); F2.display();
    // Send total dist into EEPROM
    writeIntoEEPROM(1, totalDist);  
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
      maxSpeed = 0; tripDist = 0;
    }

    //Button 2 and 3
    if(digitalRead(S2) != S2_lastState || digitalRead(S3) != S3_lastState) {
      S2_lastState = digitalRead(S2);
      S3_lastState = digitalRead(S3);
      if(digitalRead(S2) == HIGH) {
        speedometerMode++;
        if(speedometerMode == 3) speedometerMode = 0;
      }
      else if(digitalRead(S2) == HIGH) {
        speedometerMode = speedometerMode - 1;
        if(speedometerMode == -1) speedometerMode = 2;
      };
      refreshSpeedCounter = 0;
      if(speedometerMode == 0) {
        F2.setCursor(0, 1); F2.print("Trip "); F2.print(int(tripDist)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes);
      }
      else if (speedometerMode == 1) {
        F2.setCursor(0, 1); F2.print("Dist "); F2.print(int(totalDist)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes);
      }
      else if (speedometerMode == 2) {
        F2.setCursor(0, 1); F2.print("Vmax "); F2.print(int(maxSpeed)); F2.setCursor(9, 1); F2.print("| "); F2.print(RTC1.hours); F2.print(":"); F2.print(RTC1.minutes); 
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
        else mainLedBrightness = 2; mainLedGlowType = 0; mainLedOn = true;
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S1) == HIGH) {
        // Change main led glow type
        mainLedGlowType++;
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH) {
        // Change main led brightness
        mainLedBrightness++;
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
      if(digitalRead(S1) == HIGH) {
        // Turn front led on/off
        if(frontLedOn) {
          frontLedOn = false; 
        }
        else frontLedBrightness = 2; frontLedGlowType = 0; frontLedOn = true;
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S1) == HIGH) {
        // Change front led glow type
        frontLedGlowType++;
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH) {
        // Change front led brightness
        frontLedBrightness++;
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 2 / front led <-- 

  // ! <-- Detect buttons in mode 3 / back led --> !  


  if(btnMode == 2) {
    // Button 1
    if(digitalRead(S1) != S1_lastState) {
      S1_lastState = digitalRead(S1);
      if(digitalRead(S1) == HIGH) {
        // Turn back led on/off
        if(backLedOn) {
          backLedOn = false; 
        }
        else backLedBrightness = 2; backLedGlowType = 0; backLedOn = true;
      };
      F1Refresh();
    };

    // Button 2
    if(digitalRead(S2) != S2_lastState) {
      S2_lastState = digitalRead(S2);
      if(digitalRead(S1) == HIGH) {
        // Change back led glow type
        backLedGlowType++;
      };
      F1Refresh();
    };

    // Button 3
    if(digitalRead(S3) != S3_lastState) {
      S3_lastState = digitalRead(S3);
      if(digitalRead(S3) == HIGH) {
        // Change back led brightness
        backLedBrightness++;
      };
      F1Refresh();
    };
  };
  // --> Detect buttons in mode 3 / back led <-- 

  // ! <-- Detect buttons in mode 4 / smart functions --> !  


  if(btnMode == 2) {
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


}
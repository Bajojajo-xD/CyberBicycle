// ! <-- Include required libraries --> !
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Clcd.h>

// --> Include required libraries <--

// ! <-- Settings --> !

byte 
F2_address = 0x27;   // LCD Screen address, !! RUN "I2C-scan" to get your address !!

// --> Settings <--

// ! <-- Communication-needed devices settings --> !

int
// Screen 2 (F2)
F2_width = 16,
F2_height = 2;

// Link F2 to hd44780 lib
hd44780_I2Clcd F2(F2_address);          
// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !


  F2.begin(F2_width, F2_height); // Init F2
  // --> Devices and pins init <--

}

void loop() {
  F2.clear(); F2.backlight(); F2.setCursor(0,0); F2.print("Probably"); F2.setCursor(0,1); F2.print("working");
  delay(2000);
  F2.clear(); F2.setCursor(0,0); F2.print("Refreshing in:"); F2.setCursor(0,1); F2.print("3...");
  delay(1000);
  F2.setCursor(0,1); F2.print("2");
  delay(1000);
  F2.setCursor(0,1); F2.print("1");
  delay(1000);
}

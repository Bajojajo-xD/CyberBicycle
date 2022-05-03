// ! <-- Include required libraries --> !
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --> Include required libraries <--

// ! <-- Settings --> !

byte 
F1_address = 0x3C;   // OLED Screen address, !! RUN "I2C-scan" to get your address !!

// --> Settings <--

// ! <-- Communication-needed devices settings --> !

int
// Screen 1 (F1)
F1_width = 128,
F1_height = 64;

// Link F1 to OLED lib
Adafruit_SSD1306 F1(F1_width, F1_height, &Wire, -1);           
// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !


  F1.begin(SSD1306_SWITCHCAPVCC, F1_address); // Init F1
  F1.clearDisplay(); F1.setTextSize(2); F1.setTextColor(WHITE); F1.setCursor(20, 15); F1.println("Modules"); F1.setCursor(35, 35); F1.println("ready"); F1.display(); //Print Everything ready
  // --> Devices and pins init <--

}

void loop() {

}

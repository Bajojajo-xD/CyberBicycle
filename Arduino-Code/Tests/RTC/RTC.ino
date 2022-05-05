// ! <-- Include required libraries --> !
#include <virtuabotixRTC.h>
// --> Include required libraries <--

// ! <-- Communication-needed devices settings --> !


// !! Run Helpers/RTC-setTime to set current time into RTC !!



int
// Real-Time Clock (RTC1)
RTC1_DAT = 12,     // IO
RTC1_CLK = 13,     // SCLK
RTC1_RST = 11;     // CE

// Link RTC1 to VirtuabotixRTC lib  
virtuabotixRTC RTC1(RTC1_CLK, RTC1_DAT, RTC1_RST);      
// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !

  Serial.begin(9600); // Open serial

  // --> Devices and pins init <--

}

void loop() {
  RTC1.updateTime(); // Init RTC1

  Serial.print(RTC1.hours); Serial.print(":"); Serial.print(RTC1.minutes); Serial.print(":"); Serial.print(RTC1.seconds); Serial.print("\n");

  delay(1000);
}

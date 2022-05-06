// ! <-- Include required libraries --> !

#include <DallasTemperature.h>
#include <OneWire.h>
// --> Include required libraries <--

// ! <-- Settings --> !

int
S7_pin = 7; // Temperature sensor
// --> Settings <--

// ! <-- Communication-needed devices settings --> !

// Link S7 to DS18B20 lib
OneWire onewire(S7_pin);
DallasTemperature S7(&onewire);   
// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !

  Serial.begin(9600);
  S7.begin();   // Init S7 (Temp sensor)
  S7.setResolution(12);
  // --> Devices and pins init <--

}

void loop() {
  S7.requestTemperatures();
  Serial.print(int(S7.getTempCByIndex(0))); Serial.print("ºC\n");
  delay(1000);
}

#include <IRremote.h>

#define RECV_PIN A1

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    if(results.value != 4294967295) Serial.println(results.value);
    irrecv.resume(); // Receive the next value
  }
}
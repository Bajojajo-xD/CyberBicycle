// ! <-- Settings --> !

char
S5 = A6;    // Light detector
// --> Settings <--

void setup() {
  Serial.begin(9600);
}

void loop() {
  float lightLevel = (1023-analogRead(S5))/10.23;
  Serial.print(lightLevel); Serial.print("%\n");
  delay(1000);
}

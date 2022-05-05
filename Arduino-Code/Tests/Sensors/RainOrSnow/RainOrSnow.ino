// ! <-- Settings --> !

char
S6 = A7;   // Rain/snow detector
// --> Settings <--

void setup() {
  Serial.begin(9600);
}

void loop() {
  float rainlevel = (1023-analogRead(S6))/10.23;
  Serial.print(rainlevel); Serial.print("%\n");
  delay(1000);
}

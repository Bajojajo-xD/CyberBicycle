// ! <-- Settings --> !

char
S6 = A7;   // Rain/snow detector
// --> Settings <--

void setup() {
  Serial.begin(9600);
}

void loop() {
  char rainlevel = analogRead(S6)/10.23;
  Serial.print(rainlevel); Serial.print("%");
  delay(1000);
}

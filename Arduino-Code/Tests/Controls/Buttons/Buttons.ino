// ! <-- Settings --> !

int
S1 = 3,    // Button 1
S2 = 4,    // Button 2
S3 = 5,    // Button 3
S4 = 6,    // Button 4

S1_lastState = 0, S2_lastState = 0, S3_lastState = 0, S4_lastState = 0;
// --> Settings <--

void setup() {
  pinMode(S1, INPUT_PULLUP); pinMode(S2, INPUT_PULLUP); pinMode(S3, INPUT_PULLUP); pinMode(S4, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(S1) != S1_lastState) {
    S1_lastState = digitalRead(S1);
    if(digitalRead(S1) != HIGH) {
      Serial.print("S1 (red) clicked!\n");
    };
  };
  if(digitalRead(S2) != S2_lastState) {
    S2_lastState = digitalRead(S2);
    if(digitalRead(S2) != HIGH) {
      Serial.print("S2 (yellow) clicked!\n");
    };
  };
  if(digitalRead(S3) != S3_lastState) {
    S3_lastState = digitalRead(S3);
    if(digitalRead(S3) != HIGH) {
      Serial.print("S3 (yellow) clicked!\n");
    };
  };
  if(digitalRead(S4) != S4_lastState) {
    S4_lastState = digitalRead(S4);
    if(digitalRead(S4) != HIGH) {
      Serial.print("S4 (green) clicked!\n");
    };
  };
}

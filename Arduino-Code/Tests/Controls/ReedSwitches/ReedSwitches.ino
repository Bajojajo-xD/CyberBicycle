// ! <-- Settings --> !

int
S8 = 8,    // Reed switch - wheel
S9 = 9,    // Reed switch - left
S10 = 10,  // Reed switch - right

S8_lastState = 0, S9_lastState = 0, S10_lastState = 0;
// --> Settings <--

void setup() {
  pinMode(S8, INPUT_PULLUP); pinMode(S9, INPUT_PULLUP); pinMode(S10, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(S8) != S8_lastState) {
    S8_lastState = digitalRead(S8);
    if(digitalRead(S8) != HIGH) {
      Serial.print("Reed switch S8 detected!\n");
    };
  };
  if(digitalRead(S9) != S9_lastState) {
    S9_lastState = digitalRead(S9);
    if(digitalRead(S9) != HIGH) {
      Serial.print("Reed switch S9 detected!\n");
    };
  };
  if(digitalRead(S10) != S10_lastState) {
    S10_lastState = digitalRead(S10);
    if(digitalRead(S10) != HIGH) {
      Serial.print("Reed switch S10 detected!\n");
    };
  };
}

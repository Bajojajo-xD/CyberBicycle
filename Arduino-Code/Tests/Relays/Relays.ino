#define Y1 A0

void setup()
{
  pinMode(Y1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Y1, LOW); 
  Serial.print("Relay off");
  delay(1000);
  digitalWrite(Y1, HIGH); 
  Serial.print("Relay on");  
  delay(1000);
}
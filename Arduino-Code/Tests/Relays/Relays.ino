int Y1 = A0;

void setup()
{
  pinMode(Y1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Y1, HIGH); 
  Serial.print("Relay on");  
  Serial.print("Relay on\n");
  delay(5000);
  digitalWrite(Y1, LOW); 
  Serial.print("Relay off\n");  
  delay(5000);
}
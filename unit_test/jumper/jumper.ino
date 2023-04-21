

int sensorValue;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = digitalRead(A0);
  Serial.println(sensorValue);
  delay(100);
}

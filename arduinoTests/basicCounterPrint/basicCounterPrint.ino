void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);

}

int counter = 0;

void loop() {
  Serial.print(counter);
  Serial.println("Hello!");
  
  Serial3.print(counter);
  Serial3.println("Hello!");
  counter++;

  delay(100);

}

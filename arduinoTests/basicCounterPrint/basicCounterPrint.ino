
#define CH1_PIN A0
#define CH2_PIN A1
#define CH3_PIN A2

void setup() {
  Serial.begin(115200);
}

int counter = 0;
char sendPacket[75];

void loop() {

  int ch1Val = analogRead(CH1_PIN);
  int ch2Val = analogRead(CH2_PIN);
  int ch3Val = analogRead(CH3_PIN);

  sprintf(sendPacket, "%d,%d,%d", ch1Val, ch2Val, ch3Val);
  Serial.println(sendPacket);

  delay(90);

}

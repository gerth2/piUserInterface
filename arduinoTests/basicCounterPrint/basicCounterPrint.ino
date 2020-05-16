
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
  int ch2Val = (int)((counter & 0x02) >> 1) * 1024; //Square wave every 2 samples
  int ch3Val = counter; // slope-1 ramp

  counter = (counter + 1) % 1024;

  sprintf(sendPacket, "%d,%d,%d", ch1Val, ch2Val, ch3Val);
  Serial.println(sendPacket);

  //delay(10);

}

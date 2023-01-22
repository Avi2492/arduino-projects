
void setup() {
  // put your setup code here, to run once:
   pinMode(2, OUTPUT);
   pinMode(3, INPUT);
   Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(2, LOW);
  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);
  int T=pulseIn(3, HIGH);
  float D=0.034*T/2;
  Serial.println(D);
}

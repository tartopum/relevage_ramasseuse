const byte PIN = ;

void setup() {
  Serial.begin(9600);

  pinMode(PIN, INPUT);
}

void loop() {
  Serial.print("Valeur : ");
  Serial.println(analogRead(PIN));
}

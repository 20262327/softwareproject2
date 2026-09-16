#define PIN_LED 7
int toogle;

void setup(){
  pinMode(PIN_LED, OUTPUT);
}

void loop(){
  digitalWrite(PIN_LED, 1);
  delay(1000);
  toogle = !toogle;
  for(toogle=0; toogle<5; toogle++){
  digitalWrite(PIN_LED, 1);
  delay(100);
  digitalWrite(PIN_LED, 0);
  delay(100);
  }
  
  digitalWrite(PIN_LED, 0);

while(1){}
}

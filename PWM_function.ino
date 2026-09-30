#define PIN_LED 7

int g_period = 10000;

void set_period(int period) {
  g_period = period;
}

void set_duty(int duty) {
  int on_time = (long)g_period * duty / 100;
  int off_time = g_period - on_time;

  if (on_time > 0) {
    digitalWrite(PIN_LED, HIGH);
    delayMicroseconds(on_time);
  }
  if (off_time > 0) {
    digitalWrite(PIN_LED, LOW);
    delayMicroseconds(off_time);
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  set_period(10000); 
}

void loop() {
    for (int d = 0; d <= 100; d += 2) { 
    set_duty(d);
  }

  for (int d = 100; d >= 0; d -= 2) {
    set_duty(d);
  }
}

#include <Servo.h>

#define PIN_LED      9
#define PIN_SERVO    10
#define PIN_TRIG     12
#define PIN_ECHO     13

#define SND_VEL      346.0
#define INTERVAL     25        
#define PULSE_DURATION 10
#define TIMEOUT      ((INTERVAL / 2) * 1000.0)
#define SCALE        (0.001 * 0.5 * SND_VEL)

#define _DIST_MIN    100.0    
#define _DIST_MAX    400.0    
#define _EMA_ALPHA   0.3      

#define _TARGET_DIST 250.0

#define _ANGLE_CLOSED 0
#define _ANGLE_OPEN   90
#define MOVING_TIME   2000.0  

#define CONTROL_MODE 1  

Servo myServo;

unsigned long last_sampling_time;
unsigned long moveStartTime;

float dist_ema = _DIST_MAX;
float dist_prev = _DIST_MAX;

int startAngle = _ANGLE_CLOSED;
int targetAngle = _ANGLE_CLOSED;
int currentAngle = _ANGLE_CLOSED;

bool isMoving = false;

float getSigmoidProgress(float t) {
  float z = (t - 0.5) * 12.0;
  return 1.0 / (1.0 + exp(-z));
}

float getEaseInOutProgress(float t) {
  return (1.0 - cos(t * PI)) / 2.0;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myServo.attach(PIN_SERVO);
  myServo.write(_ANGLE_CLOSED);

  Serial.begin(57600);
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL)
    return;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  float dist_filtered;

  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
    dist_filtered = dist_prev;
  } else if (dist_raw < _DIST_MIN) {
    dist_filtered = dist_prev;
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  dist_ema = _EMA_ALPHA * dist_filtered + (1.0 - _EMA_ALPHA) * dist_ema;

  bool vehicleDetected = (dist_ema <= _TARGET_DIST);

  if (vehicleDetected && targetAngle != _ANGLE_OPEN) {
    startAngle = currentAngle;
    targetAngle = _ANGLE_OPEN;
    moveStartTime = millis();
    isMoving = true;
  } else if (!vehicleDetected && targetAngle != _ANGLE_CLOSED) {
    startAngle = currentAngle;
    targetAngle = _ANGLE_CLOSED;
    moveStartTime = millis();
    isMoving = true;
  }

  if (isMoving) {
    float elapsedTime = millis() - moveStartTime;
    float t = elapsedTime / MOVING_TIME;

    if (t >= 1.0) {
      t = 1.0;
      isMoving = false;
    }

    float factor = 0.0;
    if (CONTROL_MODE == 1) {
      factor = getSigmoidProgress(t);     
    } else {
      factor = getEaseInOutProgress(t);   
    }

    currentAngle = startAngle + (targetAngle - startAngle) * factor;
    myServo.write(currentAngle);          
  }

  if (currentAngle > 0) {
    digitalWrite(PIN_LED, 0); // LED ON
  } else {
    digitalWrite(PIN_LED, 1); // LED OFF
  }

  Serial.print("Min:");       Serial.print(_DIST_MIN);
  Serial.print(",dist:");      Serial.print(min(dist_ema, _DIST_MAX));
  Serial.print(",Angle:");     Serial.print(currentAngle);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}

#include <Adafruit_NeoPixel.h> 

#define IN1 36
#define IN2 35
#define IN3 48
#define IN4 47
#define MTRSPD_A 37
#define MTRSPD_B 21

#define TRIG 7
#define ECHO 6

Adafruit_NeoPixel pixels(1, 38, NEO_GRB + NEO_KHZ800);

float duration = 0;
float distance = 0;

void setup() {
  Serial.begin(115200);
  pixels.begin();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  pinMode(MTRSPD_A, OUTPUT);
  pinMode(MTRSPD_B, OUTPUT);

  ledcAttach(MTRSPD_A, 20000, 10);
  ledcAttach(MTRSPD_B, 20000, 10);

  ledcWrite(MTRSPD_A, 675);
  ledcWrite(MTRSPD_B, 700);
}

void loop() {
  pixels.clear();

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = (duration * 0.0343) / 2;

  Serial.println(distance);

  if (distance <= 15) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
  }

  delay(100);
}
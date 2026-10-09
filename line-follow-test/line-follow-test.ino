// S3 mac address - 94:a9:90:dd:94:24
// C mac address - f4:65:0b:47:19:ec

#include <esp_now.h>
#include <WiFi.h>

#define IN1 2
#define IN2 38
#define IN3 48
#define IN4 47
#define MTRSPD_A 1
#define MTRSPD_B 21

#define IR1 14
#define IR2 13
#define IR3 12
#define IR4 11
#define IR5 10

typedef struct {
  int power;
  float Kp;
  float Ki;
  float Kd;
  float weight_low;
  float weight_high;
  unsigned int min_spd;
  unsigned int base_spd;
  unsigned int max_spd;
  unsigned int icap;
} dataPacket;

dataPacket packet = {
  0,
  1,
  0,
  0,
  1,
  2,
  0,
  512,
  1023,
  200
};

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&packet, incomingData, sizeof(dataPacket));
}

float readSensors() {
  int ir1 = !digitalRead(IR1);
  int ir2 = !digitalRead(IR2);
  int ir4 = !digitalRead(IR4);
  int ir5 = !digitalRead(IR5);

  return (-packet.weight_high * ir1) + (-packet.weight_low * ir2) + (packet.weight_low * ir4) + (packet.weight_high * ir5);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  pinMode(MTRSPD_A, OUTPUT);
  pinMode(MTRSPD_B, OUTPUT);

  ledcAttach(MTRSPD_A, 20000, 10);
  ledcAttach(MTRSPD_B, 20000, 10);

  ledcWrite(MTRSPD_A, packet.base_spd);
  ledcWrite(MTRSPD_B, packet.base_spd);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
}

//josiah waz here

float integral = 0;
float last_error = 0;

int last_power = 0;

void loop() {
  if (packet.power && !last_power) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    ledcWrite(MTRSPD_A, 700);
    ledcWrite(MTRSPD_B, 700);

    last_power = 1;

    delay(100);
  } else if (!packet.power) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);

    last_power = 0;

    return;
  }

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  float error = readSensors();

  integral = constrain(integral + error, -packet.icap, packet.icap);

  float derivitive = error - last_error;

  int correction = (packet.Kp * error) + (packet.Ki * integral) + (packet.Kd * derivitive);

  last_error = error;

  ledcWrite(MTRSPD_A, constrain(packet.base_spd - correction, packet.min_spd, packet.max_spd));
  ledcWrite(MTRSPD_B, constrain(packet.base_spd + correction, packet.min_spd, packet.max_spd));

  if (error == -packet.weight_high - packet.weight_low) {
    ledcWrite(MTRSPD_B, 0);
  } else if (error == packet.weight_high + packet.weight_low) {
    ledcWrite(MTRSPD_A, 0);
  }
}
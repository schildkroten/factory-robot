// S3 mac address - 94:a9:90:dd:94:24
// C mac address - f4:65:0b:47:19:ec

#include <esp_now.h>
#include <WiFi.h>

const uint8_t broadcastAddress[] = {0x94, 0xA9, 0x90, 0xDD, 0x94, 0x24};

esp_now_peer_info_t peerInfo;

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

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {}

float readFloat() {
  while (Serial.available() == 0) {}
  float float_recived = Serial.parseFloat();
  while (Serial.available() > 0) {
    Serial.read();
  }

  return float_recived;
}
 
void setup() {
  Serial.begin(115200);

  pinMode(34, INPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

#define MAX_BUFF_LEN 128
char in_buff[MAX_BUFF_LEN];
unsigned int buff_index;

void loop() {
  dataPacket last_packet = packet;

  packet.power = digitalRead(34);

  char cmd[32];
  float arg = 0;

  while (Serial.available() > 0) {
    char recv_byte = Serial.read();

    if (recv_byte == '\n' || recv_byte == '\r') {
      if (buff_index > 0) {
        in_buff[buff_index] = '\0';

        int num_parsed = sscanf(in_buff, "%s %f", cmd, &arg);
        if (num_parsed == 2) {
          if (strcmp(cmd, "Kp") == 0) {
            packet.Kp = arg;
          }

          if (strcmp(cmd, "Ki") == 0) {
            packet.Ki = arg;
          }

          if (strcmp(cmd, "Kd") == 0) {
            packet.Kd = arg;
          }

          if (strcmp(cmd, "Wh") == 0) {
            packet.weight_high = arg;
          }

          if (strcmp(cmd, "Wl") == 0) {
            packet.weight_low = arg;
          }

          if (strcmp(cmd, "Ms") == 0) {
            packet.min_spd = arg;
          }

          if (strcmp(cmd, "Bs") == 0) {
            packet.base_spd = arg;
          }

          if (strcmp(cmd, "Mxs") == 0) {
            packet.max_spd = arg;
          }

          if (strcmp(cmd, "Ic") == 0) {
            packet.icap = arg;
          }
        }

        buff_index = 0;
      }
    } else if (buff_index < MAX_BUFF_LEN - 1) {
      in_buff[buff_index++] = recv_byte;
    }
  }

  if (last_packet.power == packet.power
  && last_packet.Kp == packet.Kp
  && last_packet.Ki == packet.Ki
  && last_packet.Kd == packet.Kd
  && last_packet.weight_low == packet.weight_low
  && last_packet.weight_high == packet.weight_high
  && last_packet.min_spd == packet.min_spd
  && last_packet.base_spd == packet.base_spd
  && last_packet.max_spd == packet.max_spd
  && last_packet.icap == packet.icap) {
    return;
  }

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(packet));

  Serial.println("Packet:");

  Serial.print("Power: ");
  Serial.println(packet.power);

  Serial.print("Kp: ");
  Serial.println(packet.Kp);

  Serial.print("Ki: ");
  Serial.println(packet.Ki);

  Serial.print("Kd: ");
  Serial.println(packet.Kd);

  Serial.print("Weight Low: ");
  Serial.println(packet.weight_low);
  
  Serial.print("Weight High: ");
  Serial.println(packet.weight_high);

  Serial.print("Min Speed: ");
  Serial.println(packet.min_spd);

  Serial.print("Base Speed: ");
  Serial.println(packet.base_spd);

  Serial.print("Max Speed: ");
  Serial.println(packet.max_spd);

  Serial.print("Integral Cap: ");
  Serial.println(packet.icap);

  Serial.println("");
   
  if (result == ESP_OK) {
    Serial.println("Sent with success\n");
  } else {
    Serial.println("Error sending the data\n");
  }
}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC - slave 
uint8_t slaveAddress[] = {0xCC, 0x50, 0xE3, 0x6F, 0x12, 0x60};

const int pinStart = D1; // pro test spojit s GND
const int pinSpeedUp = D2;
const int pinSpeedDown = D5;
const int pinOnOff = D6;

int lastStart = HIGH;
int lastSpeedUp = HIGH;
int lastSpeedDown = HIGH;
int lastOnOff = HIGH;

void setup() {
 Serial.begin(115200);
  pinMode(pinStart, INPUT_PULLUP);
  pinMode(pinSpeedUp, INPUT_PULLUP);
  pinMode(pinSpeedDown, INPUT_PULLUP);
  pinMode(pinOnOff, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) return;
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(slaveAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  int curStart = digitalRead(pinStart);
  int curSpeedUp = digitalRead(pinSpeedUp);
  int curSpeedDown = digitalRead(pinSpeedDown);
  int curOnOff = digitalRead(pinOnOff);

  if (lastStart == HIGH && curStart == LOW) {
    uint8_t cmd = 1;
    esp_now_send(slaveAddress, &cmd, sizeof(cmd));
    delay(100);
  }
  if (lastSpeedUp == HIGH && curSpeedUp == LOW) {
    uint8_t cmd = 2;
    esp_now_send(slaveAddress, &cmd, sizeof(cmd));
    delay(100);
  }
  if (lastSpeedDown == HIGH && curSpeedDown == LOW) {
    uint8_t cmd = 3;
    esp_now_send(slaveAddress, &cmd, sizeof(cmd));
    delay(100);
  }
  if (lastOnOff == HIGH && curOnOff == LOW) {
    uint8_t cmd = 4;
    esp_now_send(slaveAddress, &cmd, sizeof(cmd));
    delay(100);
  }

  lastStart = curStart;
  lastSpeedUp = curSpeedUp;
  lastSpeedDown = curSpeedDown;
  lastOnOff = curOnOff;
}
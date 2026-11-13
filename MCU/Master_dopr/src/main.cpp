#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC - slave 
uint8_t slaveAddress[] = {0xCC, 0x50, 0xE3, 0x6F, 0x12, 0x60};

struct CmdPin {
  uint8_t cmd;
  uint8_t pin;
  int lastState;
};

CmdPin cmdPins[] = {
  {1, D1, HIGH}, //Start
  {2, D6, HIGH}, //Speed Up
  {3, D5, HIGH}, //Speed Down
  {4, D2, HIGH}  //On/Off
};

const size_t PIN_COUNT = sizeof(cmdPins)/sizeof(cmdPins[0]);
const unsigned long signalDelay = 120; //ms
unsigned long lastSendTime = 0;
void OnDataSent(uint8_t *macAdr, uint8_t msgStatus)
{
  if (msgStatus == 0) {
    Serial.println("Received (ACK)");
  } else {
    Serial.println("Msg send failed");
  }
}

void setup() {
  Serial.begin(115200);

  for (size_t i = 0; i < PIN_COUNT; ++i) {
    pinMode(cmdPins[i].pin, INPUT_PULLUP);
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) 
  {
    Serial.println("Error ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(slaveAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  unsigned long now = millis();
  if (now - lastSendTime < signalDelay) {
    return;
  }
  for (size_t i = 0; i < PIN_COUNT; ++i) {
    int currentState = digitalRead(cmdPins[i].pin);
    bool edgeDet = cmdPins[i].lastState == HIGH && currentState == LOW;
    cmdPins[i].lastState = currentState;

    if (edgeDet)
    {
      uint8_t cmd = cmdPins[i].cmd;
      esp_now_send(slaveAddress, &cmd, sizeof(cmd));
      lastSendTime = now;
      break;
    }
  }
}
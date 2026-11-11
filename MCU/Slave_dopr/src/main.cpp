#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

const int pinOptoclen = D2;

//NEC kodovani
struct CmdNEC {
  uint8_t cmd;
  uint32_t NECcode;
  const char* cmdName;
};
const CmdNEC cmdNEC[] = {
  {1, 0xBB44FF00, "Start"},
  {2, 0xBC43FF00, "Speed Up"},
  {3, 0xF20DFF00, "Speed Down"},
  {4, 0xEA15FF00, "On/Off"}
};
const size_t NEC_COUNT = sizeof(cmdNEC)/sizeof(cmdNEC[0]);
volatile uint8_t CmdPending = 0;

void sendBasebandNEC(uint32_t data) {
  noInterrupts();
  digitalWrite(pinOptoclen, HIGH);
  delayMicroseconds(9000);
  digitalWrite(pinOptoclen, LOW);
  delayMicroseconds(4500);
  
  for (int i = 0; i < 32; i++) {
    digitalWrite(pinOptoclen, HIGH);
    delayMicroseconds(560);
    if (data & (1UL << i)) {
      digitalWrite(pinOptoclen, LOW);
      delayMicroseconds(1690); //1
    } else {
      digitalWrite(pinOptoclen, LOW);
      delayMicroseconds(560); //0
    }
  }
  digitalWrite(pinOptoclen, HIGH);
  delayMicroseconds(560);
  digitalWrite(pinOptoclen, LOW); 
  interrupts();
}

void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if(len > 0 && CmdPending == 0)
  {
    CmdPending = incomingData[0];
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinOptoclen, OUTPUT);
  digitalWrite(pinOptoclen, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0)
  {
    Serial.println("Error ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataRecv);
}
void loop() {
  if(CmdPending > 0)
  {
    bool cmdFound = false;
    
    for (size_t i = 0; i < NEC_COUNT; ++i) {
      if (cmdNEC[i].cmd == CmdPending) {
        sendBasebandNEC(cmdNEC[i].NECcode);
        Serial.print("Sent command: ");
        Serial.println(cmdNEC[i].cmdName);
        cmdFound = true;
        break;
      }
    }
    if (!cmdFound) {
      Serial.print("Unknown command: ");
      Serial.println(CmdPending);
    }
    CmdPending = 0;
  }
}
#include <ESP8266WiFi.h>
#include <espnow.h>

const int pinOptoclen = D2;

//NEC kodovani
const uint32_t KOD_ONOFF   = 0xEA15FF00;
const uint32_t KOD_START   = 0xBB44FF00;
const uint32_t KOD_SPEEDUP = 0xBC43FF00;
const uint32_t KOD_SPEEDDN = 0xF20DFF00;

void sendBasebandNEC(uint32_t data) {
  noInterrupts();
  digitalWrite(pinOptoclen, HIGH); delayMicroseconds(9000);
  digitalWrite(pinOptoclen, LOW); delayMicroseconds(4500);
  
  for (int i = 0; i < 32; i++) {
    digitalWrite(pinOptoclen, HIGH); delayMicroseconds(560);
    if (data & (1UL << i)) {
      digitalWrite(pinOptoclen, LOW); delayMicroseconds(1690); 
    } else {
      digitalWrite(pinOptoclen, LOW); delayMicroseconds(560);  
    }
  }
  
  digitalWrite(pinOptoclen, HIGH); delayMicroseconds(560);
  digitalWrite(pinOptoclen, LOW); 
  interrupts();
}

void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  uint8_t povel = incomingData[0];
  if (povel == 1) sendBasebandNEC(KOD_START);
  else if (povel == 2) sendBasebandNEC(KOD_SPEEDUP);
  else if (povel == 3) sendBasebandNEC(KOD_SPEEDDN);
  else if (povel == 4) sendBasebandNEC(KOD_ONOFF);
}

void setup() {
  pinMode(pinOptoclen, OUTPUT);
  digitalWrite(pinOptoclen, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) return;
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataRecv);
}
void loop() {

}
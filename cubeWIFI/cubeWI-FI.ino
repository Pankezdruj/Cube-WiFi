/*
  Exoy Tesseract sketch
  Copyright Exoy Ltd.

  Made by Exoy Team.
*/
// ===== Inclusions =====
#include <FastLED.h>
#include "Constants.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "CaptivePortalManager.h"
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "Types.h"
#include "EepromManager.h"

// ===== Object Init =====
WiFiManager wifiManager;
WiFiServer wifiServer(ESP_HTTP_PORT);
WiFiUDP Udp;

// ===== Var Init =====
uint16_t localPort = ESP_UDP_PORT;
char packetBuffer[MAX_UDP_BUFFER_SIZE];
char inputBuffer[MAX_UDP_BUFFER_SIZE];

ModeType modes[MODE_AMOUNT];

bool loadingFlag = true;
bool ONflag = false;
uint32_t eepromTimeout;
bool settChanged = false;

bool CaptivePortalManager::captivePortalCalled = false;


void setup() {
  ESP.wdtEnable(WDTO_8S);

  Serial.begin(115200);
  FastLED.addLeds<WS2812, LED_DI, COLOR_ORDER>(leds, NUM_LEDS);
  if (CUR_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CUR_LIMIT);
  FastLED.setBrightness(BRIGHT);
  FastLED.clear();
  randomSeed(getEntropy(A0));
  perlinPoint = random(0, 32768);
  pinMode(BTN_DI, INPUT);
  delay(10);

  // EEPROM
  EepromManager::InitEepromSettings(                        // инициализация EEPROM; запись начального состояния настроек, если их там ещё нет; инициализация настроек лампы значениями из EEPROM
    modes, &espMode, &ONflag, &currentMode);
  if (EEPROM.read(EEPROM_LOW_PASS_ADRESS) == 0) autoLowPass();

  nextMode = EEPROM.read(EEPROM_CURRENT_MODE_ADDRESS);

  WiFiConnect();

  // ОСТАЛЬНОЕ
  changePower();
  loadingFlag = true;

  //startEffect();
}

void loop() {
  parseUDP();
  buttonTick();
  draw();
  effectChangeHandler();
  EepromManager::HandleEepromTick(&settChanged, &eepromTimeout, &ONflag,
                                  &currentMode, modes);
  ESP.wdtFeed();

}

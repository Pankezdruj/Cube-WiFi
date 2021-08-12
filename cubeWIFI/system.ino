byte getMaxNoise(uint16_t x, uint16_t y) {
  return constrain(map(inoise8(x, y), 50, 200, 0, 255), 0, 255);
}

uint32_t getEntropy(byte pin) {
  unsigned long seed = 0;
  for (int i = 0; i < 400; i++) {
    seed = 1;
    for (byte j = 0; j < 16; j++) {
      seed *= 4;
      seed += analogRead(pin) & 3;
    }
  }
  return seed;
}
float secureMap(int x, int in_min, int in_max, int out_min, int out_max) {
  if (x < in_min) return out_min;
  if (x > in_max) return out_max;
  if (in_min == in_max) return out_min;
  if (out_min == out_max) return out_min;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

byte getBrightnessU() {
  return BRIGHTNESS < 50 ? 50 : BRIGHTNESS;
}

byte getBGBrightnessU() {
  return (modes[currentMode].BGBrightness - BRIGHTNESS) > -70 ? BRIGHTNESS - 70 : modes[currentMode].BGBrightness;
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void autoLowPass() {
  // для режима VU
  delay(10);                                // ждём инициализации АЦП
  int thisMax = 0;                          // максимум
  int thisLevel;
  for (byte i = 0; i < 200; i++) {
    thisLevel = analogRead(SOUND_R);        // делаем 200 измерений
    if (thisLevel > thisMax)                // ищем максимумы
      thisMax = thisLevel;                  // запоминаем
    delay(4);                               // ждём 4мс
  }
  LOW_PASS = thisMax + LOW_PASS_ADD;        // нижний порог как максимум тишины + некая величина

  if (EEPROM_LOW_PASS) {
    EEPROM.write(EEPROM_LOW_PASS_ADRESS, LOW_PASS);
  }
}

void fullLowPass() {
  digitalWrite(MLED_PIN, MLED_ON);   // включить светодиод
  FastLED.setBrightness(0); // погасить ленту
  FastLED.clear();          // очистить массив пикселей
  FastLED.show();           // отправить значения на ленту
  delay(500);               // подождать чутка
  autoLowPass();            // измерить шумы
  delay(500);               // подождать
  FastLED.setBrightness(getBrightnessU());  // вернуть яркость
  digitalWrite(MLED_PIN, !MLED_ON);    // выключить светодиод
}

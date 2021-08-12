void effectChangeHandler() {
  if (changeModes && millis() - tmrColor >= CHANGE_PRD * 1000L) {
    FastLED.clear();
    tmrColor = millis();
    fadeFlag = true;
    incrementMode();
  }
  if (fadeFlag && millis() - tmrFade >= 10) {
    static int8_t fadeDir = -1;
    tmrFade = millis();
    if (fadeFlag) {
      curBright += 5 * fadeDir;

      if (curBright < 5) {
        curBright = 5;
        fadeDir = 1;
        changeMode();
        EepromManager::SaveModesSettings(&currentMode, modes);
      }
      if (curBright > BRIGHT) {
        curBright = BRIGHT;
        fadeDir = -1;
        fadeFlag = false;
      }
      FastLED.setBrightness(curBright);
    }
  }
}

void changePower()
{
  if (ONflag)
  {
    draw();
    for (uint8_t i = 0U; i < getBrightnessU(); i = constrain(i + 8, 0, getBrightnessU()))
    {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.setBrightness(getBrightnessU());
    delay(2);
    FastLED.show();
  }
  else
  {
    draw();
    for (uint8_t i = getBrightnessU(); i > 0; i = constrain(i - 8, 0, getBrightnessU()))
    {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.clear();
    delay(2);
    FastLED.show();
  }
}

void changeMode() {
  currentMode = nextMode;
  EepromManager::SaveModesSettings(&currentMode, modes);
  tmrColor = millis();
  int thisDebth = random(0, 32768);
  byte thisStep = random(2, 7) * 5;
  switch (currentMode) {
    case (5 || 14):
      for (int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV(getMaxNoise(thisDebth + i * thisStep, thisDebth),
                                 (!random(9) ? 30 : 255),
                                 musicSync ? 255 : (constrain((i + 7) * (i + 7), 0, 255)));
      }
      break;
    default:
      for (int i = 0; i < 4; i++) {
        CHSV color = CHSV(random(0, 256), bool(random(0, 3)) * 255, bool(random(0, 3)) * 255);
        for (byte j = 0; j < 4; j++) {
          currentPalette[i * 4 + j] = color;
        }
      }
      break;
  }
}

void incrementMode() {
  nextMode++;
  if (nextMode > MODE_AMOUNT - 1) {
    nextMode = 0;
  }
}

void draw() {
  int scrollSpeed = 0;
  if (ONflag && millis() - tmrDraw >= DRAW_DELAY) {
    tmrDraw = millis();
    getSoundLength();
    switch (currentMode) {
      case 0: //COLOR
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(modes[currentMode].Color[0], modes[currentMode].Color[1], getBrightnessU());
        break;
      case 1: //COLOR SHIFT
        if (millis() - color_timer > COLOR_SPEED) {
          color_timer = millis();
          this_color += COLOR_SPEED > 100 ? 1 : map(COLOR_SPEED, 100, 0, 1, 10);
          if (this_color > 255) this_color = 0;
        }
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(this_color, modes[currentMode].Color[1], getBrightnessU());
        break;
      case 2: //RAINBOW
        if (millis() - rainbow_timer > 30) {
          rainbow_timer = millis();
          this_color += RAINBOW_PERIOD;
          if (this_color > 255) this_color = 0;
          if (this_color < 0) this_color = 255;
        }
        rainbow_steps = this_color;
        for (int i = 0; i < FACE_SIZE; i++) {
          fillVertex(i, CHSV((int)floor(rainbow_steps), 255, getBrightnessU()));
          rainbow_steps += RAINBOW_STEP_2;
          if (rainbow_steps > 255) rainbow_steps = 0;
          if (rainbow_steps < 0) rainbow_steps = 255;
        }
        break;
      case 3: //STROBOSCOPE REACTION
        {
          if (thisBright[3] < 10 || thisBright[3] < getBGBrightnessU()) for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(modes[currentMode].Color[0], modes[currentMode].Color[1], getBGBrightnessU());
          else for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(modes[currentMode].Color[0], modes[currentMode].Color[1], thisBright[3]);
          break;
        }
      case 4: //COLORFUL STROBOSCOPE REACTION
        {
          static byte color;
          if (thisBright[3] < 10 || thisBright[3] < getBGBrightnessU()) for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(color, modes[currentMode].Color[1], getBGBrightnessU());
          else {
            for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(color, 255, thisBright[3]);
            color = random(254);
          }
          break;
        }
      case 5: //FIRE
        if (millis() - tmrFire > 40) {
          tmrFire = millis();
          fireRoutine();
        }
        break;
      case 6:  {//VOLUME REACTION
          for (int i = 0; i < FACE_SIZE; i++) {
            fillVertex(i, ColorFromPalette(currentPalette, (float)255 * (i + 1) / FACE_SIZE / 2 + counter / 4, getBGBrightnessU(), LINEARBLEND));
          }
          for (int i = 0; i < Rlenght; i++) {
            fillVertex(i, ColorFromPalette(currentPalette, (float)255 * (i + 1) / FACE_SIZE / 2 + counter / 4, getBrightnessU(), LINEARBLEND));
          }
          break;
        }
      case 7: { //BREATHING REACTION
          FastLED.clear();
          static byte linearCounter = 0;
          static int maxVol = 0;
          if (thisBright[0] > maxVol) maxVol = thisBright[0];
          if (linearCounter % BREATHING_SPEED == 0) {
            for (byte i = 0; i < 15; i++) {
              linearPalette[i] = linearPalette[i + 1];
            }
            linearPalette[15] = CHSV(linearCounter, 255, (float)255 * maxVol / 100);
            maxVol = 0;
          }

          linearCounter += 1;
          for (int i = 0; i < FACE_SIZE; i++) {
            fillVertex(i, ColorFromPalette(linearPalette, (float)255 * i / FACE_SIZE, 255, LINEARBLEND));
          }
          break;
        }
      case 8: {//LINES REACTION
          counter += thisBright[0];
          for (int i = 0; i < FACE_SIZE; i++) {
            fillSimple(i, ColorFromPalette(currentPalette, getMaxNoise(i * PAL_STEP / 2 + counter / LINES_SPEED, 0), 255, LINEARBLEND));
          }
          break;
        }
      case 9: //RAINBOW REACTION
        for (int i = 0; i < FACE_SIZE; i++) {
          fillVertex(i, CHSV((int) map(inoise8(i, noiseY), 100, RAINBOW_REACTION_SPEED, 20, 255), 255, getBrightnessU()));
        }
        scrollSpeed = (thisBright[0] < thisBright[1] ? 2 : floatMap((float)thisBright[0], 10.00, (float)thisBright[2], 2.00, 15.00));
        noiseY += (scrollSpeed < 0 ? 0.05 : scrollSpeed);
        if (noiseY > 1000000) noiseY = 0;
        break;
      case 10: //COLOR REACTION
        for (int i = 0; i < FACE_SIZE; i++) {
          fillVertex(i, CHSV((int) map(inoise8(i, noiseY), 100, RAINBOW_REACTION_SPEED, modes[currentMode].Color[0] - 10, modes[currentMode].Color[0] + 15), 255, getBrightnessU()));
        }
        scrollSpeed = (thisBright[0] < thisBright[1] ? 2 : floatMap((float)thisBright[0], 10.00, (float)thisBright[2], 2.00, 15.00));
        noiseY += (scrollSpeed < 0 ? 0.05 : scrollSpeed);
        if (noiseY > 1000000) noiseY = 0;
        break;
      case 11: {//LIFE
          for (int i = 0; i < FACE_SIZE; i++) {
            fillSimple(i, ColorFromPalette(currentPalette, getMaxNoise(i * PAL_STEP + counter, counter), 255, LINEARBLEND));
          }
          break;
        }
      case 12: { //STARS REACTION
          STARS_SPEED = floatMap((float)thisBright[0], 0.00, 100.00, 6000.00, 100.00);
          STARS_AMOUNT = map(thisBright[0], 0, 100, 5, 15);
          static int brightness = getBrightnessU();
          static bool fadeBlack = false;
          if (millis() - tmrStars >= STARS_SPEED) {
            memset(starPos, 0, sizeof(starPos));
            tmrStars = millis();
            fadeBlack = false;
            brightness = 10;
            for (int i = 0; i < NUM_LEDS; i++) {
              bool showStar = !starPos[i == 0 ? 0 : i - 1] ? random(101) < STARS_AMOUNT : false;
              leds[i] = CHSV(modes[currentMode].Color[0],  modes[currentMode].Color[1], showStar ? brightness : 0);
              starPos[i] = showStar;
            }
            for (int i = 0; i < NUM_LEDS; i++) {
              if (!starPos[i]) leds[i] = CHSV(modes[currentMode].Color[0],  modes[currentMode].Color[1], getBGBrightnessU());
            }
          } else {
            if (millis() - tmrStarsFade >= map(STARS_SPEED, 6000, 100, 50, 5)) {
              tmrStarsFade = millis();
              if (!fadeBlack) {
                for (int i = 0; i < NUM_LEDS; i++) {
                  if (starPos[i]) leds[i].maximizeBrightness(brightness);
                }
                brightness += map(STARS_SPEED, 6000, 100, 3, 20);
                if (brightness >= getBrightnessU()) fadeBlack = true;
              } else {
                for (int i = 0; i < NUM_LEDS; i++) {
                  if (starPos[i])  leds[i].nscale8(map(STARS_SPEED, 6000, 100, 240, 50));
                }
              }
            }
          }
          break;
        }
      case 13: { //STARS
          static int brightness = getBrightnessU();
          static bool fadeBlack = false;
          if (millis() - tmrStars >= STARS_SPEED) {
            memset(starPos, 0, sizeof(starPos));
            tmrStars = millis();
            fadeBlack = false;
            brightness = 10;
            for (int i = 0; i < NUM_LEDS; i++) {
              bool showStar = !starPos[i == 0 ? 0 : i - 1] ? random(101) < STARS_AMOUNT : false;
              leds[i] = CHSV(modes[currentMode].Color[0],  modes[currentMode].Color[1], showStar ? brightness : 0);
              starPos[i] = showStar;
            }
            for (int i = 0; i < NUM_LEDS; i++) {
              if (!starPos[i]) leds[i] = CHSV( modes[currentMode].Color[0],  modes[currentMode].Color[1], getBGBrightnessU());
            }
          } else {
            if (millis() - tmrStarsFade >= map(STARS_SPEED, 6000, 100, 50, 5)) {
              tmrStarsFade = millis();
              if (!fadeBlack) {
                for (int i = 0; i < NUM_LEDS; i++) {
                  if (starPos[i]) {

                    leds[i].maximizeBrightness(brightness);
                  }
                }
                brightness += map(STARS_SPEED, 6000, 100, 3, 20);
                if (brightness >= getBrightnessU()) fadeBlack = true;
              } else {
                for (int i = 0; i < NUM_LEDS; i++) {
                  if (starPos[i])  leds[i].nscale8(map(STARS_SPEED, 6000, 100, 240, 50));
                }
              }
            }
          }
          break;
        }
    }
    FastLED.show();
    counter += speed;
  }
}
void blink(CRGB color) {
  FastLED.clear();
  for (int i = 0; i < FACE_SIZE; i++) {
    fillSimple(i, CHSV(0, 0, 0));
  }
  FastLED.show();
  delay(300);

  FastLED.clear();
  for (int i = 0; i < FACE_SIZE; i++) {
    fillSimple(i, color);
  }
  FastLED.show();
  delay(200);

  FastLED.clear();
  for (int i = 0; i < FACE_SIZE; i++) {
    fillSimple(i, CHSV(0, 0, 0));
  }
  FastLED.show();
  delay(300);
}
void startEffect() {
  FastLED.clear();
  for (int i = 0; i < FACE_SIZE; i++) {
    fillVertex(i, ColorFromPalette(RainbowColors_p, i * 3));
    FastLED.show();
    delay(60 / FACE_SIZE);
  }
  for (int i = FACE_SIZE; i > 0; i--) {
    fillVertex(i, CRGB(0, 0, 0));
    FastLED.show();
    delay(60 / FACE_SIZE);
  }
}

void fillSimple(int num, CRGB color) {  // num 0-NUM_LEDS / 6
  for (byte i = 0; i < 6; i++) {
    leds[i * FACE_SIZE + num] = color;
  }
}

void fillVertex(int num, CRGB color) { // num 0-NUM_LEDS / 6
  num /= 4;
  byte thisRow = 0;
  for (byte i = 0; i < 3; i++) {
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
  }
  thisRow = 13;
  for (byte i = 0; i < 3; i++) {
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
    leds[LINE_SIZE * thisRow - num - 1] = color;
    leds[LINE_SIZE * thisRow + num] = color;
    thisRow += 2;
  }
}

void fillFire(int num, int face, CRGB color) {
  switch (face) {
    case 1:
      leds[num] = color;
      if (num > LINE_SIZE * 3 - 1) leds[8 * LINE_SIZE - num - 1] = color;
      else if (num > LINE_SIZE * 2 - 1) leds[21 * LINE_SIZE - num - 1] = color;
      else if (num > LINE_SIZE - 1) leds[23 * LINE_SIZE - num - 1] = color;
      else if (num < LINE_SIZE) leds[12 * LINE_SIZE - num - 1] = color;
      break;
    case 2:
      leds[LINE_SIZE * 5] = color;
      leds[LINE_SIZE * 8 - 1] = color;
      leds[LINE_SIZE * 8] = color;
      leds[LINE_SIZE * 11 - 1] = color;
      leds[LINE_SIZE * 18 - 1] = color;
      leds[LINE_SIZE * 19] = color;
      leds[LINE_SIZE * 21 - 1] = color;
      leds[LINE_SIZE * 22] = color;

      for (int i = LINE_SIZE - 1; i > 0; i--) {
        fillColumns(i, leds[i + LINE_SIZE * 5 - 1]);
      }
      break;
    case 3:
      leds[num + LINE_SIZE * 12] = color;
      if (num > LINE_SIZE * 3 - 1) leds[20 * LINE_SIZE - num - 1] = color;
      else if (num > LINE_SIZE * 2 - 1) leds[9 * LINE_SIZE - num - 1] = color;
      else if (num > LINE_SIZE - 1) leds[11 * LINE_SIZE - num - 1] = color;
      else if (num < LINE_SIZE) leds[24 * LINE_SIZE - num - 1] = color;
      break;
  }
}

void fillColumns(int num, CRGB color) {
  leds[num + LINE_SIZE * 5] = color;
  leds[LINE_SIZE * 8 - num - 1] = color;
  leds[num + LINE_SIZE * 8] = color;
  leds[LINE_SIZE * 11 - num - 1] = color;
  leds[LINE_SIZE * 18 - num - 1] = color;
  leds[num + LINE_SIZE * 19] = color;
  leds[LINE_SIZE * 21 - num - 1] = color;
  leds[num + LINE_SIZE * 22] = color;
}

void fireRoutine()
{
  for (int i = 0; i < FACE_SIZE; i++) {
    fillFire(i, 1, CHSV(random(101) < 31 ? random(20) + 5 : 15, 255, random(101) < 31 ? random(100) : getBrightnessU()));
  }

  fillFire(LINE_SIZE, 2, CHSV(5, 255, random(101) > 41 ? random(150) : 0));

  for (int i = 0; i < FACE_SIZE; i++) {
    fillFire(i, 3, CHSV(0, 255, random(101) < 31 ? random(50) : getBrightnessU() - 200 < 0 ? 30 : getBrightnessU() - 200));
  }
}

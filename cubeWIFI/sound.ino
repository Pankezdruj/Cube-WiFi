void getSoundLength() {
  RsoundLevel = 0;
  maxLevel = 0;

  for (byte i = 0; i < 100; i ++) {
    RcurrentLevel = analogRead(ADC_PIN);
    if (RsoundLevel < RcurrentLevel) RsoundLevel = RcurrentLevel;
  }

  // LOW PASS FILTER
  RsoundLevel = secureMap(RsoundLevel, LOW_PASS, 1023, 0, 500); //20 > LOW_PASS

  // FILTER
  if (RsoundLevel_f < 1) RsoundLevel_f = 1; else if (RsoundLevel_f > 1000) RsoundLevel_f = 200;
  if (currentMode == 5 || currentMode == 6 || currentMode == 7) RsoundLevel_f = (RsoundLevel * SMOOTH) + (RsoundLevel_f * (1 - SMOOTH));
  else RsoundLevel_f = RsoundLevel;
  if (RsoundLevel_f > 500) RsoundLevel_f = 300;

  averageLevel = (float)RsoundLevel_f * averK + averageLevel * (1 - averK);
  if (averageLevel < 2) averageLevel = 2; else if (averageLevel > 500) averageLevel = 300;

  maxLevel = (float)averageLevel * MAX_COEF;

  if (maxLevel < 5) maxLevel = 5;
  if (maxLevel < RsoundLevel_f) maxLevel = RsoundLevel_f;

  Rlenght = secureMap(RsoundLevel_f, 0, maxLevel, 0, FACE_SIZE);

  thisBright[0] = RsoundLevel_f;
  thisBright[1] = averageLevel;
  thisBright[2] = maxLevel;

  if (RsoundLevel_f > averageLevel + 20) colorMusicFlash = true;
  else colorMusicFlash = false;

  if (thisBright[3] > 0) thisBright[3] -= SMOOTH_STEP; else if (colorMusicFlash) thisBright[3] = 255;
  if (thisBright[3] < 0) thisBright[3] = 0;
}

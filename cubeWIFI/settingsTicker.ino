void settingsTick() {
  switch (currentMode) {
    case (1): //COLOR SHIFT
      COLOR_SPEED = map(modes[1].Speed, 0, 100, 255, 0);
      break;
    case (2): //RAINBOW
      RAINBOW_STEP_2 = floatMap((float)modes[2].Speed, 0.00, 100.00, 1.00, 10.00); //ширина полосок
      RAINBOW_PERIOD = map(modes[2].Speed, 0, 100, 1, 20); //скорость
      break;
    case (3): //STROBOSCOPE REACTION
      SMOOTH_STEP = map(modes[3].Speed, 0, 100, 1, NUM_LEDS);
      break;
    case (4): //COLORFUL STROBOSCOPE REACTION
      SMOOTH_STEP = map(modes[4].Speed, 0, 100, 1, NUM_LEDS);
      break;
    case (6): //VOLUME REACTION
      SMOOTH = floatMap((float)modes[6].Speed, 0.00, 100.00, 0.20, 1.00);
      break;
    case (7): //BREATHING REACTION
      SMOOTH = floatMap((float)modes[7].Speed, 0.00, 100.00, 0.20, 1.00);
      break;
    case (8): //LINES REACTION
      SMOOTH = floatMap((float)modes[8].Speed, 0.00, 100.00, 0.10, 1.50);
      LINES_SPEED = floatMap((float)modes[8].Speed, 0.00, 100.00, 5.00, 2.00);
      break;
    case (9): //RAINBOW REACTION
      RAINBOW_REACTION_SPEED = map(modes[9].Speed, 0, 100, 300, 150);
      break;
    case (10): //COLOR REACTION
      RAINBOW_REACTION_SPEED = map(modes[10].Speed, 0, 100, 300, 150);
      break;
    case (13): //STARS
      STARS_SPEED = floatMap((float)modes[13].Speed, 0.00, 100.00, 6000.00, 100.00);
      STARS_AMOUNT = map(modes[13].Speed, 0, 100, 5, 15);
      break;
  }
}

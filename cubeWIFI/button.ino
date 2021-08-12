void buttonTick() {
  bool btnState = digitalRead(BTN_DI);
  if (!btnState && doubleFlag && millis() > tmrBtn + 400 && millis() < tmrBtn + 500) {
    fadeFlag = true;
    incrementMode();
    doubleFlag = false;
  }
  if (btnState && !flag) {
    flag = true;
    if (doubleFlag && millis() > tmrBtn && millis() < tmrBtn + 400) {
      doubleFlag = false;
    } else doubleFlag = true;
    tmrBtn = millis();
    delay(50);  
  }
  if (!btnState && flag) {
    flag = false;
    if (millis() - tmrBtn >= 500) {
      doubleFlag = false;
      changeModes = !changeModes;
      blink(changeModes ? CRGB(0, 255, 0) : CRGB(255, 0, 0));
    }
  }
}

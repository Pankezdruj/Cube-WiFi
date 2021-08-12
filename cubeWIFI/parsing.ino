void parseUDP()
{
  int32_t packetSize = Udp.parsePacket();

  if (packetSize)
  {
    int16_t n = Udp.read(packetBuffer, MAX_UDP_BUFFER_SIZE);
    packetBuffer[n] = '\0';
    strcpy(inputBuffer, packetBuffer);

#ifdef GENERAL_DEBUG
    Serial.print(F("Inbound UDP packet: "));
    Serial.println(inputBuffer);
#endif

    if (Udp.remoteIP() == WiFi.localIP())                   // не реагировать на свои же пакеты
    {
      return;
    }

    char reply[MAX_UDP_BUFFER_SIZE];
    processInputBuffer(inputBuffer, reply, true);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(reply);
    Udp.endPacket();

#ifdef GENERAL_DEBUG
    Serial.print(F("Outbound UDP packet: "));
    Serial.println(reply);
    Serial.println();
#endif
  }
}


void processInputBuffer(char *inputBuffer, char *outputBuffer, bool generateOutput)
{
  char buff[MAX_UDP_BUFFER_SIZE], *endToken = NULL;

  if (!strncmp_P(inputBuffer, PSTR("GET"), 3))
  {
    sendCurrent(inputBuffer);
  }

  else if (!strncmp_P(inputBuffer, PSTR("EFF"), 3))
  {
    memcpy(buff, &inputBuffer[3], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    nextMode = (uint8_t)atoi(buff);
    fadeFlag = true;
    eepromTimeout = millis();
    FastLED.clear();
    delay(1);
    sendCurrent(inputBuffer);
    FastLED.setBrightness(getBrightnessU());
    settingsTick();
  }

  else if (!strncmp_P(inputBuffer, PSTR("BRI"), 3))
  {
    memcpy(buff, &inputBuffer[3], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    BRIGHTNESS = constrain(atoi(buff), 1, 255);
    EepromManager::SaveModesSettings(&currentMode, modes);
    FastLED.setBrightness(getBrightnessU());
    eepromTimeout = millis();
    sendCurrent(inputBuffer);
    settingsTick();
  }

  else if (!strncmp_P(inputBuffer, PSTR("BGB"), 3))
  {
    memcpy(buff, &inputBuffer[3], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    modes[currentMode].BGBrightness = atoi(buff);
    EepromManager::SaveModesSettings(&currentMode, modes);
    eepromTimeout = millis();
    sendCurrent(inputBuffer);
    settingsTick();
  }

  else if (!strncmp_P(inputBuffer, PSTR("CLR"), 3))
  {
    memcpy(buff, &inputBuffer[3], 3);   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    modes[currentMode].Color[0] = constrain(atoi(buff), 1, 255);
    memcpy(buff, &inputBuffer[7], strlen(inputBuffer));
    modes[currentMode].Color[1] = constrain(atoi(buff), 1, 255);
    EepromManager::SaveModesSettings(&currentMode, modes);
    eepromTimeout = millis();
    sendCurrent(inputBuffer);
    settingsTick();
  }


  else if (!strncmp_P(inputBuffer, PSTR("SPD"), 3))
  {
    memcpy(buff, &inputBuffer[3], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    modes[currentMode].Speed = atoi(buff);
    EepromManager::SaveModesSettings(&currentMode, modes);
    eepromTimeout = millis();
    sendCurrent(inputBuffer);
    settingsTick();
  }

  else if (!strncmp_P(inputBuffer, PSTR("P_ON"), 4))
  {
    ONflag = true;
    eepromTimeout = millis();
    changePower();
    EepromManager::SaveOnFlag(&ONflag);
    sendCurrent(inputBuffer);
  }

  else if (!strncmp_P(inputBuffer, PSTR("P_OFF"), 5))
  {
    ONflag = false;
    eepromTimeout = millis();
    changePower();
    EepromManager::SaveOnFlag(&ONflag);
    sendCurrent(inputBuffer);
  }
  else if (!strncmp_P(inputBuffer, PSTR("EFCH"), 3))
  {
    memcpy(buff, &inputBuffer[4], strlen(inputBuffer));   // взять подстроку, состоящую последних символов строки inputBuffer, начиная с символа 4
    changeModes = (bool)atoi(buff);
    blink(changeModes ? CRGB(0, 255, 0) : CRGB(255, 0, 0));
    eepromTimeout = millis();
  }
  else if (!strncmp_P(inputBuffer, PSTR("CAL"), 3))
  {
    fullLowPass();
    eepromTimeout = millis();
  }
  else
  {
    inputBuffer[0] = '\0';
  }

  if (strlen(inputBuffer) <= 0)
  {
    return;
  }

  if (generateOutput)                                     // если запрошен вывод ответа выполнения команд, копируем его в исходящий буфер
  {
    strcpy(outputBuffer, inputBuffer);
  }
  inputBuffer[0] = '\0';                                  // очистка буфера, читобы не он не интерпретировался, как следующий входной пакет
}

void sendCurrent(char *outputBuffer)
{
  sprintf_P(outputBuffer, PSTR("CURR %u %u %u %u %u %u %u %u %u %u %u"),
            PRODUCT_ID,
            PRODUCT_TYPE,
            PRODUCT_SIZE,
            currentMode,
            BRIGHTNESS,
            modes[currentMode].BGBrightness,
            modes[currentMode].Speed,
            modes[currentMode].Color[0],
            modes[currentMode].Color[1],
            changeModes,
            ONflag
           );
}

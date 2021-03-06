void WiFiConnect() {
  String apNameS = (String)AP_NAME + " #" + (String)PRODUCT_ID;
  unsigned char* buf = new unsigned char[20];
  apNameS.getBytes(buf, 100, 0);
  const char *apName = (const char*)buf;

  // WI-FI
  wifiManager.setDebugOutput(WIFIMAN_DEBUG);                // вывод отладочных сообщений
  // wifiManager.setMinimumSignalQuality();                 // установка минимально приемлемого уровня сигнала WiFi сетей (8% по умолчанию)
  CaptivePortalManager *captivePortalManager = new CaptivePortalManager(&wifiManager);
  if (espMode == 0U)                                        // режим WiFi точки доступа
  {
    // wifiManager.setConfigPortalBlocking(false);
    //    showWarning(CRGB::Green, 1000U, 500U);

    if (sizeof(AP_STATIC_IP))
    {
      Serial.println(F("Используется статический IP адрес WiFi точки доступа"));
      wifiManager.setAPStaticIPConfig(                      // wifiManager.startConfigPortal использовать нельзя, т.к. он блокирует вычислительный процесс внутри себя, а затем перезагружает ESP, т.е. предназначен только для ввода SSID и пароля
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),      // IP адрес WiFi точки доступа
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),                    // первый доступный IP адрес сети
        IPAddress(255, 255, 255, 0));                                                       // маска подсети
    }

    WiFi.softAP(apName, AP_PASS);

    Serial.println(F("Старт в режиме WiFi точки доступа"));
    Serial.print(F("IP адрес: "));
    Serial.println(WiFi.softAPIP());

    wifiServer.begin();
  }
  else                                                      // режим WiFi клиента (подключаемся к роутеру, если есть сохранённые SSID и пароль, иначе создаём WiFi точку доступа и запрашиваем их)
  {
    Serial.println(F("Старт в режиме WiFi клиента (подключение к роутеру)"));

    if (WiFi.SSID().length())
    {
      Serial.printf_P(PSTR("Подключение к WiFi сети: %s\n"), WiFi.SSID().c_str());

      if (sizeof(STA_STATIC_IP))                            // ВНИМАНИЕ: настраивать статический ip WiFi клиента можно только при уже сохранённых имени и пароле WiFi сети (иначе проявляется несовместимость библиотек WiFiManager и WiFi)
      {
        Serial.print(F("Сконфигурирован статический IP адрес: "));
        Serial.printf_P(PSTR("%u.%u.%u.%u\n"), STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]);
        wifiManager.setSTAStaticIPConfig(
          IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]),// статический IP адрес ESP в режиме WiFi клиента
          IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], 1),               // первый доступный IP адрес сети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
          IPAddress(255, 255, 255, 0));                                                     // маска подсети (справедливо для 99,99% случаев; для сетей меньше чем на 255 адресов нужно вынести в константы)
      }
    }
    else
    {
      Serial.println(F("WiFi сеть не определена, запуск WiFi точки доступа для настройки параметров подключения к WiFi сети..."));
      CaptivePortalManager::captivePortalCalled = true;
      wifiManager.setBreakAfterConfig(true);                // перезагрузка после ввода и сохранения имени и пароля WiFi сети
      //      showWarning(CRGB::Yellow, 1000U, 500U);               // мигание жёлтым цветом 0,5 секунды (1 раз) - нужно ввести параметры WiFi сети для подключения
    }

    wifiManager.setConnectTimeout(ESP_CONN_TIMEOUT);        // установка времени ожидания подключения к WiFi сети, затем старт WiFi точки доступа
    wifiManager.setConfigPortalTimeout(ESP_CONF_TIMEOUT);   // установка времени работы WiFi точки доступа, затем перезагрузка; отключить watchdog?
    wifiManager.autoConnect(apName, AP_PASS);              // пытаемся подключиться к сохранённой ранее WiFi сети; в случае ошибки, будет развёрнута WiFi точка доступа с указанными AP_NAME и паролем на время ESP_CONN_TIMEOUT секунд; http://AP_STATIC_IP:ESP_HTTP_PORT (обычно http://192.168.0.1:80) - страница для ввода SSID и пароля от WiFi сети роутера

    delete captivePortalManager;
    captivePortalManager = NULL;

    if (WiFi.status() != WL_CONNECTED)                      // подключение к WiFi не установлено
    {
      if (CaptivePortalManager::captivePortalCalled)        // была показана страница настройки WiFi ...
      {
        if (millis() < (ESP_CONN_TIMEOUT + ESP_CONF_TIMEOUT) * 1000U) // пользователь ввёл некорректное имя WiFi сети и/или пароль или запрошенная WiFi сеть недоступна
        {
          Serial.println(F("Не удалось подключиться к WiFi сети\nУбедитесь в корректности имени WiFi сети и пароля\nРестарт для запроса нового имени WiFi сети и пароля...\n"));
          wifiManager.resetSettings();
        }
        else                                                // пользователь не вводил имя WiFi сети и пароль
        {
          Serial.println(F("Время ожидания ввода SSID и пароля от WiFi сети или подключения к WiFi сети превышено\nЛампа будет перезагружена в режиме WiFi точки доступа!\n"));


          Serial.printf_P(PSTR("Рабочий режим лампы изменён и сохранён в энергонезависимую память\nНовый рабочий режим: ESP_MODE = %d, %s\nРестарт...\n"),
                          espMode, espMode == 0U ? F("WiFi точка доступа") : F("WiFi клиент (подключение к роутеру)"));
        }
      }
      else                                                  // страница настройки WiFi не была показана, не удалось подключиться к ранее сохранённой WiFi сети (перенос в новую WiFi сеть)
      {
        Serial.println(F("Не удалось подключиться к WiFi сети\nВозможно, заданная WiFi сеть больше не доступна\nРестарт для запроса нового имени WiFi сети и пароля...\n"));
        wifiManager.resetSettings();
      }

      ESP.restart();
    }

    if (CaptivePortalManager::captivePortalCalled &&        // первое подключение к WiFi сети после настройки параметров WiFi на странице настройки - нужна перезагрузка для применения статического IP
        sizeof(STA_STATIC_IP) &&
        WiFi.localIP() != IPAddress(STA_STATIC_IP[0], STA_STATIC_IP[1], STA_STATIC_IP[2], STA_STATIC_IP[3]))
    {
      Serial.println(F("Рестарт для применения заданного статического IP адреса..."));
      delay(100);
      ESP.restart();
    }

    Serial.print(F("IP адрес: "));
    Serial.println(WiFi.localIP());
  }
  ESP.wdtFeed();

  Serial.printf_P(PSTR("Порт UDP сервера: %u\n"), localPort);
  Udp.begin(localPort);
}

#pragma once

#include <ESP8266WebServer.h>

// ===== Settings =====
#define PRODUCT_ID 1
#define PRODUCT_TYPE 1
#define PRODUCT_SIZE 16 //(inches / inches x inches)

#define EDGE_LEDS 20
#define CUR_LIMIT 5000

// ===== Constants =====
#define VOL_THR 20 //mirco
#define BRIGHT 255 //brightness
#define CHANGE_PRD 20 //effect change period in seconds
#define ADC_PIN A0 //micro pin
#define LED_DI 2 //led pin
#define BTN_DI 4 //button pin
#define PAL_STEP 30 //palletes step
#define MODE_AMOUNT 14
// ----- Strip ------
#define COLOR_ORDER           (GRB)                         // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB
// ----- Esp -----
#define ESP_MODE              (1U)                          // 0U - WiFi точка доступа, 1U - клиент WiFi (подключение к роутеру)
uint8_t espMode = 1U;                                 // ESP_MODE может быть сохранён в энергонезависимую память и изменён в процессе работы лампы без необходимости её перепрошивки

#define ESP_RESET_ON_START    (false)                       // true - если при старте нажата кнопка (или кнопки нет!), сохранённые настройки будут сброшены; false - не будут
#define ESP_HTTP_PORT         (80U)                         // номер порта, который будет использоваться во время первой утановки имени WiFi сети (и пароля), к которой потом будет подключаться лампа в режиме WiFi клиента (лучше не менять)
#define ESP_UDP_PORT          (8888U)                       // номер порта, который будет "слушать" UDP сервер во время работы лампы как в режиме WiFi точки доступа, так и в режиме WiFi клиента (лучше не менять)
#define ESP_CONN_TIMEOUT      (7U)                          // время в секундах (ДОЛЖНО БЫТЬ МЕНЬШЕ 8, иначе сработает WDT), которое ESP будет пытаться подключиться к WiFi сети, после его истечения автоматически развернёт WiFi точку доступа
#define ESP_CONF_TIMEOUT      (300U)                        // время в секундах, которое ESP будет ждать ввода SSID и пароля WiFi сети роутера в конфигурационном режиме, после его истечения ESP перезагружается
#define GENERAL_DEBUG                                       // если строка не закомментирована, будут выводиться отладочные сообщения
#define WIFIMAN_DEBUG         (false)                       // вывод отладочных сообщений при подключении к WiFi сети: true - выводятся, false - не выводятся; настройка не зависит от GENERAL_DEBUG
#define LED_PIN               (2U)                          // пин ленты                (D4)
#define SOUND_R               A0                           // аналоговый пин вход аудио, правый канал
#define MLED_PIN              (13U)                        // пин светодиода режимов
#define MLED_ON               HIGH

const uint8_t STA_STATIC_IP[] = {};                         // статический IP адрес: {} - IP адрес определяется роутером; {192, 168, 1, 66} - IP адрес задан явно (если DHCP на роутере не решит иначе); должен быть из того же диапазона адресов, что разадёт роутер
// SSID WiFi сети и пароль будут запрошены WiFi Manager'ом в режиме WiFi точки доступа, нет способа захардкодить их в прошивке
// ----- AP -----
#define AP_NAME               ("ExoyIndividual")                   // имя WiFi точки доступа, используется как при запросе SSID и пароля WiFi сети роутера, так и при работе в режиме ESP_MODE = 0
#define AP_PASS               ("12345678")                  // пароль WiFi точки доступа
const uint8_t AP_STATIC_IP[] = {192, 168, 4, 1};            // статический IP точки доступа (лучше не менять)
// ----- UDP -----
#define MAX_UDP_BUFFER_SIZE   (129U)                        // максимальный размер буффера UDP сервера
// ----- Libs -----
#define FASTLED_INTERRUPT_RETRY_COUNT   (1)                 // default: 2; // Use this to determine how many times FastLED will attempt to re-transmit a frame if interrupted for too long by interrupts
#define FASTLED_ESP8266_RAW_PIN_ORDER                       // FASTLED_ESP8266_RAW_PIN_ORDER, FASTLED_ESP8266_D1_PIN_ORDER or FASTLED_ESP8266_NODEMCU_PIN_ORDER


// ===== System =====
const int NUM_LEDS = (EDGE_LEDS * 24);
CRGBPalette16 currentPalette, linearPalette;
const int FACE_SIZE = EDGE_LEDS * 4;
const int LINE_SIZE = EDGE_LEDS;
CRGB leds[NUM_LEDS];
uint32_t tmrDraw, tmrColor, tmrFade, tmrBtn, tmrStars, tmrStarsFade, tmrFire;
bool doubleFlag = false;
bool flag = false;

// ===== Variables =====
int8_t currentMode = 0;
//byte lastMode = 0;
int8_t nextMode = 0;
int perlinPoint;
int curBright = BRIGHT;
bool fadeFlag = true;
bool changeModes = false;
uint16_t counter = 0;
byte speed = 5;
bool musicSync = true;

// ===== Music Sync =====
int Rlenght;
float RsoundLevel, RsoundLevel_f = 0;

// ===== Effects =====
byte BRIGHTNESS = 200;
// ----- Rainbow -----
float RAINBOW_STEP = 5.00;         // шаг изменения цвета радуги

// ----- Draw ------
#define DRAW_DELAY 1               // период основного цикла отрисовки (по умолчанию 5)

// ----- Volume -----
#define VOLUME_SPEED 2

// ----- Fire -----

// ----- Breathing -----
int BREATHING_SPEED = 1;

// ----- Lines -----
float LINES_SPEED = 3;

// ----- Rainbow Reaction -----
int RAINBOW_REACTION_SPEED = 200;

// ----- Stars -----
float STARS_SPEED = 5;
int STARS_AMOUNT = 1;
bool starPos[NUM_LEDS] = {};

// ???????????????????????????????
// ----- режим шкала громкости
float SMOOTH = 0.5;               // коэффициент плавности анимации VU (по умолчанию 0.5)
#define MAX_COEF 1.8              // коэффициент громкости (максимальное равно среднему * этот коэф) (по умолчанию 1.8)

// ----- сигнал
#define EXP 1.0                  // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)

// ----- нижний порог шумов
uint16_t LOW_PASS = 100;          // нижний порог шумов режим VU, ручная настройка
#define EEPROM_LOW_PASS 1         // порог шумов хранится в энергонезависимой памяти (по умолч. 1)
#define LOW_PASS_ADD 13           // "добавочная" величина к нижнему порогу, для надёжности (режим VU)

// ----- режим цветомузыки
byte SMOOTH_STEP = 70;           // шаг уменьшения яркости в режиме цветомузыки (чем больше, тем быстрее гаснет)
float RAINBOW_REACTION_STEP = 1;

// ----- режим подсветки
byte LIGHT_COLOR = 0;             // начальный цвет подсветки
byte LIGHT_SAT = 255;             // начальная насыщенность подсветки
byte COLOR_SPEED = 1;
int RAINBOW_PERIOD = 1;
float RAINBOW_STEP_2 = 0.5;
float BG_SCROLL_SPEED = 5;

// ----- режим бегущих частот
byte RUNNING_SPEED = 11;
int noiseY = 0;


float averageLevel = 50;
int maxLevel = 100;
int MAX_CH = NUM_LEDS / 2;
int hue;
unsigned long main_timer, running_timer, color_timer, rainbow_timer, eeprom_timer;
float averK = 0.006;
byte count;
float ind = (float)255 / MAX_CH;   // коэффициент перевода для палитры
int RcurrentLevel, LcurrentLevel;
boolean colorMusicFlash;
int thisBright[4];
float rainbow_steps;
int this_color;

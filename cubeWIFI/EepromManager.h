#pragma once

#include <EEPROM.h>
#include "Types.h"
#define EEPROM_TOTAL_BYTES_USED              (69U)         // общий размер используемой EEPROM памяти (сумма всех хранимых настроек + 1 байт)
#define EEPROM_MODES_START_ADDRESS           (0U)          // начальный адрес в EEPROM памяти для записи настроек эффектов (яркость, скорость, масштаб)
#define EEPROM_PRODUCT_ON_ADDRESS            (60U)         // адрес в EEPROM памяти для записи состояния лампы (вкл/выкл)
#define EEPROM_FIRST_RUN_ADDRESS             (62U)         // адрес в EEPROM памяти для записи признака первого запуска (определяет необходимость первоначальной записи всех хранимых настроек)
#define EEPROM_CURRENT_MODE_ADDRESS          (64U)         // адрес в EEPROM памяти для записи номера текущего эффекта лампы
#define EEPROM_LOW_PASS_ADRESS               (66U)
#define EEPROM_BRIGHTNESS_ADRESS             (68U)

#define EEPROM_MODE_STRUCT_SIZE              (4U)           // 1 байт - яркость; 1 байт - скорость; 1 байт - масштаб

#define EEPROM_FIRST_RUN_MARK                (24U)          // число-метка, если ещё не записно в EEPROM_FIRST_RUN_ADDRESS, значит нужно проинициализировать EEPROM и записать все первоначальные настройки
#define EEPROM_WRITE_DELAY                   (30000UL)      // отсрочка записи в EEPROM после последнего изменения хранимых настроек, позволяет уменьшить количество операций записи в EEPROM


class EepromManager
{
  public:
    static void InitEepromSettings(ModeType modes[], uint8_t* espMode, bool* onFlag, int8_t* currentMode)
    {
      // записываем в EEPROM начальное состояние настроек, если их там ещё нет
      EEPROM.begin(EEPROM_TOTAL_BYTES_USED);
      delay(50);

      if (EEPROM.read(EEPROM_FIRST_RUN_ADDRESS) != EEPROM_FIRST_RUN_MARK)
      {
        EEPROM.write(EEPROM_FIRST_RUN_ADDRESS, EEPROM_FIRST_RUN_MARK);
        EEPROM.commit();

        for (uint8_t i = 0; i < MODE_AMOUNT; i++)
        {
          EEPROM.put(EEPROM_MODES_START_ADDRESS * i, modes[i]);
          EEPROM.commit();
        }

        EEPROM.write(EEPROM_BRIGHTNESS_ADRESS, BRIGHTNESS);
        EEPROM.write(EEPROM_PRODUCT_ON_ADDRESS, 0);
        EEPROM.write(EEPROM_CURRENT_MODE_ADDRESS, 0);


        EEPROM.commit();
      }

      // инициализируем настройки лампы значениями из EEPROM
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        EEPROM.get(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * i, modes[i]);
      }

      *onFlag = (bool)EEPROM.read(EEPROM_PRODUCT_ON_ADDRESS);
      *currentMode = EEPROM.read(EEPROM_CURRENT_MODE_ADDRESS);
      LOW_PASS = EEPROM.read(EEPROM_LOW_PASS_ADRESS);
    }

    static void SaveModesSettings(int8_t* currentMode, ModeType modes[])
    {
      EEPROM.put(EEPROM_MODES_START_ADDRESS + EEPROM_MODE_STRUCT_SIZE * (*currentMode), modes[*currentMode]);
      EEPROM.write(EEPROM_BRIGHTNESS_ADRESS, BRIGHTNESS); 
      EEPROM.write(EEPROM_CURRENT_MODE_ADDRESS, *currentMode);
      EEPROM.commit();
    }


    static void HandleEepromTick(bool* settChanged, uint32_t* eepromTimeout, bool* onFlag, int8_t* currentMode, ModeType modes[])
    {
      if (*settChanged && millis() - *eepromTimeout > EEPROM_WRITE_DELAY)
      {
        *settChanged = false;
        *eepromTimeout = millis();
        SaveOnFlag(onFlag);
        SaveModesSettings(currentMode, modes);
        if (EEPROM.read(EEPROM_CURRENT_MODE_ADDRESS) != *currentMode)
        {
          EEPROM.write(EEPROM_CURRENT_MODE_ADDRESS, *currentMode);
        }
        EEPROM.commit();
      }
    }

    static void SaveOnFlag(bool* onFlag)
    {
      EEPROM.write(EEPROM_PRODUCT_ON_ADDRESS, *onFlag);
      EEPROM.commit();
    }

    static uint16_t ReadUint16(uint16_t address)
    {
      uint16_t val;
      uint8_t* p = (uint8_t*)&val;
      *p        = EEPROM.read(address);
      *(p + 1)  = EEPROM.read(address + 1);
      return val;
    }

    static void WriteUint16(uint16_t address, uint16_t val)
    {
      uint8_t* p = (uint8_t*)&val;
      EEPROM.write(address, *p);
      EEPROM.write(address + 1, *(p + 1));
      EEPROM.commit();
    }

    static int16_t ReadInt16(uint16_t address)
    {
      int16_t val;
      uint8_t* p = (uint8_t*)&val;
      *p        = EEPROM.read(address);
      *(p + 1)  = EEPROM.read(address + 1);
      return val;
    }

    static void WriteInt16(uint16_t address, int16_t val)
    {
      uint8_t* p = (uint8_t*)&val;
      EEPROM.write(address, *p);
      EEPROM.write(address + 1, *(p + 1));
      EEPROM.commit();
    }

    static uint32_t ReadUint32(uint16_t address)
    {
      uint32_t val;
      uint8_t* p = (uint8_t*)&val;
      *p        = EEPROM.read(address);
      *(p + 1)  = EEPROM.read(address + 1);
      *(p + 2)  = EEPROM.read(address + 2);
      *(p + 3)  = EEPROM.read(address + 3);
      return val;
    }

    static void WriteUint32(uint16_t address, uint32_t val)
    {
      uint8_t* p = (uint8_t*)&val;
      EEPROM.write(address, *p);
      EEPROM.write(address + 1, *(p + 1));
      EEPROM.write(address + 2, *(p + 2));
      EEPROM.write(address + 3, *(p + 3));
      EEPROM.commit();
    }

    static int32_t ReadInt32(uint16_t address)
    {
      int32_t val;
      uint8_t* p = (uint8_t*)&val;
      *p        = EEPROM.read(address);
      *(p + 1)  = EEPROM.read(address + 1);
      *(p + 2)  = EEPROM.read(address + 2);
      *(p + 3)  = EEPROM.read(address + 3);
      return val;
    }

    static void WriteInt32(uint16_t address, int32_t val)
    {
      uint8_t* p = (uint8_t*)&val;
      EEPROM.write(address, *p);
      EEPROM.write(address + 1, *(p + 1));
      EEPROM.write(address + 2, *(p + 2));
      EEPROM.write(address + 3, *(p + 3));
      EEPROM.commit();
    }

  private:
};

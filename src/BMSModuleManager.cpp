#include "config.h"
#include "BMSModuleManager.h"
#include "BMSUtil.h"
#include "Logger.h"

extern EEPROMSettings settings;
CAN_message_t OUTmsg;

BMSModuleManager::BMSModuleManager()
{
  for (int i = 1; i <= MAX_MODULE_ADDR; i++) {
    modules[i].setExists(false);
    modules[i].setAddress(i);
  }
  lowestPackVolt = 1000.0f;
  highestPackVolt = 0.0f;
  lowestPackTemp = 200.0f;
  highestPackTemp = -100.0f;
  isFaulted = false;
  balancing = false;
  balcnt = 0;//counter to stop balancing for cell measurement
}

bool BMSModuleManager::checkcomms()
{
  int g = 0;
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      g = 1;
      if (modules[y].isReset())
      {
        //Do nothing as the counter has been reset
      }
      else
      {
        modules[y].setExists(false);
        return false;
      }
    }
    modules[y].setReset(false);
    modules[y].setAddress(y);
  }
  if ( g == 0)
  {
    return false;
  }
  return true;
}

void BMSModuleManager::setBalanceHyst(float newVal)
{
  BalHys = newVal;
  //Serial.println();
  //Serial.println(BalHys, 3);
}

void BMSModuleManager::balanceCells(int debug)
{


  uint16_t balance = 0;//bit 0 - 5 are to activate cell balancing 1-6
  //Serial.println();
  // Serial.println(LowCellVolt + BalHys, 3);
  /*
    Serial.println();
    Serial.println();
    Serial.print("Balcnt:");
    Serial.println(balcnt);
  */
  if (balcnt > 60)
  {
    balcnt = 0;
  }


  if (balcnt > 10)
  {
    if (balcnt == 11 || balcnt == 15 || balcnt == 20 || balcnt == 25 || balcnt == 30 || balcnt == 35 || balcnt == 40 || balcnt == 45 || balcnt == 50 || balcnt == 55)
    {
      balancing = false;
      for (int y = 1; y < 63; y++)
      {
        if (modules[y].isExisting() == 1)
        {
          balance = 0;
          for (int i = 0; i < 12; i++)
          {
            if ((LowCellVolt + BalHys) < modules[y].getCellVoltage(i))
            {
              balance = balance | (1 << i);
            }
            /*
              else
              {
              Serial.print(" | ");
              Serial.print(i);
              }
            */
          }
          if (balance > 0)
          {
            balancing = true;
          }
          if (debug == 1)
          {
            Serial.println();
            Serial.print("Module ");
            Serial.print(y);
            Serial.print(" | ");
            Serial.println(balance, HEX);

          }

          OUTmsg.buf[0] = 0X00;
          OUTmsg.buf[1] = 0X00;
          OUTmsg.buf[2] = 0X00;
          OUTmsg.buf[3] = 0X00;
          OUTmsg.buf[4] = 0X00;
          OUTmsg.buf[5] = 0X00;
          OUTmsg.buf[6] = 0X00;
          OUTmsg.buf[7] = 0X00;

          for (int i = 0; i < 8; i++)
          {
            if (bitRead(balance, i) == 1)
            {
              OUTmsg.buf[i] = 0x08;
            }
            else
            {
              OUTmsg.buf[i] = 0x00;
            }
          }

          switch (y)
          {
            case (1):
              OUTmsg.id  = 0x1A55540A;
              break;
            case (2):
              OUTmsg.id  = 0x1A55540C;
              break;
            case (3):
              OUTmsg.id  = 0x1A55540E;
              break;
            case (4):
              OUTmsg.id  = 0x1A555410;
              break;
            case (5):
              OUTmsg.id  = 0x1A555412;
              break;
            case (6):
              OUTmsg.id  = 0x1A555414;
              break;
            case (7):
              OUTmsg.id  = 0x1A555416;
              break;
            case (8):
              OUTmsg.id  = 0x1A555418;
              break;
            case (9):
              OUTmsg.id  = 0x1A55541A;
              break;
            case (10):
              OUTmsg.id  = 0x1A5554AB;
              break;
            case (11):
              OUTmsg.id  = 0x1A5554AD;
              break;
            case (12):
              OUTmsg.id  = 0x1A5554AF;
              break;

            default:
              break;
          }
          OUTmsg.len = 8;
          OUTmsg.flags.extended = 1;
          Can0.write(OUTmsg);

          delay(1);

          for (int i = 8; i < 13; i++)
          {
            if (bitRead(balance, i) == 1)
            {
              OUTmsg.buf[i - 8] = 0x08;
            }
            else
            {
              OUTmsg.buf[i - 8] = 0x00;
            }
          }
          OUTmsg.buf[4] = 0xFE;
          OUTmsg.buf[5] = 0xFE;
          OUTmsg.buf[6] = 0xFE;
          OUTmsg.buf[7] = 0xFE;

          switch (y)
          {
            case (1):
              OUTmsg.id  = 0x1A55540B;
              break;
            case (2):
              OUTmsg.id  = 0x1A55540D;
              break;
            case (3):
              OUTmsg.id  = 0x1A55540F;
              break;
            case (4):
              OUTmsg.id  = 0x1A555411;
              break;
            case (5):
              OUTmsg.id  = 0x1A555413;
              break;
            case (6):
              OUTmsg.id  = 0x1A555415;
              break;
            case (7):
              OUTmsg.id  = 0x1A555417;
              break;
            case (8):
              OUTmsg.id  = 0x1A555419;
              break;
            case (9):
              OUTmsg.id  = 0x1A55541B;
              break;
            case (10):
              OUTmsg.id  = 0x1A5554AC;
              break;
            case (11):
              OUTmsg.id  = 0x1A5554AE;
              break;
            case (12):
              OUTmsg.id  = 0x1A5554B0;
              break;

            default:
              break;
          }
          OUTmsg.len = 8;
          OUTmsg.flags.extended = 1;
          Can0.write(OUTmsg);
        }
      }

      if (balancing == false)
      {
        balcnt = 0;
      }
    }
  }
  else
  {
    if (balcnt == 1)
    {
      OUTmsg.buf[0] = 0X00;
      OUTmsg.buf[1] = 0X00;
      OUTmsg.buf[2] = 0X00;
      OUTmsg.buf[3] = 0X00;
      OUTmsg.buf[4] = 0X00;
      OUTmsg.buf[5] = 0X00;
      OUTmsg.buf[6] = 0X00;
      OUTmsg.buf[7] = 0X00;

      OUTmsg.len = 8;
      OUTmsg.flags.extended = 1;

      OUTmsg.id  = 0x1A55540A;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55540C;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55540E;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555410;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555412;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555414;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555416;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555418;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55541A;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554AB;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554AD;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554AF;
      Can0.write(OUTmsg);
      delay(1);


      OUTmsg.buf[0] = 0X00;
      OUTmsg.buf[1] = 0X00;
      OUTmsg.buf[2] = 0X00;
      OUTmsg.buf[3] = 0X00;
      OUTmsg.buf[4] = 0xFE;
      OUTmsg.buf[5] = 0xFE;
      OUTmsg.buf[6] = 0xFE;
      OUTmsg.buf[7] = 0xFE;

      OUTmsg.id  = 0x1A55540B;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55540D;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55540F;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555411;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555413;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555415;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555417;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A555419;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A55541B;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554AC;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554AE;
      Can0.write(OUTmsg);
      delay(1);

      OUTmsg.id  = 0x1A5554B0;
      Can0.write(OUTmsg);
      delay(1);

      balancing = false;
    }
  }
  balcnt++;
  /*
    Serial.print("Bal:");
    Serial.print(balancing);
  */
  OUTmsg.flags.extended = 0;
}




int BMSModuleManager::seriescells()
{
  spack = 0;
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      spack = spack + modules[y].getscells();
    }
  }
  return spack;
}

void BMSModuleManager::clearmodules()
{
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      modules[y].clearmodule();
      modules[y].setExists(false);
      modules[y].setAddress(y);
    }
  }
}

void BMSModuleManager::decodetemp(CAN_message_t &msg, int debug, int type)
{
  int CMU = 0;
  if (type == 1)
  {
    CMU = (msg.id & 0xFF);
    if (CMU > 10 && CMU < 60)
    {
      CMU = CMU & 0x0F;
      CMU = (CMU * 0.5) + 1;
    }
    if (CMU > 0 && CMU < 15);
    {
      modules[CMU].setExists(true);
      modules[CMU].setReset(true);
      modules[CMU].decodetemp(msg, 1);
      if (debug == 1)
      {
        Serial.println();
        Serial.print(CMU);
        Serial.print(" | Temp Found");
      }
    }
  }
  if (type == 2)
  {
    CMU = (msg.id & 0x0F);
    if (CMU > 0 && CMU < 15);
    {
      CMU++;
      if (msg.buf[5] != 0xDF) //Check module is not initializing OR a "spoof module"
      {
        modules[CMU].setExists(true);
        modules[CMU].setReset(true);
        modules[CMU].decodetemp(msg, 2);
        if (debug == 1)
        {
          Serial.println();
          Serial.print(CMU);
          Serial.print("|  Temp Found");
        }
      }
    }
  }
}

void BMSModuleManager::decodecan(CAN_message_t &msg, int debug)
{
  int CMU, Id = 0;
  if (balancing == false)
  {
    switch (msg.id)
    {
      ///////////////// one extender increment//////////

      case (0x1D0):
        CMU = 9;
        Id = 0;
        break;
      case (0x1D1):
        CMU = 9;
        Id = 1;
        break;
      case (0x1D2):
        CMU = 9;
        Id = 2;
        break;
      case (0x1D3):
        CMU = 9;
        Id = 3;
        break;

      case (0x1D4):
        CMU = 10;
        Id = 0;
        break;
      case (0x1D5):
        CMU = 10;
        Id = 1;
        break;
      case (0x1D6):
        CMU = 10;
        Id = 2;
        break;
      case (0x1D8):
        CMU = 11;
        Id = 0;
        break;
      case (0x1D9):
        CMU = 11;
        Id = 1;
        break;
      case (0x1DA):
        CMU = 11;
        Id = 2;
        break;
      case (0x1DC):
        CMU = 12;
        Id = 0;
        break;
      case (0x1DD):
        CMU = 12;
        Id = 1;
        break;
      case (0x1DE):
        CMU = 12;
        Id = 2;
        break;

      case (0x1E0):
        CMU = 13;
        Id = 0;
        break;
      case (0x1E1):
        CMU = 13;
        Id = 1;
        break;
      case (0x1E2):
        CMU = 13;
        Id = 2;
        break;

      case (0x1E4):
        CMU = 14;
        Id = 0;
        break;
      case (0x1E5):
        CMU = 14;
        Id = 1;
        break;
      case (0x1E6):
        CMU = 14;
        Id = 2;
        break;

      case (0x1E8):
        CMU = 15;
        Id = 0;
        break;
      case (0x1E9):
        CMU = 15;
        Id = 1;
        break;
      case (0x1EA):
        CMU = 15;
        Id = 2;
        break;

      case (0x1EC):
        CMU = 16;
        Id = 0;
        break;
      case (0x1ED):
        CMU = 16;
        Id = 1;
        break;
      case (0x1EE):
        CMU = 16;
        Id = 2;
        break;


      ///////////////////////standard ids////////////////


      case (0x1B0):
        CMU = 1;
        Id = 0;
        break;
      case (0x1B1):
        CMU = 1;
        Id = 1;
        break;
      case (0x1B2):
        CMU = 1;
        Id = 2;
        break;
      case (0x1B3):
        CMU = 1;
        Id = 3;
        break;

      case (0x1B4):
        CMU = 2;
        Id = 0;
        break;
      case (0x1B5):
        CMU = 2;
        Id = 1;
        break;
      case (0x1B6):
        CMU = 2;
        Id = 2;
        break;
      case (0x1B7):
        CMU = 2;
        Id = 3;
        break;

      case (0x1B8):
        CMU = 3;
        Id = 0;
        break;
      case (0x1B9):
        CMU = 3;
        Id = 1;
        break;
      case (0x1BA):
        CMU = 3;
        Id = 2;
        break;
      case (0x1BB):
        CMU = 3;
        Id = 3;
        break;

      case (0x1BC):
        CMU = 4;
        Id = 0;
        break;
      case (0x1BD):
        CMU = 4;
        Id = 1;
        break;
      case (0x1BE):
        CMU = 4;
        Id = 2;
        break;
      case (0x1BF):
        CMU = 4;
        Id = 3;
        break;

      case (0x1C0):
        CMU = 5;
        Id = 0;
        break;
      case (0x1C1):
        CMU = 5;
        Id = 1;
        break;
      case (0x1C2):
        CMU = 5;
        Id = 2;
        break;
      case (0x1C3):
        CMU = 5;
        Id = 3;
        break;

      case (0x1C4):
        CMU = 6;
        Id = 0;
        break;
      case (0x1C5):
        CMU = 6;
        Id = 1;
        break;
      case (0x1C6):
        CMU = 6;
        Id = 2;
        break;
      case (0x1C7):
        CMU = 6;
        Id = 3;
        break;

      case (0x1C8):
        CMU = 7;
        Id = 0;
        break;
      case (0x1C9):
        CMU = 7;
        Id = 1;
        break;
      case (0x1CA):
        CMU = 7;
        Id = 2;
        break;
      case (0x1CB):
        CMU = 7;
        Id = 3;
        break;

      case (0x1CC):
        CMU = 8;
        Id = 0;
        break;
      case (0x1CD):
        CMU = 8;
        Id = 1;
        break;
      case (0x1CE):
        CMU = 8;
        Id = 2;
        break;
      case (0x1CF):
        CMU = 8;
        Id = 3;
        break;

      default:
        return;
        break;
    }
    if (CMU > 0 && CMU < 64)
    {
      if (Id < 3)
      {
        if (msg.buf[2] != 0xFF && msg.buf[5] != 0xFF && msg.buf[7] != 0xFF) //Check module is not initializing OR a "spoof module"
        {
          if (debug == 1)
          {
            Serial.println();
            Serial.print(CMU);
            Serial.print(",");
            Serial.print(Id);
            Serial.print(" | ");
          }
          modules[CMU].setExists(true);
          modules[CMU].setReset(true);
          modules[CMU].decodecan(Id, msg);
        }
      }
      else
      {
        if (msg.buf[2] != 0xFF) //Check module is not initializing OR a "spoof module"
        {
          if (debug == 1)
          {
            Serial.println();
            Serial.print(CMU);
            Serial.print(",");
            Serial.print(Id);
            Serial.print(" | ");
          }
          modules[CMU].setExists(true);
          modules[CMU].setReset(true);
          modules[CMU].decodecan(Id, msg);
        }
      }
    }
  }
}


void BMSModuleManager::getAllVoltTemp()
{
  packVolt = 0.0f;

  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      Logger::debug("");
      Logger::debug("Module %i exists. Reading voltage and temperature values", x);
      Logger::debug("Module voltage: %f", modules[x].getModuleVoltage());
      Logger::debug("Lowest Cell V: %f     Highest Cell V: %f", modules[x].getLowCellV(), modules[x].getHighCellV());
      Logger::debug("Temp1: %f       Temp2: %f", modules[x].getTemperature(0), modules[x].getTemperature(1));
      packVolt += modules[x].getModuleVoltage();
      if (modules[x].getLowTemp() < lowestPackTemp) lowestPackTemp = modules[x].getLowTemp();
      if (modules[x].getHighTemp() > highestPackTemp) highestPackTemp = modules[x].getHighTemp();
    }
  }

  packVolt = packVolt / Pstring;
  if (packVolt > highestPackVolt) highestPackVolt = packVolt;
  if (packVolt < lowestPackVolt) lowestPackVolt = packVolt;

  if (digitalRead(11) == LOW) {
    if (!isFaulted) Logger::error("One or more BMS modules have entered the fault state!");
    isFaulted = true;
  }
  else
  {
    if (isFaulted) Logger::info("All modules have exited a faulted state");
    isFaulted = false;
  }
}

float BMSModuleManager::getLowCellVolt()
{
  LowCellVolt = 5.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getLowCellV() <  LowCellVolt)  LowCellVolt = modules[x].getLowCellV();
    }
  }
  return LowCellVolt;
}

float BMSModuleManager::getHighCellVolt()
{
  HighCellVolt = 0.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getHighCellV() >  HighCellVolt)  HighCellVolt = modules[x].getHighCellV();
    }
  }
  return HighCellVolt;
}

float BMSModuleManager::getPackVoltage()
{
  return packVolt;
}

int BMSModuleManager::getNumModules()
{
  return numFoundModules;
}

float BMSModuleManager::getLowVoltage()
{
  return lowestPackVolt;
}

float BMSModuleManager::getHighVoltage()
{
  return highestPackVolt;
}

void BMSModuleManager::setBatteryID(int id)
{
  batteryID = id;
}

void BMSModuleManager::setPstrings(int Pstrings)
{
  Pstring = Pstrings;
}

void BMSModuleManager::setSensors(int sensor, float Ignore, float VoltDelta)
{
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      modules[x].settempsensor(sensor);
      modules[x].setIgnoreCell(Ignore);
      modules[x].setDelta(VoltDelta);
    }
  }
}

float BMSModuleManager::getAvgTemperature()
{
  float avg = 0.0f;
  lowTemp = 999.0f;
  highTemp = -999.0f;
  int y = 0; //counter for modules below -70 (no sensors connected)
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getAvgTemp() > -70)
      {
        avg += modules[x].getAvgTemp();
        if (modules[x].getHighTemp() > highTemp)
        {
          highTemp = modules[x].getHighTemp();
        }
        if (modules[x].getLowTemp() < lowTemp)
        {
          lowTemp = modules[x].getLowTemp();
        }
      }
      else
      {
        y++;
      }
    }
  }
  avg = avg / (float)(numFoundModules - y);

  return avg;
}

float BMSModuleManager::getHighTemperature()
{
  return highTemp;
}

float BMSModuleManager::getLowTemperature()
{
  return lowTemp;
}

float BMSModuleManager::getAvgCellVolt()
{
  numFoundModules = 0;
  float avg = 0.0f;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getAverageV() > 0)
      {
        avg += modules[x].getAverageV();
        numFoundModules++;
      }
    }
  }
  avg = avg / (float)numFoundModules;

  return avg;
}

void BMSModuleManager::printPackSummary()
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;

  Logger::console("");
  Logger::console("");
  Logger::console("");
  Logger::console("Modules: %i  Cells: %i  Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules, seriescells(),
                  getPackVoltage(), getAvgCellVolt(), getAvgTemperature());
  Logger::console("");
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();

      Logger::console("                               Module #%i", y);

      Logger::console("  Voltage: %fV   (%fV-%fV)     Temperatures: (%fC-%fC)", modules[y].getModuleVoltage(),
                      modules[y].getLowCellV(), modules[y].getHighCellV(), modules[y].getLowTemp(), modules[y].getHighTemp());
      if (faults > 0)
      {
        Logger::console("  MODULE IS FAULTED:");
        if (faults & 1)
        {
          SERIALCONSOLE.print("    Overvoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 12; i++)
          {
            if (COV & (1 << i))
            {
              SERIALCONSOLE.print(i + 1);
              SERIALCONSOLE.print(" ");
            }
          }
          SERIALCONSOLE.println();
        }
        if (faults & 2)
        {
          SERIALCONSOLE.print("    Undervoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 12; i++)
          {
            if (CUV & (1 << i))
            {
              SERIALCONSOLE.print(i + 1);
              SERIALCONSOLE.print(" ");
            }
          }
          SERIALCONSOLE.println();
        }
        if (faults & 4)
        {
          Logger::console("    CRC error in received packet");
        }
        if (faults & 8)
        {
          Logger::console("    Power on reset has occurred");
        }
        if (faults & 0x10)
        {
          Logger::console("    Test fault active");
        }
        if (faults & 0x20)
        {
          Logger::console("    Internal registers inconsistent");
        }
      }
      if (alerts > 0)
      {
        Logger::console("  MODULE HAS ALERTS:");
        if (alerts & 1)
        {
          Logger::console("    Over temperature on TS1");
        }
        if (alerts & 2)
        {
          Logger::console("    Over temperature on TS2");
        }
        if (alerts & 4)
        {
          Logger::console("    Sleep mode active");
        }
        if (alerts & 8)
        {
          Logger::console("    Thermal shutdown active");
        }
        if (alerts & 0x10)
        {
          Logger::console("    Test Alert");
        }
        if (alerts & 0x20)
        {
          Logger::console("    OTP EPROM Uncorrectable Error");
        }
        if (alerts & 0x40)
        {
          Logger::console("    GROUP3 Regs Invalid");
        }
        if (alerts & 0x80)
        {
          Logger::console("    Address not registered");
        }
      }
      if (faults > 0 || alerts > 0) SERIALCONSOLE.println();
    }
  }
}

void BMSModuleManager::printPackDetails(int digits)
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;
  int cellNum = 0;

  Logger::console("");
  Logger::console("");
  Logger::console("");
  Logger::console("Modules: %i Cells: %i Strings: %i  Voltage: %fV   Avg Cell Voltage: %fV  Low Cell Voltage: %fV   High Cell Voltage: %fV Delta Voltage: %zmV   Avg Temp: %fC ", numFoundModules, seriescells(),
                  Pstring, getPackVoltage(), getAvgCellVolt(), LowCellVolt, HighCellVolt, (HighCellVolt - LowCellVolt) * 1000, getAvgTemperature());
  Logger::console("");
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();

      SERIALCONSOLE.print("Module #");
      SERIALCONSOLE.print(y);
      if (y < 10) SERIALCONSOLE.print(" ");
      SERIALCONSOLE.print("  ");
      SERIALCONSOLE.print(modules[y].getModuleVoltage(), digits);
      SERIALCONSOLE.print("V");
      for (int i = 0; i < 13; i++)
      {
        if (cellNum < 10) SERIALCONSOLE.print(" ");
        SERIALCONSOLE.print("  Cell");
        SERIALCONSOLE.print(cellNum++);
        SERIALCONSOLE.print(": ");
        SERIALCONSOLE.print(modules[y].getCellVoltage(i), digits);
        SERIALCONSOLE.print("V");
      }
      SERIALCONSOLE.println();
      if (modules[y].getType() == 1)
      {
        SERIALCONSOLE.print(" Temp 1: ");
        SERIALCONSOLE.print(modules[y].getTemperature(0));
        SERIALCONSOLE.print("C Temp 2: ");
        SERIALCONSOLE.print(modules[y].getTemperature(1));
        SERIALCONSOLE.print("C Temp 3: ");
        SERIALCONSOLE.print(modules[y].getTemperature(2));
        SERIALCONSOLE.print("C | Bal Stat: ");
        SERIALCONSOLE.println(modules[y].getBalStat(), HEX);
      }
      else
      {
        SERIALCONSOLE.print(" Temp 1: ");
        SERIALCONSOLE.print(modules[y].getTemperature(0));
        SERIALCONSOLE.print("C | Bal Stat: ");
        SERIALCONSOLE.println(modules[y].getBalStat(), HEX);
      }
    }
  }
}
void BMSModuleManager::printAllCSV(unsigned long timestamp, float current, int SOC)
{
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      SERIALCONSOLE.print(timestamp);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(current, 0);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(SOC);
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(y);
      SERIALCONSOLE.print(",");
      for (int i = 0; i < 8; i++)
      {
        SERIALCONSOLE.print(modules[y].getCellVoltage(i));
        SERIALCONSOLE.print(",");
      }
      SERIALCONSOLE.print(modules[y].getTemperature(0));
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(modules[y].getTemperature(1));
      SERIALCONSOLE.print(",");
      SERIALCONSOLE.print(modules[y].getTemperature(2));
      SERIALCONSOLE.println();
    }
  }
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      Serial2.print(timestamp);
      Serial2.print(",");
      Serial2.print(current, 0);
      Serial2.print(",");
      Serial2.print(SOC);
      Serial2.print(",");
      Serial2.print(y);
      Serial2.print(",");
      for (int i = 0; i < 13; i++)
      {
        Serial2.print(modules[y].getCellVoltage(i));
        Serial2.print(",");
      }
      if (modules[y].getType() == 1)
      {
        Serial2.print(modules[y].getTemperature(0));
        Serial2.print(",");
        Serial2.print(modules[y].getTemperature(1));
        Serial2.print(",");
        Serial2.print(modules[y].getTemperature(2));
        Serial2.println();
      }
      else
      {
        Serial2.print(modules[y].getTemperature(0));
      }
    }
  }
}

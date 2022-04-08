#include <SPI.h>
#include <mcp2515.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "Dashboard_Data.h"
#include "Main_Display.h"
#include "Project_Functions.h"
#include "ETS_Parse.h"

#include <TaskScheduler.h>
Scheduler DAF;

Task TS_Dashboard_Main(40, TASK_FOREVER, &Dashboard_Main_Callback);
Task TS_Right_Display(50, TASK_FOREVER, &Right_Display_Callback);
Task TS_Icons(100, TASK_FOREVER, &Icons_Callback);
Task TS_Speedometer(50, TASK_FOREVER, &Speedometer_Callback);
Task TS_RPM(50, TASK_FOREVER, &RPM_Callback);
Task TS_Distances(1000, TASK_FOREVER, &Distances_Callback);
Task TS_Arrows(50, TASK_FOREVER, &Arrows_Callback);
Task TS_Time(1000, TASK_FOREVER, &Time_Callback);
Task TS_MileageReset(500, TASK_FOREVER, &Mileage_Read_Callback);

#define I_CAN_CS 8
#define V_CAN_CS 9

MCP2515 ICAN(I_CAN_CS);
MCP2515 VCAN(V_CAN_CS);

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  SPI.begin();

  ICAN.reset();
  ICAN.setBitrate(CAN_250KBPS, MCP_8MHZ);
  ICAN.setNormalMode();

  VCAN.reset();
  VCAN.setBitrate(CAN_250KBPS, MCP_8MHZ);
  VCAN.setNormalMode();




  DAF.addTask(TS_Dashboard_Main);
  TS_Dashboard_Main.enable();

  DAF.addTask(TS_Right_Display);
  TS_Right_Display.enable();

  DAF.addTask(TS_Icons);
  TS_Icons.enable();

  DAF.addTask(TS_Speedometer);
  TS_Speedometer.enable();

  DAF.addTask(TS_RPM);
  TS_RPM.enable();

  DAF.addTask(TS_Distances);
  TS_Distances.enable();

  DAF.addTask(TS_Arrows);
  TS_Arrows.enable();

  DAF.addTask(TS_Time);
  TS_Time.enable();

  DAF.addTask(TS_MileageReset);
  TS_MileageReset.enable();

  //Ignition = true;
  DisplayStartUp = true;
  DisplayActive = true;
  //EngineOn = false;

  //ToggleIcon(COOLANT_LEVEL, WARNING, 1);
  //ToggleIcon(EBS_TRUCK, ALARM, 1);
  //ToggleIcon(ENGINE, ALARM, 1);
  //ToggleIcon(ENGINE, WARNING, 1);
}

long timer2 = 0;

void loop()
{
  ParseIncoming();

  if (millis() - DisplayUpperTimer > DisplayUpperDelay && DisplayActive)
  {
    DisplayUpperFrame.can_id  = 0x18FF3527 | CAN_EFF_FLAG;
    DisplayUpperFrame.can_dlc = 8;

    FillDisplayUpperArray();

    DisplayUpperCnt++;
    if (DisplayUpperCnt > 3)
      DisplayUpperCnt = 0;

    for (byte cnt = 0; cnt < 8; cnt++)
    {
      DisplayUpperFrame.data[cnt] = DisplayUpperArray[DisplayUpperCnt][cnt];
    }

    ICAN.sendMessage(&DisplayUpperFrame);

    DisplayUpperTimer = millis();

    if (DisplayUpperCnt == 2)
      DisplayUpperDelay = 30;
    else
      DisplayUpperDelay = 10;
  }
  if (!DisplayActive)
    DisplayUpperCnt = 0;


  if (millis() - timer2 > 700)
  {
    timer2 = millis();
    counter++;
    //Speed = counter;
    //CruiseControl = counter;
    //DisplayTurbo[0][4] = counter;

    //Serial.println(counter, HEX);
  }

  /* Text mode icons
     0х10 чувак
     трасса км
     трасса мол
     трасса
     спальник
     сервис
     часи
     фура
     работа
     руль
     заправка

  */

  if (millis() - DisplayStratupTimer > DisplayStratupDelay && DisplayStartUp)
  {
    if (DisplayPage < 2 || DisplayPage == 2 && DisplayStartupIconCnt == 9)
      DisplayPage++;

    if (DisplayPage == 1)
    {
      if (DisplayAlarmCount != 0 || DisplayWarningsCount != 0)
      {
        DisplayStartupStage1_FillArray(); // Filling up 3727 Array
        DisplayCnt = 0;
        DisplayArraySize = 3;

        DisplayStratupTimer = millis();
        DisplayStratupDelay = 3000;
      }
      else
        DisplayStratupDelay = 0;
    }
    else if (DisplayPage == 2) // Showing warnings one by one with 4 sec duration for each
    {
      DisplayCnt = 0;
      DisplayArraySize = 7;

      byte IconID = DisplayIcons[DisplayStartupIconCnt];
      if (IconID != 0 && IconID != 1 && IconID != 7)
      {
        Serial.println(IconID);
        // Copying message to buffer
        if (DisplayStartupIconCnt < 3) // Alarms
          DisplayIconMessage_FillArray(IconID, ALARM);
        else // Warnings
          DisplayIconMessage_FillArray(IconID, WARNING);

        DisplayStratupDelay = 4000;
      }
      else
        DisplayStratupDelay = 0;

      DisplayStartupIconCnt++;
      DisplayStratupTimer = millis();
    }
    else if (DisplayPage == 3)
    {
      DisplayCnt = 0;
      DisplayArraySize = 8;

      DisplayStartUp = false;
      DisplayStartupIconCnt = 0;
    }
  }

  if (millis() - DispTimer > 10)
  {
    DisplayOther.can_id  = 0x18FF3727 | CAN_EFF_FLAG;
    DisplayOther.can_dlc = 8;

    DisplayCnt++;
    if (DisplayCnt > DisplayArraySize - 1)
      DisplayCnt = 0;

    for (byte cnt = 0; cnt < 8; cnt++)
    {
      if (DisplayPage == 1)
        DisplayOther.data[cnt] = DisplayStartupStage1[DisplayCnt][cnt];
      else if (DisplayPage == 2)
        DisplayOther.data[cnt] = DisplayStartupStage2[DisplayCnt][cnt];
      else
      {
        if (DispPage == 0)
        {
          DisplayOther.data[cnt] = pgm_read_byte(&DisplayBlank[DisplayCnt][cnt]);
        }
        else if (DispPage == 1)
        {
          DisplayFuelUsage_FillArray();
          DisplayOther.data[cnt] = DisplayFuelUsage[DisplayCnt][cnt];
        }
        else if (DispPage == 2)
        {
          DisplayFuelInfo_FillArray();
          DisplayOther.data[cnt] = DisplayFuelInfo[DisplayCnt][cnt];
        }
        else if (DispPage == 3)
        {
          DisplayOther.data[cnt] = DisplayTurbo[DisplayCnt][cnt];
        }
        else if (DispPage == 4)
        {
          DisplaySpeedInfo_FillArray();
          DisplayOther.data[cnt] = DisplaySpeedInfo[DisplayCnt][cnt];
        }
        else if (DispPage == 5)
        {
          DisplayTemperature_FillArray();
          DisplayOther.data[cnt] = DisplayTemperature[DisplayCnt][cnt];
        }
      }
    }

    ICAN.sendMessage(&DisplayOther);
    DispTimer = millis();
  }

  DAF.execute();
}

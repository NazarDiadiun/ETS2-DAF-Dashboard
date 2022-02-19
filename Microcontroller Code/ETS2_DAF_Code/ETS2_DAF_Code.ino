#include <SPI.h>
#include <mcp2515.h>
#include <EEPROM.h>

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

 // Ignition = true;
  DisplayActive = true;
  //EngineOn = false;

  //ToggleIcon(ASR, WARNING, 1);
}

long timer = 0;

long timer2 = 0;
byte counter = 0;

byte DisplayCounter1 = 0;
struct can_frame Main_Display1;
struct can_frame Main_Display3;
void loop()
{
  ParseIncoming();

  if (millis() - timer2 > 1500)
  {
    timer2 = millis();
    counter++;
    //Serial.println(counter);
  }
  if (millis() - timer > 40 && DisplayCounter1 == 0 || millis() - timer >= 10 && DisplayCounter1 == 1 || millis() - timer >= 10 && DisplayCounter1 == 2)
  {
    byte WarnStartIndex = 3;
    // Checking for active warnings or alarms
    if (DisplayIcons[0] == 0 && DisplayIcons[1] == 0 && DisplayIcons[2] == 0)
      ActiveAlarm = false;
    else
      ActiveAlarm = true;

    if (DisplayIcons[3] == 0 && DisplayIcons[4] == 0 && DisplayIcons[5] == 0 && DisplayIcons[6] == 0 && DisplayIcons[7] == 0 && DisplayIcons[8] == 0)
      ActiveWarn = false;
    else
      ActiveWarn = true;

    //setting color for display (off/off, yellow/off, red/off, red/yellow, yellow/yellow)
    if (ActiveAlarm)
    {
      DisplayUpAlarm = true;
      DisplayUpWarn = false;

      if (ActiveWarn) // if alarm is on and some warning message are displaying
        WarnStartIndex = 6; // display first 3 of them (3 left is hidden)
    }
    else
    {
      DisplayUpAlarm = false;
      if (ActiveWarn)
      {
        if (DisplayIcons[3] != 0 || DisplayIcons[4] != 0 || DisplayIcons[5] != 0)
          DisplayUpWarn = true;
      }
      else
        DisplayUpWarn = false;
    }
    if (ActiveWarn)
    {
      if (ActiveAlarm || DisplayIcons[6] != 0 || DisplayIcons[7] != 0 || DisplayIcons[8] != 0)
        DisplayDownWarn = true;
      else
        DisplayDownWarn = false;
    }

    //Ignition = !Ignition;
    if (DisplayActive)
    {
      
      Main_Display1.can_id  = 0x18FF3527 | CAN_EFF_FLAG;
      Main_Display1.can_dlc = 8;

      Main_Display3.can_id  = 0x18FF3727 | CAN_EFF_FLAG;
      Main_Display3.can_dlc = 8;

      cnt_Msg_count();

      if (DisplayCounter1 == 0)
      {
        Main_Display3.data[0] = 0x10;
        Main_Display3.data[1] = 0x0E;

        Main_Display3.data[2] = 0x01;

        //DisplayDownWarn = true;
        //DisplayUpWarn = true;
        Main_Display3.data[3] = (DisplayDownWarn << 7) | (DisplayUpWarn << 5) | (DisplayUpAlarm << 4);
        Main_Display3.data[4] = 0x01; // enable icons ?
        Main_Display3.data[5] = (SoundAlarmOnce << 0) | (SoundAlarmCont << 1) | (SoundWarnOnce << 2);

        // Alarm 1 icon
        Main_Display3.data[6] = 0x01;

        if (ActiveAlarm)
          Main_Display3.data[7] = DisplayIcons[0];    // ALARM 1
        else
          Main_Display3.data[7] = DisplayIcons[3];    // ALARM 1

        Main_Display1.data[0] = 0x10;
        Main_Display1.data[1] = 0x0F;
        Main_Display1.data[2] = 0x0A;

        // UpBar zone. 0000 xyza: x - yellow right part, y - red right part (no red color), z - yellow left corner, a - red left corner
        bool DisplayUpBarAlarm = false;

        if (DisplayAlarmCount != 0)
          DisplayUpBarAlarm = true;

        Main_Display1.data[3] = (1 << 3) | (!DisplayUpBarAlarm << 1) | (DisplayUpBarAlarm << 0);

        Main_Display1.data[4] = 0x01;
        Main_Display1.data[5] = 0x00;
        if (DisplayAlarmCount != 0)
        {
          Main_Display1.data[6] = 0x81;
          Main_Display1.data[7] = 48 + DisplayAlarmCount;
        }
        else
        {
          Main_Display1.data[6] = 0x01;
          Main_Display1.data[7] = 32;
        }

        DisplayCounter1 = 1;
      }
      else if (DisplayCounter1 == 1)
      {
        // Icons Alarm 2 - warn 1
        Main_Display3.data[0] = 0x21;
        Main_Display3.data[1] = 0x01;
        Main_Display3.data[3] = 0x01;
        Main_Display3.data[5] = 0x01;
        Main_Display3.data[7] = 0x00;

        if (ActiveAlarm)
        {
          Main_Display3.data[2] = DisplayIcons[1];  // ALARM 2
          Main_Display3.data[4] = DisplayIcons[2];  // ALARM 3
          Main_Display3.data[6] = DisplayIcons[3]; // WARN 1
        }
        else
        {
          Main_Display3.data[2] = DisplayIcons[4];  // ALARM 2
          Main_Display3.data[4] = DisplayIcons[5];  // ALARM 3
          Main_Display3.data[6] = DisplayIcons[6]; // WARN 1
        }


        Main_Display1.data[0] = 0x21;

        if (DisplayWarningsCount != 0)
        {
          Main_Display1.data[1] = 0x81;
          Main_Display1.data[2] = 48 + DisplayWarningsCount;
        }
        else
        {
          Main_Display1.data[1] = 0x01;
          Main_Display1.data[2] = 32;
        }

        Main_Display1.data[3] = 0x00; // first number of gear
        Main_Display1.data[4] = 0x01;
        Main_Display1.data[6] = 0x01;
        // 13 - R; 14 - N; 15 - D;

        if (Gear == 0) // if neutral gear
        {
          // Displaying 'N' symbol. First number disabled
          Main_Display1.data[5] = 20; // first number of gear
          Main_Display1.data[7] = 14; // Second number
        }
        else if (Gear < 0) // if reverse
        {
          // displaying 'R' symbol and gear number. example R1, R2 etc
          Main_Display1.data[5] = 13; // first number of geat
          Main_Display1.data[7] = abs(Gear); // Second number
        }
        else if (Gear > 0)
        {
          // Displaying current gear
          Main_Display1.data[5] = Gear / 10; // first number of gear

          byte secondNumber = Gear % 10; // second number is mod from gear
          if (secondNumber == 0) // if second number is zero
            Main_Display1.data[7] = 10; // putting here 10 (because 0 = disabled number)
          else // else if second number is 1-9
            Main_Display1.data[7] = secondNumber; // just displaying it here
        }
        DisplayCounter1 = 2;
      }
      else if (DisplayCounter1 == 2)
      {
        Main_Display3.data[0] = 0x22;
        Main_Display3.data[2] = 0x01;

        if (ActiveAlarm)
        {
          Main_Display3.data[1] = DisplayIcons[4]; // WARN 2
          Main_Display3.data[3] = DisplayIcons[5]; // WARN 3
        }
        else
        {
          Main_Display3.data[1] = DisplayIcons[7]; // WARN 2
          Main_Display3.data[3] = DisplayIcons[8]; // WARN 3
        }

        Main_Display3.data[4] = 0x72;
        Main_Display3.data[5] = 0x65;
        Main_Display3.data[6] = 0x73;
        Main_Display3.data[7] = 0x73;

        Main_Display1.data[0] = 0x22;

        //(Transmission, turtle, auto, Manual, parking, gear stick)
        Main_Display1.data[1] = 1;
        if (Gear > 0)
          Main_Display1.data[2] = 3;
        else
          Main_Display1.data[2] = 0;

        Main_Display1.data[3] = 14;
        Main_Display1.data[4] = 14;
        Main_Display1.data[5] = 14;
        Main_Display1.data[6] = 14;
        Main_Display1.data[7] = 14;
        DisplayCounter1 = 0;
      }

      ICAN.sendMessage(&Main_Display3);
      ICAN.sendMessage(&Main_Display1);
      timer = millis();
    }
  }

  DAF.execute();
}

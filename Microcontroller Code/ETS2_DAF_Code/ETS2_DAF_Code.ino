#include <SPI.h>
#include <mcp2515.h>
#include <EEPROM.h>

#include "Dashboard_Data.h"
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

  Ignition = true;
  DisplayActive = true;
  EngineOn = true;
}

long timer = 0;

byte DisplayCounter1 = 0;
struct can_frame Main_Display1;
struct can_frame Main_Display3;
void loop()
{
  ParseIncoming();

  if (millis() - timer > 40 && DisplayCounter1 == 0 || millis() - timer >= 10 && DisplayCounter1 == 1 || millis() - timer >= 10 && DisplayCounter1 == 2)
  {
    //Ignition = !Ignition;
    if (DisplayActive)
    {
      Main_Display1.can_id  = 0x18FF3527 | CAN_EFF_FLAG;
      Main_Display1.can_dlc = 8;

      Main_Display3.can_id  = 0x18FF3727 | CAN_EFF_FLAG;
      Main_Display3.can_dlc = 8;

      if (DisplayCounter1 == 0)
      {
        Main_Display3.data[0] = 0x10;
        Main_Display3.data[1] = 0x0E;
        Main_Display3.data[2] = 0x01;
        Main_Display3.data[3] = 0x90;
        Main_Display3.data[4] = 0x01;
        Main_Display3.data[5] = 0x00;
        Main_Display3.data[6] = 0x01;
        Main_Display3.data[7] = 0x01;

        Main_Display1.data[0] = 0x10;
        Main_Display1.data[1] = 0x0F;
        Main_Display1.data[2] = 0x0A;
        Main_Display1.data[3] = 0x0A;
        Main_Display1.data[4] = 0x01;
        Main_Display1.data[5] = 0x00;
        Main_Display1.data[6] = 0x00;
        Main_Display1.data[7] = 0x00;
        DisplayCounter1 = 1;
      }
      else if (DisplayCounter1 == 1)
      {
        Main_Display3.data[0] = 0x21;
        Main_Display3.data[1] = 0x01;
        Main_Display3.data[2] = 0x07;
        Main_Display3.data[3] = 0x01;
        Main_Display3.data[4] = 0x0D;
        Main_Display3.data[5] = 0x01;
        Main_Display3.data[6] = 0x0D;
        Main_Display3.data[7] = 0x00;

        Main_Display1.data[0] = 0x21;
        Main_Display1.data[1] = 0x00;
        Main_Display1.data[2] = 49;//48 + DisplayWarningsCount;
        Main_Display1.data[3] = 0x00;
        Main_Display1.data[4] = 0x00;
        Main_Display1.data[5] = 0x00;
        Main_Display1.data[6] = 0x00;
        Main_Display1.data[7] = 0x00;
        DisplayCounter1 = 2;
      }
      else if (DisplayCounter1 == 2)
      {
        Main_Display3.data[0] = 0x22;
        Main_Display3.data[1] = 0x00;
        Main_Display3.data[2] = 0x20;
        Main_Display3.data[3] = 0x70;
        Main_Display3.data[4] = 0x72;
        Main_Display3.data[5] = 0x65;
        Main_Display3.data[6] = 0x73;
        Main_Display3.data[7] = 0x73;

        Main_Display1.data[0] = 0x22;
        Main_Display1.data[1] = 0x00;
        Main_Display1.data[2] = 0x00;
        Main_Display1.data[3] = 0x00;
        Main_Display1.data[4] = 0x00;
        Main_Display1.data[5] = 0x00;
        Main_Display1.data[6] = 0x00;
        Main_Display1.data[7] = 0x00;
        DisplayCounter1 = 0;
      }

      if (!EngineOn)
        ICAN.sendMessage(&Main_Display3);
      ICAN.sendMessage(&Main_Display1);
      timer = millis();
    }
  }

  DAF.execute();
}

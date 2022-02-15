struct can_frame Speedometer;
struct can_frame RPM;
struct can_frame Distances;
struct can_frame MileageReset;

void Speedometer_Callback()
{
  Speedometer.can_id  = 0x0CFE6CEE | CAN_EFF_FLAG;
  Speedometer.can_dlc = 8;

  Speedometer.data[0] = 0x00;
  Speedometer.data[1] = 0x00;
  Speedometer.data[2] = 0x00;
  Speedometer.data[3] = IndicatorTacho;
  Speedometer.data[4] = 0x00;
  Speedometer.data[5] = 0x00;

  unsigned int SpeedConverted = Speed * 256;
  Speedometer.data[6] = lowByte(SpeedConverted);
  Speedometer.data[7] = highByte(SpeedConverted);

  VCAN.sendMessage(&Speedometer);
}

void RPM_Callback()
{
  RPM.can_id  = 0x0CF00400 | CAN_EFF_FLAG;
  RPM.can_dlc = 8;

  unsigned int RPMsConv = EngineSpeed / 0.125;

  RPM.data[0] = 0x00;
  RPM.data[1] = 0x00;
  RPM.data[2] = 0x00;
  RPM.data[3] = lowByte(RPMsConv);
  RPM.data[4] = highByte(RPMsConv);
  RPM.data[5] = 0x00;
  RPM.data[6] = 0x00;
  RPM.data[7] = 0x00;

  VCAN.sendMessage(&RPM);
}

void Distances_Callback()
{
  Distances.can_id  = 0x18FEC1EE | CAN_EFF_FLAG;
  Distances.can_dlc = 8;

  unsigned long DistanceMultiplied = Distance * 200;
  unsigned long DistanceDailyMultiplied = DistanceDaily * 200;

  byte DistanceConverted[4] = {0};
  byte DistanceDailyConverted[4] = {0};
  for (int i = 0; i < 4; i++)
  {
    DistanceConverted[i] = (DistanceMultiplied >> (i * 8)) & 0xff;
    DistanceDailyConverted[i] = (DistanceDailyMultiplied >> (i * 8)) & 0xff;
  }

  Distances.data[0] = DistanceConverted[0];
  Distances.data[1] = DistanceConverted[1];
  Distances.data[2] = DistanceConverted[2];
  Distances.data[3] = DistanceConverted[3];
  Distances.data[4] = DistanceDailyConverted[0];
  Distances.data[5] = DistanceDailyConverted[1];
  Distances.data[6] = DistanceDailyConverted[2];
  Distances.data[7] = DistanceDailyConverted[3];

  VCAN.sendMessage(&Distances);
}

void Mileage_Read_Callback()
{
  if (VCAN.readMessage(&MileageReset) == MCP2515::ERROR_OK)
  {
    if (MileageReset.can_id == 0x98DEEE17)
    {
      if (MileageReset.data[0])
      {
        DistanceDaily = 0.0;
        DistanceEEPROM = Distance;
        EEPROM.put(5, DistanceEEPROM);
        Serial.print("MileageReset\n");
      }
    }
  }
}

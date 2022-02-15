
struct can_frame Dashboard_Main;
struct can_frame Right_Display;
struct can_frame Icons;
struct can_frame Arrows;
struct can_frame Time;

void Dashboard_Main_Callback()
{
  Dashboard_Main.can_id  = 0x18FF3327 | CAN_EFF_FLAG;
  Dashboard_Main.can_dlc = 8;

  Dashboard_Main.data[0] = !Ignition | (Ignition << 2);
  Dashboard_Main.data[1] = Dashboard_Backlight * 255 / 100;
  Dashboard_Main.data[2] = MiniDisplays_Backlight * 255 / 100;
  Dashboard_Main.data[3] = Icons_Backlight * 255 / 100;
  Dashboard_Main.data[4] = 0xFA;
  Dashboard_Main.data[5] = 0xFF;
  Dashboard_Main.data[6] = 0xFF;
  Dashboard_Main.data[7] = Sound_Loudness * 255 / 100;

  ICAN.sendMessage(&Dashboard_Main);
}

void Right_Display_Callback()
{
  Right_Display.can_id  = 0x18FF3427 | CAN_EFF_FLAG;
  Right_Display.can_dlc = 8;

  bool NegativeTemperature = false;
  if (OutsideTemperature < 0.0)
    NegativeTemperature = true;

  Right_Display.data[0] = 1 << 0 | (1 << 6) | (!NegativeTemperature << 4);
  Right_Display.data[1] = 0x20;

  int OutsideTemperatureInt = OutsideTemperature * 10.0; // Умножаем температуру на 10, чтобы перевести в целочисленное значение.
  OutsideTemperatureInt = abs(OutsideTemperatureInt);

  byte Temperature_FirstNumber = OutsideTemperatureInt / 100;
  byte Temperature_SecondNumber = (OutsideTemperatureInt % 100) / 10;
  byte Temperature_PointNumber = OutsideTemperatureInt % 10;

  if (OutsideTemperatureInt >= 100)
    Right_Display.data[2] = (Temperature_FirstNumber << 0) | (1 << 5) | (1 << 4);
  else
    Right_Display.data[2] = 0x20;

  Right_Display.data[3] = (Temperature_SecondNumber << 0) | (1 << 5) | (1 << 4);
  Right_Display.data[4] = (Temperature_PointNumber << 0) | (1 << 5) | (1 << 4);

  Right_Display.data[5] = 0x00;
  Right_Display.data[6] = 0x00;
  Right_Display.data[7] = 0xFF;

  ICAN.sendMessage(&Right_Display);
}

void Icons_Callback()
{
  Icons.can_id  = 0x18FF3927 | CAN_EFF_FLAG;
  Icons.can_dlc = 8;

  // Обработка прерывчастого сигнала предупреждения
  if (millis() - WarnTimer > 500 && Warning || millis() - WarnTimer > 500 && Alarm) // Если активен сигнал предупреждения, и прошло 350 мс цикл
  {
    if (Warning)
      SoundWarnOnce = !SoundWarnOnce; // Инвертируем значение
    if (Alarm)
      SoundAlarmOnce = !SoundAlarmOnce;

    WarnTimer = millis(); // Сбрасываем таймер
  }

  Icons.data[0] = (IndicatorLowFuel << 2);
  Icons.data[1] = IconLeftTruck | (IconLeftTrailer << 2) | (IconHighBeam << 4) | (IconStop << 6);
  Icons.data[2] = IconLowGear | (IconRightTrailer << 2) | (IconRightTruck << 4) | (IconRetarder << 6);
  Icons.data[3] = IconWindowCycleArrow | (IconCoil << 2) | (IconAxisBlock << 4) | (IconPTO << 6);
  Icons.data[4] = IconASR | (IconFogFront << 2) | (IconFogRear << 4) | (IconParkingBrake << 6);
  Icons.data[5] = IconChassisTransport | (IconWorkLight << 2) | (IconTrailerAbs << 4);
  Icons.data[6] = SoundAlarmOnce | (SoundAlarmCont << 2) | (SoundWarnOnce << 4) | (SoundWarnCont << 6);
  Icons.data[7] = (SoundBlinker << 6) | (!SoundBlinker << 4);

  ICAN.sendMessage(&Icons);
}

void Arrows_Callback()
{
  Arrows.can_id  = 0x18FF3227 | CAN_EFF_FLAG;
  Arrows.can_dlc = 8;

  // Fuel 0x00 - 0x23 HEX
  byte FuelHigh = Fuel * 35.0 / 100;
  byte FuelLow = (Fuel * 35.0 / 100 - FuelHigh) * 100;

  int WaterTemperatureConverted = WaterTemperature;

  if (WaterTemperature >= 30 && WaterTemperature <= 110)
    WaterTemperatureConverted = WaterTemperature;
  else if (WaterTemperature > 110)
    WaterTemperatureConverted = 110;
  else if (WaterTemperature < 30)
    WaterTemperatureConverted = 30;

  if (WaterTemperatureConverted >= 30 && WaterTemperatureConverted < 40)
    WaterTemperatureConverted = map(WaterTemperatureConverted, 30, 70, 0, 100);
  else
    WaterTemperatureConverted = map(WaterTemperatureConverted, 30, 110, 0, 100);

  byte TempHigh = WaterTemperatureConverted * 35.0 / 100;
  byte TempLow = (WaterTemperatureConverted * 35.0 / 100 - TempHigh) * 100;

  // Air pressure bar from 0 to 100%
  byte Air1High = Air1 * 35.0 / 100;
  byte Air1Low = (Air1 * 35.0 / 100 - Air1High) * 100;
  byte Air2High = Air2 * 35.0 / 100;
  byte Air2Low = (Air2 * 35.0 / 100 - Air2High) * 100;

  Arrows.data[0] = Air1Low;
  Arrows.data[1] = Air1High; // 1 contour
  Arrows.data[2] = Air2Low;
  Arrows.data[3] = Air2High; // 2 contour
  Arrows.data[4] = FuelLow;
  Arrows.data[5] = FuelHigh; // fuel
  Arrows.data[6] = TempLow;
  Arrows.data[7] = TempHigh + 1;

  ICAN.sendMessage(&Arrows);
}

void Time_Callback()
{
  Time.can_id  = 0x18FEE627 | CAN_EFF_FLAG;
  Time.can_dlc = 8;

  Time.data[0] = 0x00;
  Time.data[1] = TimeMinutes;
  Time.data[2] = TimeHours;
  Time.data[3] = 0x00;
  Time.data[4] = 0x00;
  Time.data[5] = 0x00;
  Time.data[6] = 0x7D;
  Time.data[7] = 0x7D;

  ICAN.sendMessage(&Time);
}

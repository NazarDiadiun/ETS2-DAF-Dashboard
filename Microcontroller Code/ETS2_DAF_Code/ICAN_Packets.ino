struct can_frame Dashboard_Main; // CAN Packet Structure for basic dashboard functions (ignition, backlight etc)
struct can_frame Right_Display; // CAN Packet Structure for right display (Time, Temperature)
struct can_frame Icons; // CAN Packet Structure for icons control (blinkers, lights etc)
struct can_frame Arrows; // CAN Packet Structure to control all gauges except tachometer and speedometer (there are in V-CAN Bus)
struct can_frame Time; // CAN Packet Structure to show time on right display

void Dashboard_Main_Callback() // Dashboard Main Callback
{
  Dashboard_Main.can_id  = 0x18FF3327 | CAN_EFF_FLAG; // CAN ID: 0x18FF3327
  Dashboard_Main.can_dlc = 8;

  Dashboard_Main.data[0] = !Ignition | (Ignition << 2); // Here is Ignition state (to turn on or off dashboard)
  Dashboard_Main.data[1] = Dashboard_Backlight * 255 / 100; // Dashboard backlight (gauges, needles)
  Dashboard_Main.data[2] = MiniDisplays_Backlight * 255 / 100; // Right and Left displays backlight
  Dashboard_Main.data[3] = Icons_Backlight * 255 / 100; // icons brightness
  Dashboard_Main.data[4] = 0xFA;
  Dashboard_Main.data[5] = 0xFF;
  Dashboard_Main.data[6] = 0xFF;
  Dashboard_Main.data[7] = Sound_Loudness * 255 / 100; // Sound loudness

  ICAN.sendMessage(&Dashboard_Main); // Send this packet to I-CAN Bus
}

void Right_Display_Callback()
{
  Right_Display.can_id  = 0x18FF3427 | CAN_EFF_FLAG; // CAN ID: 0x18FF3427
  Right_Display.can_dlc = 8;

  bool NegativeTemperature = false; // Negative outside temperature flag
  if (OutsideTemperature < 0.0) // if outside temperature lower than 0.0
    NegativeTemperature = true; // make neg.temp. flag true

  Right_Display.data[0] = 1 << 0 | (1 << 6) | (!NegativeTemperature << 4); // Placing negative temperature sign to 0 byte 
  Right_Display.data[1] = 0x20; // First number (can be only 1 - for farenheit, i think)

  int OutsideTemperatureInt = OutsideTemperature * 10.0; // Multiple temperature by 10 to get integer (and save decimal, but x10)
  OutsideTemperatureInt = abs(OutsideTemperatureInt); // make it absolute

  byte Temperature_FirstNumber = OutsideTemperatureInt / 100; // First number of temperature - just divide by 100
  byte Temperature_SecondNumber = (OutsideTemperatureInt % 100) / 10; // Second number - modulo from 100 and divide by 10
  byte Temperature_PointNumber = OutsideTemperatureInt % 10; // last number (decimal) - modulo from 10

  if (OutsideTemperatureInt >= 100) // if temperature higher than 10.0 (because recently we multiplied it by 10)
    Right_Display.data[2] = (Temperature_FirstNumber << 0) | (1 << 5) | (1 << 4); // We need to turn this number firstly and than show it
  else // else if temperature less than 10.0
    Right_Display.data[2] = 0x20; // Disable this number

  Right_Display.data[3] = (Temperature_SecondNumber << 0) | (1 << 5) | (1 << 4); // Turn on and show 2 number
  Right_Display.data[4] = (Temperature_PointNumber << 0) | (1 << 5) | (1 << 4); // And decimal 

  Right_Display.data[5] = 0x00;
  Right_Display.data[6] = 0x00;
  Right_Display.data[7] = 0xFF;

  ICAN.sendMessage(&Right_Display); // Send this packet to I-CAN Bus
}

void Icons_Callback()
{
  Icons.can_id  = 0x18FF3927 | CAN_EFF_FLAG; // CAN ID: 0x18FF3927
  Icons.can_dlc = 8;

  Icons.data[0] = (IndicatorLowFuel << 2); // Low fuel indicator here
  Icons.data[1] = IconLeftTruck | (IconLeftTrailer << 2) | (IconHighBeam << 4) | (IconStop << 6); // Left brinkers. High beam and STOP sign icons
  Icons.data[2] = IconLowGear | (IconRightTrailer << 2) | (IconRightTruck << 4) | (IconRetarder << 6); // Low shift gear, right blinker and retarder here
  Icons.data[3] = IconWindowCycleArrow | (IconCoil << 2) | (IconAxisBlock << 4) | (IconPTO << 6); // idn what window with arrow in it means, but yeah. And Coil, differential block and PTO icons here
  Icons.data[4] = IconASR | (IconFogFront << 2) | (IconFogRear << 4) | (IconParkingBrake << 6); // ASR, Fog light front and rear and parking brakes
  Icons.data[5] = IconChassisTransport | (IconWorkLight << 2) | (IconTrailerAbs << 4); // Chassis not in transport, Work lights on, and trailer ABS icons
  Icons.data[6] = 0x00; 
  Icons.data[7] = (SoundBlinker << 6) | (!SoundBlinker << 4); // Blinker sound

  ICAN.sendMessage(&Icons); // Send this packet to I-CAN Bus
}

void Arrows_Callback()
{
  Arrows.can_id  = 0x18FF3227 | CAN_EFF_FLAG; // CAN ID: 0x18FF3227
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

  ICAN.sendMessage(&Arrows); // Send this packet to I-CAN Bus
}

void Time_Callback()
{
  Time.can_id  = 0x18FEE627 | CAN_EFF_FLAG; // CAN ID: 0x18FEE627
  Time.can_dlc = 8;

  Time.data[0] = 0x00;
  Time.data[1] = TimeMinutes;
  Time.data[2] = TimeHours;
  Time.data[3] = 0x00;
  Time.data[4] = 0x00;
  Time.data[5] = 0x00;
  Time.data[6] = 0x7D;
  Time.data[7] = 0x7D;

  ICAN.sendMessage(&Time); // Send this packet to I-CAN Bus
}

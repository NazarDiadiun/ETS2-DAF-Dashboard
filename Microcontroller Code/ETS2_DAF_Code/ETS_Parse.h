
bool TrailerConnected = false;
bool newData = false;
String IncomingData = "";
#include <ArduinoJson.h>

StaticJsonDocument<600> ETS2;

void ParseIncoming()
{
  while (Serial.available() > 0)
  {
    //IncomingData = Serial.readStringUntil('\n');
    char data = Serial.read();

    if (data == '\n')
    {
      newData = true;
      //Serial.println("Ok");
      ETS2.clear();
    }
    else
      IncomingData += data;
  }

  if (newData)
  {
    DeserializationError error = deserializeJson(ETS2, IncomingData);

    TimeHours = ETS2["Game"][1];
    TimeMinutes = ETS2["Game"][2];

    unsigned long DistanceJSON = ETS2["Params"][14];
    Distance = DistanceJSON / 10.0;

    if (!ETS2["Game"][0])
    {
      DistanceDaily = Distance - DistanceEEPROM;
    }

    Air2 = map(ETS2["Params"][5], 0, 1400, 0, 100);
    Air1 = ETS2["Params"][4];

    float SpeedJSON = ETS2["Params"][0];
    Speed = SpeedJSON / 10;
    EngineSpeed = ETS2["Params"][1];
    float FuelJSON = ETS2["Params"][2];
    Fuel = FuelJSON / 10;
    WaterTemperature = ETS2["Params"][3];

    //Serial.println(Speed);

    TrailerConnected = ETS2["Params"][13];

    IconLeftTruck = ETS2["Icon"][2];
    IconLeftTrailer = TrailerConnected * IconLeftTruck;

    IconRightTruck = ETS2["Icon"][3];
    IconRightTrailer = TrailerConnected * IconRightTruck;

    if (IconRightTruck || IconLeftTruck)
      SoundBlinker = true;
    else
      SoundBlinker = false;

    if (ETS2["Icon"][1] || ETS2["Warn"][1] || ETS2["Warn"][6])
    {
      IconParkingBrake = true;
      IconStop = true;
    }
    else
    {
      IconParkingBrake = false;
      IconStop = false;
    }

    if (ETS2["Warn"][1] && EngineOn) // Если предупреждение об низком давлении воздуха
    {
      if (!ETS2["Warn"][6]) // Если давление не критически низкое
      {
        DisplayWarningsCount = 1;
        if (!Warning)
        {
          Warning = true;
          Alarm = false;
          SoundAlarmOnce = false;
          WarnTimer = millis();
        }
        SoundAlarmCont = false;
      }
      else // Если давление критически низкое
      {
        /*if (!Alarm)
        {
          Alarm = true;
          Warning = false;
          SoundWarnOnce = false;
          WarnTimer = millis();
        }*/
        SoundAlarmCont = true;
      }
    }
    else // Если давление в норме
    {
      Warning = false;
      Alarm = false;
      SoundWarnOnce = false;
      SoundWarnCont = false; // Отключаем сигнализации
      SoundAlarmCont = false;
      SoundAlarmOnce = false;
    }

    LightParking = ETS2["Icon"][4];
    IconFogFront = ETS2["Icon"][5];
    if (IconFogFront)
      IconHighBeam = ETS2["Icon"][6];

    IndicatorLowFuel = ETS2["Warn"][2];

    if (ETS2["Icon"][0] > 0)
      IconRetarder = true;
    else
      IconRetarder = false;

    if (IconFogFront || LightParking)
      Dashboard_Backlight = ETS2["Icon"][8];
    else
      Dashboard_Backlight = 0;

    MiniDisplays_Backlight = ETS2["Icon"][8];
    Icons_Backlight = MiniDisplays_Backlight;

    if (!Ignition && ETS2["Params"][11])
    {
      Ignition = true;
      EEPROM.get(5, DistanceEEPROM);
    }
    if (Ignition && !ETS2["Params"][11])
      Ignition = false;

    EngineOn = ETS2["Params"][12]; // engine on

    newData = false;
    IncomingData = "";
  }
}

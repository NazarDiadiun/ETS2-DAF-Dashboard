#include <ArduinoJson.h> // Including Arduino JSON Library

bool newData = false; // New Data to parse flag
String IncomingData = ""; // String to temporary store incoming data
bool PageSelectorActive = false;

StaticJsonDocument<550> ETS2; // Creating Static JSON Document

void ParseIncoming()
{
  while (Serial.available() > 0) // if there are any data in serial port
  {
    // in while cycle
    char data = Serial.read(); // Starting to read it byte by byte

    if (data == '\n') // if current character is EOL
    {
      newData = true; // Set flag, that we have new data to parse
      ETS2.clear(); // And clearing JSON document, getting it ready to new data
    }
    else // if current char is not EOL
      IncomingData += data; // Just add it to string
  }

  if (newData) // if new data flag is true
  {
    DeserializationError error = deserializeJson(ETS2, IncomingData); // Deserializing incoming data string into JSON Document

    // Game Data Parsing
    GamePaused = ETS2["Game"][0];
    TimeHours = ETS2["Game"][1];
    TimeMinutes = ETS2["Game"][2];
    NextRest = ETS2["game"][3];

    // Truck Parameters Parsing

    // Truck Speed (Need to divide by 10 to get correct float value)
    float SpeedJSON = ETS2["Params"][0];
    Speed = SpeedJSON / 10;

    // Truck Engine Speed
    EngineSpeed = ETS2["Params"][1];

    // Fuel Section
    FuelCapacity = ETS2["Params"][2];
    FuelAmount = ETS2["Params"][3];
    Range = ETS2["Params"][4];
    AvFuelCons = ETS2["Params"][5];
    Fuel = (float(FuelAmount) / float(FuelCapacity)) * 100.0;

    // Adblue Section
    AdBlueCapacity = ETS2["Params"][6];
    AdBlueAmount = ETS2["Params"][7];
    AdBlue = (float(AdBlueAmount) / float(AdBlueCapacity)) * 100;
    Air1 = AdBlue;

    // oil Section
    OilPressure = ETS2["Params"][8];
    OilTemperature = ETS2["Params"][9];

    // Truck Water Temperature
    WaterTemperature = ETS2["Params"][10];

    // Truck Air Pressure (Need to convert from psi to 0-100% gauge)
    Air2 = map(ETS2["Params"][11], 0, 140, 0, 100);

    // Battery Voltage
    float BatteryVoltageJSON = ETS2["Params"][12];
    BatteryVoltage = BatteryVoltageJSON / 10;

    // Cruise Control Speed (0 if not applied)
    CruiseControl = ETS2["Params"][13];

    // Current Gear
    Gear = ETS2["Params"][14];

    EffectiveThrottle = ETS2["Params"][15];

    BrakesTemperature = ETS2["Params"][16];

    // Truck Ingition State
    if (!Ignition && ETS2["Params"][17]) // if variable is not true & current ignition state is ON
    {
      DisplayCnt = 0;
      DisplayArraySize = 8;
      DisplayPage = 0;
      DisplayStartUp = true;
      DisplayStartupIconCnt = 0;

      Ignition = true; // Change variable to true
      EEPROM.get(5, DistanceEEPROM); // Getting daily distance from EEPROM Memory
    }
    if (Ignition && !ETS2["Params"][17]) // if variable is true & current ignition state if OFF
      Ignition = false; // Change variable to false

    // Truck Engine State
    EngineOn = ETS2["Params"][18];

    // Trailer Connected Flag
    TrailerConnected = ETS2["Params"][19];

    // Truck Odometer (Need to divide by 10 to get correct float value)
    unsigned long DistanceJSON = ETS2["Params"][20];
    Distance = DistanceJSON / 10.0;

    if (!GamePaused) // if currently riding a truck (game not paused)
      DistanceDaily = Distance - DistanceEEPROM; // Calculating current daily mileage as current odometer value - last saved odometer (after daily reset)

    SpeedLimit = ETS2["Params"][21];


    bool PageSelector = ETS2["Cmd"][0];
    if (PageSelector && !PageSelectorActive)
    {
      if (EngineOn)
      {
        DispPage++;
        if (DispPage >= 6)
          DispPage = 0;
        Serial.print("nxtpgOk\n");
      }
    }
    if (!PageSelector && PageSelectorActive)
      PageSelectorActive = false;

    // Truck Icons Parsing

    // Truck Left Blinker
    IconLeftTruck = ETS2["Icon"][1];

    // Trailer Left Blinker
    IconLeftTrailer = TrailerConnected * IconLeftTruck; // if trailer connected and left blinker is on right now

    // Truck Right Blinker
    IconRightTruck = ETS2["Icon"][2];

    // Trailer Right Blinker
    IconRightTrailer = TrailerConnected * IconRightTruck; // if trailer connected and right blinker is on right now

    // Blinker Sound on Dashboard
    if (IconRightTruck || IconLeftTruck) // if any blinker on right now
      SoundBlinker = true; // Make "Blinker ON sound"
    else // if all blinkers is OFF
      SoundBlinker = false; // Disable blinkers sounds

    // if parking brake, low pressure warn or alarm
    if (ETS2["Icon"][0] || ETS2["Warn"][2] || ETS2["Warn"][3])
    {
      IconParkingBrake = true; // Parking brake icon ON
      IconStop = true; // as well as STOP icon
    }
    else // else if all is ok
    {
      IconParkingBrake = false; // disabling parking brake icon
      IconStop = false; // and STOP sign
    }

    if (ETS2["Warn"][2] && Ignition) // if low pressure warning and engine is running
    {
      ToggleIcon(LOW_AIR, ALARM, 1);
      if (EngineOn)
        SoundAlarmCont = true;
    }
    else // if air pressure is normal
    {
      ToggleIcon(LOW_AIR, ALARM, 0);
      SoundWarnOnce = false; // disabling all kinds of sounds
      SoundAlarmCont = false; // alarm continous...
      SoundAlarmOnce = false; // and alarm once as well
    }

    // Truck Parking Light
    LightParking = ETS2["Icon"][3];

    // Truck Low Beam Light
    IconFogFront = ETS2["Icon"][4]; // (but there are no Low Beam, so i replaced it with fog light - looks pretty similar. Almost...)

    // Truck High Beam Light
    if (IconFogFront) // if low beam light is ON
      IconHighBeam = ETS2["Icon"][5]; // we can turn on high beam
    else // if not
      IconHighBeam = false; // Make hgih beam light disabled

    // Truck Low Fuel Indicator
    IndicatorLowFuel = ETS2["Warn"][5];

    LowBattery = ETS2["Warn"][1];
    ToggleIcon(BATTERY, ALARM, LowBattery);

    LowOilPressure = ETS2["Warn"][0];
    ToggleIcon(OIL_PRESSURE, ALARM, LowOilPressure);


    // Truck Retarder Icon
    IconRetarder = ETS2["Icon"][7];

    // Dashboard Backlight
    // if parking lights or low beam lights turned on
    if (IconFogFront || LightParking)
      Dashboard_Backlight = ETS2["Icon"][11]; // Make dashboard backlight shine :)
    else // if not
      Dashboard_Backlight = 0; // disabling dashboard backlight

    // Displays Backlight (time, odometer etc)
    MiniDisplays_Backlight = ETS2["Icon"][11];
    Icons_Backlight = MiniDisplays_Backlight; // We can also adjust icons backlight brightness

    IconAxisBlock = ETS2["Icon"][10];

    EngineWear = ETS2["Wear"][0];
    if (EngineWear >= 3 && EngineWear < 5)
      ToggleIcon(ENGINE, WARNING, 1);
    else if (EngineWear >= 5)
    {
      ToggleIcon(ENGINE, WARNING, 0);
      ToggleIcon(ENGINE, ALARM, 1);
    }
    else if (EngineWear < 3)
    {
      ToggleIcon(ENGINE, WARNING, 0);
      ToggleIcon(ENGINE, ALARM, 0);
    }

    TransmissionWear = ETS2["Wear"][1];
    if (TransmissionWear >= 3 && TransmissionWear < 5)
      ToggleIcon(TRANSMISSION, WARNING, 1);
    else if (TransmissionWear >= 5)
    {
      ToggleIcon(TRANSMISSION, WARNING, 0);
      ToggleIcon(TRANSMISSION, ALARM, 1);
    }
    else if (TransmissionWear < 3)
    {
      ToggleIcon(TRANSMISSION, WARNING, 0);
      ToggleIcon(TRANSMISSION, ALARM, 0);
    }

    CabinWear = ETS2["Wear"][2];
    ChassisWear = ETS2["Wear"][3];
    WheelsWear = ETS2["Wear"][4];


    newData = false; // Data Parsed fully
    IncomingData = ""; // Clearing incoming data string
  }
}

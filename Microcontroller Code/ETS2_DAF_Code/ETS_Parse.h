#include <ArduinoJson.h> // Including Arduino JSON Library

bool newData = false; // New Data to parse flag
String IncomingData = ""; // String to temporary store incoming data

StaticJsonDocument<600> ETS2; // Creating Static JSON Document 

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

    // Truck Parameters Parsing

    // Truck Speed (Need to divide by 10 to get correct float value)
    float SpeedJSON = ETS2["Params"][0];
    Speed = SpeedJSON / 10;

    // Truck Engine Speed
    EngineSpeed = ETS2["Params"][1];

    // Truck Fuel Level (Need to divide by 10 to get correct float value)
    float FuelJSON = ETS2["Params"][2];
    Fuel = FuelJSON / 10;

    // Truck Water Temperature
    WaterTemperature = ETS2["Params"][3];

    // Truck Adblue Level (Instead of 1 air pressure gauge. Need to divide by 10 to get correct float value)
    float AdblueJSON = ETS2["Params"][4];
    AdBlue = AdblueJSON / 10.0;
    Air1 = AdBlue;

    // Truck Air Pressure (Need to convert from psi to 0-100% gauge)
    Air2 = map(ETS2["Params"][5], 0, 140, 0, 100);

    // Truck Ingition State
    if (!Ignition && ETS2["Params"][11]) // if variable is not true & current ignition state is ON
    {
      Ignition = true; // Change variable to true
      EEPROM.get(5, DistanceEEPROM); // Getting daily distance from EEPROM Memory
    }
    if (Ignition && !ETS2["Params"][11]) // if variable is true & current ignition state if OFF
      Ignition = false; // Change variable to false

    // Truck Engine State
    EngineOn = ETS2["Params"][12];

    // Trailer Connected Flag
    TrailerConnected = ETS2["Params"][13];

    // Truck Odometer (Need to divide by 10 to get correct float value)
    unsigned long DistanceJSON = ETS2["Params"][14];
    Distance = DistanceJSON / 10.0;

    if (!GamePaused) // if currently riding a truck (game not paused)
      DistanceDaily = Distance - DistanceEEPROM; // Calculating current daily mileage as current odometer value - last saved odometer (after daily reset)


    // Truck Icons Parsing

    // Truck Left Blinker
    IconLeftTruck = ETS2["Icon"][2];

    // Trailer Left Blinker
    IconLeftTrailer = TrailerConnected * IconLeftTruck; // if trailer connected and left blinker is on right now

    // Truck Right Blinker
    IconRightTruck = ETS2["Icon"][3];

    // Trailer Right Blinker
    IconRightTrailer = TrailerConnected * IconRightTruck; // if trailer connected and right blinker is on right now

    // Blinker Sound on Dashboard
    if (IconRightTruck || IconLeftTruck) // if any blinker on right now
      SoundBlinker = true; // Make "Blinker ON sound"
    else // if all blinkers is OFF
      SoundBlinker = false; // Disable blinkers sounds

    // if parking brake, low pressure warn or alarm
    if (ETS2["Icon"][1] || ETS2["Warn"][1] || ETS2["Warn"][6])
    {
      IconParkingBrake = true; // Parking brake icon ON
      IconStop = true; // as well as STOP icon
    }
    else // else if all is ok
    {
      IconParkingBrake = false; // disabling parking brake icon
      IconStop = false; // and STOP sign
    }

    if (ETS2["Warn"][1] && EngineOn) // if low pressure warning and engine is running
    {
      if (!ETS2["Warn"][6]) // if it is just alarm (not so critical)
      {
        DisplayWarningsCount = 1;

        if (!Warning) // if warning flag is false
        {
          Warning = true; // make it true (to repeat just once)
          Alarm = false; // alarm state is off
          SoundAlarmOnce = false; // disable alarm (in case if it enabled)
          WarnTimer = millis(); // and reset timer (to make warn sound periodically)
        }
        SoundAlarmCont = false; // disabling alarm sound (to not interferece with warning in case)
      }
      else // if air pressure too low (critical)
      {
        /*if (!Alarm)
          {
          Alarm = true;
          Warning = false;
          SoundWarnOnce = false;
          WarnTimer = millis();
          }*/

        SoundAlarmCont = true; // Enabling continous alarm sound
      }
    }
    else // if air pressure is normal
    {
      Warning = false; // Warning flag is off
      Alarm = false; // as well as alarm flag
      SoundWarnOnce = false; // disabling all kinds of sounds
      SoundWarnCont = false; // warning continous...
      SoundAlarmCont = false; // alarm continous...
      SoundAlarmOnce = false; // and alarm once as well
    }

    // Truck Parking Light
    LightParking = ETS2["Icon"][4];

    // Truck Low Beam Light 
    IconFogFront = ETS2["Icon"][5]; // (but there are no Low Beam, so i replaced it with fog light - looks pretty similar. Almost...)

    // Truck High Beam Light
    if (IconFogFront) // if low beam light is ON
      IconHighBeam = ETS2["Icon"][6]; // we can turn on high beam
    else // if not
      IconHighBeam = false; // Make hgih beam light disabled

    // Truck Low Fuel Indicator
    IndicatorLowFuel = ETS2["Warn"][2]; 

    // Truck Retarder Icon
    IconRetarder = ETS2["Icon"][0];

    // Dashboard Backlight
    // if parking lights or low beam lights turned on 
    if (IconFogFront || LightParking)
      Dashboard_Backlight = ETS2["Icon"][8]; // Make dashboard backlight shine :)
    else // if not
      Dashboard_Backlight = 0; // disabling dashboard backlight

    // Displays Backlight (time, odometer etc)
    MiniDisplays_Backlight = ETS2["Icon"][8];
    Icons_Backlight = MiniDisplays_Backlight; // We can also adjust icons backlight brightness

    newData = false; // Data Parsed fully
    IncomingData = ""; // Clearing incoming data string
  }
}

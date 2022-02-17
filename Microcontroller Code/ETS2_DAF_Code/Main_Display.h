bool DisplayUpWarn = false;
bool DisplayUpAlarm = false;
bool DisplayDownWarn = false;

bool ActiveWarn = false;
bool ActiveAlarm = false;

#define WARNING                     0
#define ALARM                       1

#define ICON_OFF                    0
#define OIL_PRESSURE                1
#define CABIN_LOCK                  2
#define LOW_AIR                     3
#define SIGNALIZATION               4
#define COOLANT_LEVEL               5
#define COOLANT_TEMPERATURE         6
#define BATTERY                     7
#define STEERING_1                  8
#define ENGINE                      9
#define TRANSMISSION                10
#define ABS_TRUCK                   11
#define ABS_TRAILER                 12
#define EBS_TRUCK                   13
#define EBS_TRAILER                 14
#define RETARDER                    15
#define STEERING_2                  16
#define AIRBAG                      17
#define OIL_LEVEL                   18
#define SUSPENSION_LEVEL            19
#define IMMO                        20
#define WATER_IN_FUEL               21
#define BRAKE_PADS                  22
#define EMISSION_SMTH               23
#define OILING_SYSTEM               24
#define PTO                         25
#define TURBO                       26
#define LIGHTS_OFF                  27
#define DAF_LOGO                    28
#define ASR                         29
#define LIKE_ICON                   30
#define EMISSION                    31
#define LIFTABLE_AXIS               32
#define CLUTCH                      33
#define GEAR_TOO_HIGH               34
#define AXIS_WEIGHT                 35
#define TRAILER_COOLING             36
#define TRAILER_HEATING             37
#define TRAILER_SUSPENSION          38
#define TRAILER_DOOR                39
#define MANIPULATOR                 40
#define TRAILER_LOWERING_DECK       41
#define LIGHT_BULB                  42
#define BODY_OPENED                 43
#define BODY_TEMPERATURE            44
#define DISTANCE                    45
#define VSC                         46
#define WINDSHIELD_FLUID            47
#define SPEEDP                      48

byte DisplayIcons[9] = {0}; // Array for icons. first 3 - Alarms, next 6 - Warnings

// Function to search through icons places, and if find it out - returns it location on display
byte SearchIcon(byte IconID, byte IconType)
{
  for (byte cnt = 0; cnt < 9; cnt++)
  {
    if (IconType == 0)
    {
      if (DisplayIcons[cnt] == IconID && cnt > 2)
        return cnt;
    }
    else if (IconType == 1)
    {
      if (DisplayIcons[cnt] == IconID && cnt < 3)
        return cnt;
    }
  }

  return 255;
}

void cnt_Msg_count()
{
  DisplayAlarmCount = 0;
  DisplayWarningsCount = 0;

  for (byte cnt = 0; cnt < 9; cnt++)
  {
    if (DisplayIcons[cnt] != 0)
    {
      if (cnt < 3)
        DisplayAlarmCount++;
      else
        DisplayWarningsCount++;
    }
  }
}

void Sort_Icons(byte Array[], byte IconType)
{
  byte Start = 0, End = 11;

  if (IconType == 0)
    Start = 3;
  else if (IconType == 1)
    End = 2;

  for (int i = Start; i < End - 1; i++)
  {
    for (int o = Start; o < (End - i); o++)
    {
      if (Array[o] > Array[o + 1] && Array[o + 1] != 0 || Array[o] == 0 )
      {
        int t = Array[o];
        Array[o] = Array[o + 1];
        Array[o + 1] = t;
      }
    }
  }
}

// Function to turn icon ON or OFF
bool ToggleIcon(byte IconID, byte IconType, bool State)
{
  if (State)
  {
    byte IconDuplicate = SearchIcon(IconID, IconType);
    byte FreeIconLoc = SearchIcon(ICON_OFF, IconType); // Trying to find free place on display

    if (FreeIconLoc != 255 && IconDuplicate == 255) // if we found free place
    {
      DisplayIcons[FreeIconLoc] = IconID;
      Sort_Icons(DisplayIcons, IconType);
      return 1;
    }
    else
      return 0;
  }
  else
  {
    byte FindIconLoc = SearchIcon(IconID, IconType); // Trying to find free place on display

    if (FindIconLoc != 255) // if we found free place
    {
      DisplayIcons[FindIconLoc] = 0;
      Sort_Icons(DisplayIcons, IconType);
      return 1;
    }
    else
      return 0;
  }
}

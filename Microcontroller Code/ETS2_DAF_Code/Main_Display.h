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

unsigned long DispTimer = 0;
struct can_frame DisplayOther;
byte DisplayCnt = 0;

unsigned long DisplayStratupTimer = 0;
unsigned int DisplayStratupDelay = 200;
bool DisplayStartUp = false;
byte DisplayStartupIconCnt = 0;

byte DisplayPage = 0; // Pages: 1 - All warnings and alarms icons, 2 - Current warning or alarm, 3 - All warnings and alarms with description
byte DisplayArraySize = 8;

/*
  // menu page
  byte DisplayArray[8][8] = {
  {0x10, 0x34, 0x03, 0xA0, 0x01, 0x00, 0x0D, 0x1D},
  {0x21, 0x56, 0x45, 0x48, 0x49, 0x43, 0x4C, 0x45},
  {0x22, 0x20, 0x49, 0x4E, 0x46, 0x4F, 0x09, 0x1D},
  {0x23, 0x57, 0x41, 0x52, 0x4E, 0x49, 0x4E, 0x47},
  {0x24, 0x53, 0x0A, 0x1D, 0x54, 0x52, 0x49, 0x50},
  {0x25, 0x20, 0x49, 0x4E, 0x46, 0x4F, 0x09, 0x1D},
  {0x26, 0x53, 0x45, 0x54, 0x54, 0x49, 0x4E, 0x47},
  {0x27, 0x53, 0x00, 0x01, 0x02, 0x1D, 0x72, 0x72}
  };*/
// Blank page
const byte DisplayBlank[8][8] PROGMEM = {
  {0x10, 0x34, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00},
  {0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// Fuel Usage page - Average consumption & current (not available)
byte DisplayFuelUsage[8][8] = {
  {0x10, 0x34, 0x03, 0xA0, 0x01, 0x00, 0x0C, 0x1A},
  {0x21, 0x1F, 0x46, 0x75, 0x65, 0x6C, 0x1D, 0x75},
  {0x22, 0x73, 0x61, 0x67, 0x65, 0x00, 0x12, 0x41},
  {0x23, 0x76, 0x65, 0x72, 0x61, 0x67, 0x65, 0x1E},
  {0x24, 0x32, 0x39, 0x2E, 0x38, 0x1F, 0x6C, 0x2F},
  {0x25, 0x31, 0x30, 0x30, 0x11, 0x43, 0x75, 0x72},
  {0x26, 0x72, 0x65, 0x6E, 0x74, 0x1E, 0x2D, 0x2D},
  {0x27, 0x2D, 0x1F, 0x6C, 0x2F, 0x31, 0x30, 0x30}
};

// Fuel Info page - amount of fuel in liters & range
byte DisplayFuelInfo[8][8] = {
  {0x10, 0x34, 0x03, 0xA0, 0x01, 0x00, 0x0B, 0x1A},
  {0x21, 0x1F, 0x46, 0x75, 0x65, 0x6C, 0x20, 0x69},
  {0x22, 0x6E, 0x66, 0x6F, 0x00, 0x10, 0x44, 0x69},
  {0x23, 0x73, 0x74, 0x61, 0x6E, 0x63, 0x65, 0x1E},
  {0x24, 0x2D, 0x2D, 0x2D, 0x2D, 0x20, 0x6B, 0x6D},
  {0x25, 0x0E, 0x46, 0x75, 0x65, 0x6C, 0x1E, 0x1E},
  {0x26, 0x1E, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x20},
  {0x27, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

byte DisplaySpeedInfo[8][8] = {
  {0x10, 0x34, 0x03, 0xA0, 0x01, 0x00, 0x0C, 0x13},
  {0x21, 0x1F, 0x53, 0x70, 0x65, 0x65, 0x64, 0x1F},
  {0x22, 0x69, 0x6E, 0x66, 0x6F, 0x00, 0x10, 0x43},
  {0x23, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74, 0x1E},
  {0x24, 0x2D, 0x2D, 0x2D, 0x1F, 0x6B, 0x6D, 0x2F},
  {0x25, 0x68, 0x0E, 0x43, 0x43, 0x1E, 0x1E, 0x1E},
  {0x26, 0x1F, 0x2D, 0x2D, 0x2D, 0x1F, 0x6B, 0x6D},
  {0x27, 0x2F, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00}
};

byte DisplayTemperature[8][8] = {
  {0x10, 0x34, 0x03, 0xA0, 0x01, 0x00, 0x0C, 0x13},
  {0x21, 0x54, 0x65, 0x6D, 0x70, 0x65, 0x72, 0x61},
  {0x22, 0x74, 0x75, 0x72, 0x65, 0x00, 0x0F, 0x57},
  {0x23, 0x61, 0x74, 0x65, 0x72, 0x1D, 0x1E, 0x1E},
  {0x24, 0x1E, 0x2D, 0x2D, 0x2D, 0x1F, 0xB0, 0x43},
  {0x25, 0x0E, 0x4F, 0x69, 0x6C, 0x1E, 0x1E, 0x1E},
  {0x26, 0x1E, 0x1E, 0x2D, 0x2D, 0x2D, 0x1F, 0xB0},
  {0x27, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

byte DisplayTurbo[8][8] = {
  {0x10, 0x34, 0x02, 0xA0, 0x01, 0x00, 0x01, 0x1A},
  {0x21, 0x0E, 0x54, 0x75, 0x72, 0x62, 0x6F, 0x20},
  {0x22, 0x70, 0x72, 0x65, 0x73, 0x73, 0x75, 0x72},
  {0x23, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// one message - 4 seconds duration
byte DisplayStartupStage2[7][8] = {
  {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte LowAirPressure_Message[7][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0xA0, 0x01, 0x04, 0x41, 0x03}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x10, 0x1F, 0x20, 0x1D, 0x1D, 0x41, 0x69},
  {0x22, 0x72, 0x20, 0x70, 0x72, 0x65, 0x73, 0x73},
  {0x23, 0x75, 0x72, 0x65, 0x09, 0x1F, 0x1F, 0x1F},
  {0x24, 0x1F, 0x1F, 0x1F, 0x6C, 0x6F, 0x77, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x72, 0x61, 0x6B},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte EngineWarning_Message[8][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0xA0, 0x01, 0x04, 0x41, 0x09}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x09, 0x1E, 0x1E, 0x1D, 0x45, 0x6E, 0x67},
  {0x22, 0x69, 0x6E, 0x65, 0x0A, 0x1E, 0x1F, 0x20},
  {0x23, 0x77, 0x61, 0x72, 0x6E, 0x69, 0x6E, 0x67},
  {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte EngineError_Message[8][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0x90, 0x01, 0x01, 0x41, 0x09}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x09, 0x1E, 0x1E, 0x1D, 0x45, 0x6E, 0x67},
  {0x22, 0x69, 0x6E, 0x65, 0x08, 0x1E, 0x1E, 0x1F},
  {0x23, 0x65, 0x72, 0x72, 0x6F, 0x72, 0x00, 0x00},
  {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte TransmissionWarning_Message[8][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0xA0, 0x01, 0x04, 0x41, 0x0A}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x0D, 0x1F, 0x54, 0x72, 0x61, 0x6E, 0x73},
  {0x22, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E},
  {0x23, 0x0A, 0x1E, 0x1F, 0x20, 0x77, 0x61, 0x72},
  {0x24, 0x6E, 0x69, 0x6E, 0x67, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte TransmissionError_Message[8][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0x90, 0x01, 0x01, 0x41, 0x0A}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x0D, 0x1F, 0x54, 0x72, 0x61, 0x6E, 0x73},
  {0x22, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6F, 0x6E},
  {0x23, 0x08, 0x1E, 0x1E, 0x1F, 0x65, 0x72, 0x72},
  {0x24, 0x6F, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte CoolantLevelLow_Message[7][8] PROGMEM = {
  {0x10, 0x26, 0x02, 0xA0, 0x01, 0x04, 0x41, 0x05}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x10, 0x1F, 0x1D, 0x1D, 0x43, 0x6F, 0x6F},
  {0x22, 0x6C, 0x61, 0x6E, 0x74, 0x20, 0x6C, 0x65},
  {0x23, 0x76, 0x65, 0x6C, 0x09, 0x1F, 0x1F, 0x1F},
  {0x24, 0x1F, 0x1F, 0x1F, 0x6C, 0x6F, 0x77, 0x00},
  {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte EBSTruck_Message[7][8] PROGMEM = {
  {0x10, 0x2A, 0x02, 0xA0, 0x01, 0x04, 0x41, 0x0D}, // 3 byte - Display Mode (0x03 - text only, 0x02 - icon+msg, 0x01 - icons only) | 4 byte - display colors | 6 byte - sound type |  7 byte (0x41) - Enable icon. 0x41 - blink, 01 - constant | 8 byte - icon ID
  {0x21, 0x0D, 0x1F, 0x42, 0x72, 0x61, 0x6B, 0x65},
  {0x22, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D},
  {0x23, 0x10, 0x20, 0x1D, 0x1D, 0x74, 0x72, 0x75},
  {0x24, 0x63, 0x6B, 0x20, 0x77, 0x61, 0x72, 0x6E},
  {0x25, 0x69, 0x6E, 0x67, 0x00, 0x00, 0x00, 0x00},
  {0x26, 0x00, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65}
};

void DisplayIconMessage_FillArray(byte IconID, byte MsgType) // Fill Array with icon and description - Icon ID, Message type (warning or alarm)
{
  if (IconID == 3)
    memcpy(DisplayStartupStage2, LowAirPressure_Message, sizeof(LowAirPressure_Message));
  /*if (IconID == 9 && MsgType == WARNING)
    memcpy(DisplayStartupStage2, EngineWarning_Message, sizeof(EngineWarning_Message));
    else if (IconID == 9 && MsgType == ALARM)
    memcpy(DisplayStartupStage2, EngineError_Message, sizeof(EngineError_Message));*/
  if (IconID == 5)
    memcpy(DisplayStartupStage2, CoolantLevelLow_Message, sizeof(CoolantLevelLow_Message));
  if (IconID == 13)
    memcpy(DisplayStartupStage2, EBSTruck_Message, sizeof(EBSTruck_Message));

  DisplayStartupStage2[0][3] = (1 << 7) | (!MsgType << 5) | (MsgType << 4); // Display Color (Yellow or Red)
  DisplayStartupStage2[0][5] = (!MsgType << 2) | (MsgType << 0); // Sound (Warning or Alarm)
  DisplayStartupStage2[0][7] = IconID;
}

// First Stage of Display Startup (Displaying all available icons)
byte DisplayStartupStage1[3][8] = {
  {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

unsigned long DisplayUpperTimer = 0;
byte DisplayUpperCnt = 0;
byte DisplayUpperDelay = 10;
struct can_frame DisplayUpperFrame;

byte DisplayUpperArray[3][8] = {
  {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

byte DisplayIconsNew[9] = {0}; // New arrived messages. first 3 - Alarms, next 6 - Warnings
bool DisplayNewIconAvailable = false;

byte DisplayIcons[9] = {0}; // Array for icons. first 3 - Alarms, next 6 - Warnings

// Function to search through icons places, and if find it out - returns it location on display
byte SearchIcon(byte Array[], byte IconID, byte IconType)
{
  for (byte cnt = 0; cnt < 9; cnt++)
  {
    if (IconType == 0)
    {
      if (Array[cnt] == IconID && cnt > 2)
        return cnt;
    }
    else if (IconType == 1)
    {
      if (Array[cnt] == IconID && cnt < 3)
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
    byte IconDuplicate = SearchIcon(DisplayIcons, IconID, IconType);
    byte FreeIconLoc = SearchIcon(DisplayIcons, ICON_OFF, IconType); // Trying to find free place on display

    if (FreeIconLoc != 255 && IconDuplicate == 255) // if we found free place
    {
      DisplayIcons[FreeIconLoc] = IconID;
      Sort_Icons(DisplayIcons, IconType);
      DisplayNewIconAvailable = true;
      return 1;
    }
    else
      return 0;
  }
  else
  {
    byte FindIconLoc = SearchIcon(DisplayIcons, IconID, IconType); // Trying to find free place on display

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
byte counter = 0;
void DisplayFuelUsage_FillArray()
{
  DisplayFuelUsage[4][1] = (AvFuelCons / 100) + 48;
  DisplayFuelUsage[4][2] = ((AvFuelCons / 10) % 10) + 48;
  DisplayFuelUsage[4][4] = (AvFuelCons % 10) + 48;
}

void DisplaySpeedInfo_FillArray()
{

  DisplaySpeedInfo[4][3] = (int(Speed) % 10) + 48;
  if (Speed >= 10)
  {
    if (Speed >= 100)
    {
      DisplaySpeedInfo[4][2] = ((int(Speed) / 10) % 10) + 48;
      DisplaySpeedInfo[4][1] = (int(Speed) / 100) + 48;
    }
    else
    {
      DisplaySpeedInfo[4][2] = (int(Speed) / 10) + 48;
      DisplaySpeedInfo[4][1] = 0x1F;
    }
  }
  else
  {
    DisplaySpeedInfo[4][1] = 0x1F;
    DisplaySpeedInfo[4][2] = 0x1F;
  }

  if (CruiseControl != 0)
  {
    DisplaySpeedInfo[6][4] = (int(CruiseControl) % 10) + 48;
    if (CruiseControl >= 10)
    {
      if (CruiseControl >= 100)
      {
        DisplaySpeedInfo[6][3] = ((int(CruiseControl) / 10) % 10) + 48;
        DisplaySpeedInfo[6][2] = (int(CruiseControl) / 100) + 48;
      }
      else
      {
        DisplaySpeedInfo[6][3] = (int(CruiseControl) / 10) + 48;
        DisplaySpeedInfo[6][2] = 0x1F;
      }
    }
    else
    {
      DisplaySpeedInfo[6][2] = 0x1F;
      DisplaySpeedInfo[6][3] = 0x1F;
    }
  }
  else
  {
    DisplaySpeedInfo[6][2] = 0x2D;
    DisplaySpeedInfo[6][3] = 0x2D;
    DisplaySpeedInfo[6][4] = 0x2D;
  }
}

void DisplayTemperature_FillArray()
{
  if (WaterTemperature < 0)
  {
    if (WaterTemperature <= -10)
    {
      DisplayTemperature[4][2] = 0x2D;
      DisplayTemperature[4][3] = (abs(WaterTemperature) / 10) + 48;
      DisplayTemperature[4][4] = (abs(WaterTemperature) % 10) + 48;
    }
    else
    {
      DisplayTemperature[4][2] = 0x1F;
      DisplayTemperature[4][3] = 0x2D;
      DisplayTemperature[4][4] = abs(WaterTemperature) + 48;
    }
  }
  else
  {
    if (WaterTemperature < 10)
    {
      DisplayTemperature[4][2] = 0x1F;
      DisplayTemperature[4][3] = 0x1F;
      DisplayTemperature[4][4] = WaterTemperature + 48;
    }
    else if (WaterTemperature >= 10 && WaterTemperature < 100)
    {
      DisplayTemperature[4][2] = 0x1F;
      DisplayTemperature[4][3] = (WaterTemperature / 10) + 48;
      DisplayTemperature[4][4] = (WaterTemperature % 10) + 48;
    }
    else if (WaterTemperature >= 100)
    {
      DisplayTemperature[4][2] = (WaterTemperature / 100) + 48;
      DisplayTemperature[4][3] = ((WaterTemperature / 10) % 10) + 48;
      DisplayTemperature[4][4] = (WaterTemperature % 10) + 48;
    }
  }
  if (OilTemperature < 0)
  {
    if (OilTemperature <= -10)
    {
      DisplayTemperature[6][3] = 0x2D;
      DisplayTemperature[6][4] = (abs(OilTemperature) / 10) + 48;
      DisplayTemperature[6][5] = (abs(OilTemperature) % 10) + 48;
    }
    else
    {
      DisplayTemperature[6][3] = 0x1F;
      DisplayTemperature[6][4] = 0x2D;
      DisplayTemperature[6][5] = abs(OilTemperature) + 48;
    }
  }
  else
  {
    if (OilTemperature < 10)
    {
      DisplayTemperature[6][3] = 0x1F;
      DisplayTemperature[6][4] = 0x1F;
      DisplayTemperature[6][5] = OilTemperature + 48;
    }
    else if (OilTemperature >= 10 && OilTemperature < 100)
    {
      DisplayTemperature[6][3] = 0x1F;
      DisplayTemperature[6][4] = (OilTemperature / 10) + 48;
      DisplayTemperature[6][5] = (OilTemperature % 10) + 48;
    }
    else if (WaterTemperature >= 100)
    {
      DisplayTemperature[6][3] = (OilTemperature / 100) + 48;
      DisplayTemperature[6][4] = ((OilTemperature / 10) % 10) + 48;
      DisplayTemperature[6][5] = (OilTemperature % 10) + 48;
    }
  }
}

void DisplayFuelInfo_FillArray()
{

  DisplayFuelInfo[4][1] = (Range / 1000) + 48;
  DisplayFuelInfo[4][2] = ((Range / 100) % 10) + 48;
  DisplayFuelInfo[4][3] = ((Range / 10) % 10) + 48;
  DisplayFuelInfo[4][4] = (Range % 10) + 48;

  if (Range < 1000)
  {
    if (Range < 100)
    {
      if (Range < 10)
      {
        DisplayFuelInfo[4][1] = 0x1F;
        DisplayFuelInfo[4][2] = 0x1F;
        DisplayFuelInfo[4][3] = 0x1F;
      }
      else
      {
        DisplayFuelInfo[4][1] = 0x1F;
        DisplayFuelInfo[4][2] = 0x1F;
      }
    }
    else
      DisplayFuelInfo[4][1] = 0x1F;
  }


  DisplayFuelInfo[6][3] = (FuelAmount / 1000) + 48;
  DisplayFuelInfo[6][4] = ((FuelAmount / 100) % 10) + 48;
  DisplayFuelInfo[6][5] = ((FuelAmount / 10) % 10) + 48;
  DisplayFuelInfo[6][6] = (FuelAmount % 10) + 48;

  if (FuelAmount < 1000)
  {
    if (FuelAmount < 100)
    {
      if (FuelAmount < 10)
      {
        DisplayFuelInfo[6][3] = 0x1F;
        DisplayFuelInfo[6][4] = 0x1F;
        DisplayFuelInfo[6][5] = 0x1F;
      }
      else
      {
        DisplayFuelInfo[6][3] = 0x1F;
        DisplayFuelInfo[6][4] = 0x1F;
      }
    }
    else
      DisplayFuelInfo[6][3] = 0x1F;
  }
}

bool FillDisplayUpperArray()
{
  DisplayUpperArray[0][1] = 0x0F;
  DisplayUpperArray[0][2] = 0x0A;

  cnt_Msg_count();

  // UpBar zone. 0000 xyza: x - yellow right part, y - red right part (no red color), z - yellow left corner, a - red left corner
  bool DisplayUpBarAlarm = false;

  if (DisplayAlarmCount != 0)
    DisplayUpBarAlarm = true;

  DisplayUpperArray[0][3] = (1 << 3) | (!DisplayUpBarAlarm << 1) | (DisplayUpBarAlarm << 0);
  DisplayUpperArray[0][4] = 0x01;
  DisplayUpperArray[0][5] = 0x00;

  if (DisplayAlarmCount != 0)
  {
    DisplayUpperArray[0][6] = 0x81;
    DisplayUpperArray[0][7] = 48 + DisplayAlarmCount;
  }
  else
  {
    DisplayUpperArray[0][6] = 0x01;
    DisplayUpperArray[0][7] = 32;
  }

  if (DisplayWarningsCount != 0)
  {
    DisplayUpperArray[1][1] = 0x81;
    DisplayUpperArray[1][2] = 48 + DisplayWarningsCount;
  }
  else
  {
    DisplayUpperArray[1][1] = 0x01;
    DisplayUpperArray[1][2] = 32;
  }
  DisplayUpperArray[1][3] = 0x00;
  DisplayUpperArray[1][4] = 0x01;
  DisplayUpperArray[1][6] = 0x01;

  if (Gear == 0) // if neutral gear
  {
    // Displaying 'N' symbol. First number disabled
    DisplayUpperArray[1][5] = 20; // first number of gear
    DisplayUpperArray[1][7] = 14; // Second number
  }
  else if (Gear < 0) // if reverse
  {
    // displaying 'R' symbol and gear number. example R1, R2 etc
    DisplayUpperArray[1][5] = 13; // first number of gear
    DisplayUpperArray[1][7] = abs(Gear); // Second number
  }
  else if (Gear > 0)
  {
    // Displaying current gear
    DisplayUpperArray[1][5] = Gear / 10; // first number of gear

    byte secondNumber = Gear % 10; // second number is mod from gear
    if (secondNumber == 0) // if second number is zero
      DisplayUpperArray[1][7] = 10; // putting here 10 (because 0 = disabled number)
    else // else if second number is 1-9
      DisplayUpperArray[1][7] = secondNumber; // just displaying it here
  }

  //(Transmission, turtle, auto, Manual, parking, gear stick)
  DisplayUpperArray[2][1] = 0x01;

  if (Gear > 0)
    DisplayUpperArray[2][2] = 0x03;
  else
    DisplayUpperArray[2][2] = 0x00;

  // Some info thingy (example: current speed 75km/h)
  DisplayUpperArray[2][3] = 0x00; //0x31 -- CC

  DisplayUpperArray[2][4] = 0x03; // displaying CC icon on upper bar
  DisplayUpperArray[2][5] = 0x20;
  DisplayUpperArray[2][6] = 0x1F;
  DisplayUpperArray[2][7] = 0x1F;
  
  if (CruiseControl != 0) // If cruise control active now
  {

    if (CruiseControl < 100)
    {
      DisplayUpperArray[2][6] = (CruiseControl / 10) + 48;
      DisplayUpperArray[2][7] = (CruiseControl % 10) + 48;
    }
    else
    {
      DisplayUpperArray[2][5] = (CruiseControl / 100) + 48;
      DisplayUpperArray[2][6] = ((CruiseControl / 10) % 10) + 48;
      DisplayUpperArray[2][7] = (CruiseControl % 10) + 48;
    }
  }


  return true;
}

bool DisplayStartupStage1_FillArray()
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

    if (ActiveWarn) // if alarm is on and some warning message are displaying
      WarnStartIndex = 6; // display first 3 of them (3 left is hidden)
  }
  else
    DisplayUpAlarm = false;

  cnt_Msg_count();

  DisplayStartupStage1[0][1] = 0x0E;
  DisplayStartupStage1[0][2] = 0x01;
  DisplayStartupStage1[0][3] = (1 << 7) | (!DisplayUpAlarm << 5) | (DisplayUpAlarm << 4);
  DisplayStartupStage1[0][4] = 0x01; // enable icons ?
  DisplayStartupStage1[0][5] = 0x00; //(SoundAlarmOnce << 0) | (SoundAlarmCont << 1) | (SoundWarnOnce << 2);

  // Alarm 1 icon
  DisplayStartupStage1[0][6] = 0x01;

  if (ActiveAlarm)
    DisplayStartupStage1[0][7] = DisplayIcons[0];    // ALARM 1
  else
    DisplayStartupStage1[0][7] = DisplayIcons[3];    // ALARM 1

  // Icons Alarm 2 - warn 1
  DisplayStartupStage1[1][1] = 0x01;
  DisplayStartupStage1[1][3] = 0x01;
  DisplayStartupStage1[1][5] = 0x01;
  DisplayStartupStage1[1][7] = 0x00;

  if (ActiveAlarm)
  {
    DisplayStartupStage1[1][2] = DisplayIcons[1];  // ALARM 2
    DisplayStartupStage1[1][4] = DisplayIcons[2];  // ALARM 3
    DisplayStartupStage1[1][6] = DisplayIcons[3]; // WARN 1
  }
  else
  {
    DisplayStartupStage1[1][2] = DisplayIcons[4];  // ALARM 2
    DisplayStartupStage1[1][4] = DisplayIcons[5];  // ALARM 3
    DisplayStartupStage1[1][6] = DisplayIcons[6]; // WARN 1
  }

  DisplayStartupStage1[2][2] = 0x01;

  if (ActiveAlarm)
  {
    DisplayStartupStage1[2][1] = DisplayIcons[4]; // WARN 2
    DisplayStartupStage1[2][3] = DisplayIcons[5]; // WARN 3
  }
  else
  {
    DisplayStartupStage1[2][1] = DisplayIcons[7]; // WARN 2
    DisplayStartupStage1[2][3] = DisplayIcons[8]; // WARN 3
  }

  DisplayStartupStage1[2][4] = 0x72;
  DisplayStartupStage1[2][5] = 0x65;
  DisplayStartupStage1[2][6] = 0x73;
  DisplayStartupStage1[2][7] = 0x73;

  return true;
}

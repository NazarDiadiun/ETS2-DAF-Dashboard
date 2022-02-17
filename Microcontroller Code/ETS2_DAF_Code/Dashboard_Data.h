bool GamePaused = false;
bool TrailerConnected = false;

bool Ignition = false; // Включение панели приборов (зажигание)
bool EngineOn = false;

float Speed = 0.0; // Скорость автомобиля
float EngineSpeed = 0; // Обороты двигателя
int WaterTemperature = 0; // Температура охл. жидкости
float Fuel = 0.0; // Уровень топлива в процентах 0-100%
float Air1 = 0.0; // Давление в первом контуре
float Air2 = 0.0; // Давление во втором контуре
float AdBlue = 0.0;

float Distance = 0.0; // Общий пробег
float DistanceDaily = 0.0; // Суточный пробег
float DistanceEEPROM = 0.0;

byte TimeHours = 0; // Часы
byte TimeMinutes = 0; // Минуты

byte Dashboard_Backlight = 100; // Яркость подсветки приборной панели (стрелки, шкалы)
byte MiniDisplays_Backlight = 100; // Яркость подстветки экранчиков (время, пробег)
byte Icons_Backlight = 100; // Яркость подсветки иконок
byte Sound_Loudness = 85; // Громкость звукового сигнала

float OutsideTemperature = 18.0; // Температура воздуха снаружи

// 500ms blinker
bool LightParking = false;
bool IconLeftTrailer = false;
bool IconLeftTruck = false;
bool IconRightTrailer = false;
bool IconRightTruck = false;

bool IndicatorLowFuel = false;
bool IndicatorTacho = false;

bool IconStop = false;
bool IconLowBeam = false;
bool IconHighBeam = false;
bool IconFogFront = false;
bool IconFogRear = false;
bool IconRetarder = false;
bool IconLowGear = false;
bool IconAxisBlock = false;
bool IconPTO = false;
bool IconCoil = false;
bool IconWindowCycleArrow = false;
bool IconParkingBrake = false;
bool IconASR = false;
bool IconTrailerAbs = false;
bool IconWorkLight = false;
bool IconChassisTransport = false;

bool SoundBlinker = false;
bool SoundWarnOnce = false;
bool SoundWarnCont = false;
bool SoundAlarmOnce = false;
bool SoundAlarmCont = false;

bool Warning = false;
bool Alarm = false;
unsigned long WarnTimer = 0;

byte DisplayActive = false;
byte DisplayWarningsCount = 1;

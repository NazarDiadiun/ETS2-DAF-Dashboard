bool GamePaused = false;
byte TimeHours = 0; // Часы
byte TimeMinutes = 0; // Минуты
int NextRest = 0; // Time for next rest stop

// Truck Variables
float Speed = 0.0; // Скорость автомобиля
float EngineSpeed = 0; // Обороты двигателя
int FuelCapacity = 0; // Fuel tank capacity
int FuelAmount = 0; // Amount of fuel in tank
int Range = 0; // Range in km on estimated fuel
int AvFuelCons = 0; // Average Fuel Consumption x 10
int AdBlueCapacity = 0; // Adblue tank capacity
int AdBlueAmount = 0; // Adblue current amount
int OilPressure = 0; // Oil Pressure Value in psi
int OilTemperature = 0; // Oil Temperautre Value int Celcius
int WaterTemperature = 0; // Температура охл. жидкости
int AirPressure = 0; // Air Pressure Value in psi
float BatteryVoltage = 0; // Battery Voltage Value * 10 in Volts (245 = 24.5V)
int CruiseControl = 0; // Cruise control speed (0 if disabled)
int Gear = 0; // Current gear
int EffectiveThrottle = 0; // Current Effective Throtle Value 0 - 100%
int BrakesTemperature = 0; // Current Brakes tempereature in Celsius
bool Ignition = false; // ignition on flag
bool EngineOn = false; // Engine on flag
bool TrailerConnected = false;
float Distance = 0.0; // Общий пробег
int SpeedLimit = 0; // Current Speed limit on road

// Other computed values
float DistanceDaily = 0.0; // Суточный пробег
float DistanceEEPROM = 0.0;


int Fuel = 0, Air1 = 0, Air2 = 0, AdBlue = 0;

// Warnings
bool LowOilPressure = false;
bool LowBattery = false;
bool AirPressureWarning = false;
bool AirPressureEmergency = false;
bool WaterTempWarning = false;
bool IndicatorLowFuel = false;
bool LowAdBlue = false;

// Warnings
bool IconParkingBrake = false;
bool IconLeftTruck = false;
bool IconRightTruck = false;
bool LightParking = false;
bool IconLowBeam = false;
bool IconHighBeam = false;
// Beacon lights
bool IconRetarder = false;
bool IconAxisBlock = false;

byte EngineWear = 0, TransmissionWear = 0, CabinWear = 0, ChassisWear = 0, WheelsWear = 0;


byte Dashboard_Backlight = 100; // Яркость подсветки приборной панели (стрелки, шкалы)
byte MiniDisplays_Backlight = 100; // Яркость подстветки экранчиков (время, пробег)
byte Icons_Backlight = 100; // Яркость подсветки иконок
byte Sound_Loudness = 70; // Громкость звукового сигнала

float OutsideTemperature = 18.0; // Температура воздуха снаружи

// 500ms blinker

bool IndicatorTacho = false;

bool IconLeftTrailer = false;
bool IconRightTrailer = false;
bool IconStop = false;
bool IconFogFront = false;
bool IconFogRear = false;
bool IconLowGear = false;
bool IconPTO = false;
bool IconCoil = false;
bool IconWindowCycleArrow = false;
bool IconASR = false;
bool IconTrailerAbs = false;
bool IconWorkLight = false;
bool IconChassisTransport = false;

bool SoundBlinker = false;
bool SoundWarnOnce = false;
bool SoundAlarmOnce = false;
bool SoundAlarmCont = false;

int DispPage = 0; // Current Info Page Displayed
byte DisplayActive = false;
byte DisplayAlarmCount = 0;
byte DisplayWarningsCount = 0;

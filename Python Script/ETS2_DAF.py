import requests
import serial
import json
import time
import serial

from pynput.keyboard import Key, Controller
from pynput import keyboard
keyboard = Controller()


def on_press(key):
    try:
        print('alphanumeric key {0} pressed'.format(
            key.char))
        if key.char == 'i':
            print("Next Display!")
    except AttributeError:
        print('special key {0} pressed'.format(
            key))

def round_floats(o):
    if isinstance(o, float):
        return round(o, 2)
    if isinstance(o, dict):
        return {k: round_floats(v) for k, v in o.items()}
    if isinstance(o, (list, tuple)):
        return [round_floats(x) for x in o]
    return o

i=0

DAF = {
    "Game": {},
    "Params": {},
    "Icon": {},
    "Warn": {}
}

ser = serial.Serial(
    port="COM3",
    baudrate=115200,
    bytesize=8,
    parity='N',
    stopbits=1,
    write_timeout=0,
    timeout=0,
    rtscts=False,
    dsrdtr=False)


url = 'http://192.168.1.10:25555/api/ets2/telemetry'


while (True):
    ser.flushInput()
    ser.flushOutput()

    HTTPresponse = requests.get(url).json()
    Time = HTTPresponse['game']['time']

# 0 - Paused
# 1 - Time Hours
# 2 - Time Minutes

    gamearr = [int(HTTPresponse['game']['paused']),
               int(Time[11]) * 10 + int(Time[12]),
               int(Time[14]) * 10 + int(Time[15])]

    DAF['Game'] = gamearr

# 0 - Speed
# 1 - RPM
# 2 - Fuel
# 3 - Water Temp
# 4 - Adblue
# 5 - Air Pressure
# 6 - Oil Temp
# 7 - Oil Pressure
# 8 - Average Fuel Cons.
# 9 - Battery Voltage
# 10 - Current Gear
# 11 - Ignition
# 12 - Engine on
# 13 - Trailer Attached
# 14 - Odometer

    paramsarr = [int(abs(HTTPresponse['truck']['speed'])) * 10,
                 int(HTTPresponse['truck']['engineRpm']),
                 int(int(HTTPresponse['truck']['fuel']) /
                 int(HTTPresponse['truck']['fuelCapacity']) * 1000),
                 int(HTTPresponse['truck']['waterTemperature']), 
                 int(HTTPresponse['truck']['adblue']) /
                 int(HTTPresponse['truck']['adblueCapacity']) * 100,
                 int(HTTPresponse['truck']['airPressure']) * 10,
                 int(HTTPresponse['truck']['oilTemperature']) * 10,
                 int(HTTPresponse['truck']['oilPressure'])*10,
                 int(HTTPresponse['truck']['fuelAverageConsumption']) * 10,
                 int(HTTPresponse['truck']['batteryVoltage']) * 10,
                 HTTPresponse['truck']['gear'],
                 int(HTTPresponse['truck']['electricOn']),
                 int(HTTPresponse['truck']['engineOn']),
                 int(HTTPresponse['trailer1']['attached']),
                 int(HTTPresponse['truck']['odometer'] * 10)]

    DAF['Params'] = paramsarr

# 0 - Adblue Warning
# 1 - Air Pressure Warning
# 2 - Low Fuel Warning
# 3 - Oil Pressure
# 4 - Water Temp
# 5 - Battery
# 6 - Air Pressure Emergency

    warnarr = [int(HTTPresponse['truck']['adblueWarningOn']),
               int(HTTPresponse['truck']['airPressureWarningOn']),
               int(HTTPresponse['truck']['fuelWarningOn']),
               int(HTTPresponse['truck']['oilPressureWarningOn']),
               int(HTTPresponse['truck']['waterTemperatureWarningOn']),
               int(HTTPresponse['truck']['batteryVoltageWarningOn']),
               int(HTTPresponse['truck']['airPressureEmergencyOn'])]

    DAF['Warn'] = warnarr

# 0 - Retarder
# 1 - Parking Brake
# 2 - Left Blinker 
# 3 - Right Blinker 
# 4 - Lights Parking
# 5 - Lights Low Beam
# 6 - Lights High Beam
# 7 - Lights AUX
# 8 - Dashboard Brightness

    iconarr = [int(HTTPresponse['truck']['retarderBrake']),
               int(HTTPresponse['truck']['parkBrakeOn']),
               int(HTTPresponse['truck']['blinkerLeftOn']),
               int(HTTPresponse['truck']['blinkerRightOn']),
               int(HTTPresponse['truck']['lightsParkingOn']),
               int(HTTPresponse['truck']['lightsBeamLowOn']),
               int(HTTPresponse['truck']['lightsBeamHighOn']),
               int(HTTPresponse['truck']['lightsAuxFrontOn']),
               int(HTTPresponse['truck']['lightsDashboardValue'] * 100)]

    DAF['Icon'] = iconarr

    #print(DAF['elOn'])
    SendString = json.dumps(round_floats(DAF))

    ser.write((SendString + '\n').encode())

    time.sleep(0.07)

    SerialCommand = ""
    SerialCommandArrived = False

    while ser.in_waiting:
        SerialCommand = ser.readline().decode()
        SerialCommandArrived = True
    
    if(SerialCommandArrived):
        print(SerialCommand)
        if(SerialCommand == "MileageReset\n"):
            print("Reset Mileage Button")
            keyboard.press('`')
            keyboard.release('`')
        SerialCommandArrived = False
    

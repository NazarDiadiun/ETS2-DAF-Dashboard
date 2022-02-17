import requests
import serial
import json
import time
import serial

from pynput.keyboard import Key, Controller
from pynput import keyboard
keyboard = Controller()

# URL to parse telemetry data
url = 'http://192.168.1.10:25555/api/ets2/telemetry'
# COM port to connect to dashboard controller
COMport = "COM3"
# Controller send frequency (Hz)
frequency = 15
# Daily Mileage (Trip Mileage) Button to reset in ETS2
mileagerstbtn = '`'
# Next Page Button (Button in ETS2 to change page on dashboard display)
displaynxtpg = 'i'


def on_press(key):
    try:
        print('alphanumeric key {0} pressed'.format(
            key.char))
        if key.char == displaynxtpg:
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

DAF = {
    "Game": {},
    "Params": {},
    "Icon": {},
    "Warn": {}
}

ser = serial.Serial(
    port=COMport,
    baudrate=115200,
    bytesize=8,
    parity='N',
    stopbits=1,
    write_timeout=0,
    timeout=0,
    rtscts=False,
    dsrdtr=False)


while (True):
    ser.flushInput()
    ser.flushOutput()

    HTTPresponse = requests.get(url).json()
    Time = HTTPresponse['game']['time']

# 0 - Paused (bool. Game Pause)
# 1 - Time Hours (integer. Game Current Hours)
# 2 - Time Minutes (integer. Game Current Minutes)

    gamearr = [int(HTTPresponse['game']['paused']),
               int(Time[11]) * 10 + int(Time[12]),
               int(Time[14]) * 10 + int(Time[15])]

    DAF['Game'] = gamearr

# 0 - Speed                         (integer, absolute, x10. example: 45.8kmh = 458)
# 1 - RPM                           (integer. example: 2100rpm)
# 2 - Fuel Level                    (integer, x10. example: 56.5% = 565)
# 3 - Water Temp                    (integer. example: 85 Celsius)
# 4 - Adblue Level                  (integer, x10. example: 25.0% = 250)
# 5 - Air Pressure                  (integer. example: 125psi)
# 6 - Oil Temp                      (integer. example 55 Celsius)
# 7 - Oil Pressure                  (integer. example 70psi)
# 8 - Average Fuel Cons.            (integer, x100. example: 0.30l/km = 30)
# 9 - Battery Voltage               (integer, x10. example 27.1V = 271)
# 10 - Current Gear                 (integer. example: 3)
# 11 - Ignition                     (bool. Current Ignition State)
# 12 - Engine on                    (bool. Current Engine Running State)
# 13 - Trailer Attached             (bool. Trailer Attached?)
# 14 - Odometer                     (integer, x10. example 13250.8km = 132508)

    paramsarr = [int(abs(HTTPresponse['truck']['speed'])) * 10,
                 int(HTTPresponse['truck']['engineRpm']),
                 int(int(HTTPresponse['truck']['fuel']) /
                 int(HTTPresponse['truck']['fuelCapacity']) * 1000),
                 int(HTTPresponse['truck']['waterTemperature']),
                 int(HTTPresponse['truck']['adblue']) /
                 int(HTTPresponse['truck']['adblueCapacity']) * 1000,
                 int(HTTPresponse['truck']['airPressure']),
                 int(HTTPresponse['truck']['oilTemperature']),
                 int(HTTPresponse['truck']['oilPressure']),
                 int(HTTPresponse['truck']['fuelAverageConsumption']) * 100,
                 int(HTTPresponse['truck']['batteryVoltage']) * 10,
                 HTTPresponse['truck']['gear'],
                 int(HTTPresponse['truck']['electricOn']),
                 int(HTTPresponse['truck']['engineOn']),
                 int(HTTPresponse['trailer1']['attached']),
                 int(HTTPresponse['truck']['odometer'] * 10)]

    DAF['Params'] = paramsarr

# 0 - Adblue Warning                (bool. Is AdBlue Level Low?)
# 1 - Air Pressure Warning          (bool. Is Air Pressure Low?)
# 2 - Low Fuel Warning              (bool. Is Fuel Level Low?)
# 3 - Oil Pressure                  (bool. Is Oil Pressure Low?)
# 4 - Water Temp                    (bool. Is Water Temperature too high?)
# 5 - Battery                       (bool. Is Battery Voltage Low?)
# 6 - Air Pressure Emergency        (bool. Is Air Pressure Critically Low)

    warnarr = [int(HTTPresponse['truck']['adblueWarningOn']),
               int(HTTPresponse['truck']['airPressureWarningOn']),
               int(HTTPresponse['truck']['fuelWarningOn']),
               int(HTTPresponse['truck']['oilPressureWarningOn']),
               int(HTTPresponse['truck']['waterTemperatureWarningOn']),
               int(HTTPresponse['truck']['batteryVoltageWarningOn']),
               int(HTTPresponse['truck']['airPressureEmergencyOn'])]

    DAF['Warn'] = warnarr

# 0 - Retarder                      (bool. Is Retarder On?)
# 1 - Parking Brake                 (bool. Is Parking Brake applied?)
# 2 - Left Blinker                  (bool. Is Left Blinker On?)
# 3 - Right Blinker                 (bool. Is Right Blinker On?)
# 4 - Lights Parking                (bool. Is Parking Lights On?)
# 5 - Lights Low Beam               (bool. Is Low Beam Lights On?)
# 6 - Lights High Beam              (bool. Is High Beam Lights On?)
# 7 - Lights AUX                    (bool. Is Left AUX Lights On?)
# 8 - Dashboard Brightness          (integer, x10. Dashboard backlight brightness)

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

    SendString = json.dumps(round_floats(DAF))

    ser.write((SendString + '\n').encode())

    time.sleep(frequency)

    SerialCommand = ""
    SerialCommandArrived = False

    while ser.in_waiting:
        SerialCommand = ser.readline().decode()
        SerialCommandArrived = True

    if(SerialCommandArrived):
        print(SerialCommand)
        if(SerialCommand == "MileageReset\n"):
            print("Reset Mileage Button")
            keyboard.press(mileagerstbtn)
            keyboard.release(mileagerstbtn)
        SerialCommandArrived = False

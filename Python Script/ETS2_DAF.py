import serial
import json
import time
import serial

# TODO
# Add Trailer attached bool

# For Shared Memory
import mmap
import struct

# Keyboard emulation
from pynput.keyboard import Key, Controller
from pynput import keyboard
Keyboard = Controller()

# COM port to connect to dashboard controller
COMport = "COM3"
# Controller send frequency (Hz)
frequency = 15
# Daily Mileage (Trip Mileage) Button to reset in ETS2
mileagerstbtn = '`'
# Next Page Button (Button in ETS2 to change page on dashboard display)
displaynxtpg = 'i'

game_paused = 1

send_complete = 0

mm = mmap.mmap(0, 1620, "Local\\SCSTelemetry")

DAF = {
    "Game": {},
    "Params": {},
    "Icon": {},
    "Warn": {},
    "Wear": {},
    "Cmd": {}
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
    
def on_press(key):
    if hasattr (key, 'char'):
        if key.char == 'i':
            if game_paused == 0:
                DAF['Cmd'] = [1]

listener = keyboard.Listener(
    on_press=on_press)
listener.start()

while (True):
    send_complete = 0
    
    sdk_active = struct.unpack("?", mm[0:1])[0]
    game_paused = struct.unpack("?", mm[4:5])[0]

    # Unsigned Integers - "I"
    # 40 telemetry_plugin_revision
    # 44 version_major
    # 48 version_minor
    # 52 game
    game_type = struct.unpack("I", mm[52:56])[0]
    # 56 telemetry_version_game_major
    # 60 telemetry_version_game_minor
    # 64 time_abs
    game_time = struct.unpack("I", mm[64:68])[0]
    game_time_h = (game_time / 60) % 24
    game_time_m = game_time % 60
    # 68 gears
    # 72 gears_reverse
    # 76 retarderStepCount
    truck_retarderStepCount = struct.unpack("I", mm[76:80])[0]
    # 80 truckWheelCount
    # 84 selectorCount
    # 88 time_abs_delivery
    # 92 maxTrailerCount
    # 96 unitCount
    # 100 plannedDistanceKm
    # 104 shifterSlot
    # 108 retarderBrake
    truck_retarderBrake = struct.unpack("I", mm[108:112])[0]
    # 112 lightsAuxFront
    # 116 lightsAuxRoof

    # offset 500 - integers "i"
    # 500 restStop
    rest_stop = struct.unpack("i", mm[500:504])[0]
    # 504 gear
    truck_gear = struct.unpack("i", mm[504:508])[0]
    # 508 gearDashboard
    truck_gearDashboard = struct.unpack("i", mm[508:512])[0]

    # offset 700 - floats "f"
    # 700 scale
    # 704 fuelCapacity
    truck_fuelCapacity = struct.unpack("f", mm[704:708])[0]
    # 708 fuelWarningFactor
    # 712 adblueCapacity
    truck_adblueCapacity = struct.unpack("f", mm[712:716])[0]
    # 716 adblueWarningFactor
    # 720 airPressureWarning (warning pressure)
    # 724 airPressurEmergency (Emergency pressure)
    # 728 oilPressureWarning (warning pressure)
    # 732 waterTemperatureWarning
    # 736 batteryVoltageWarning
    # 740 engineRpmMax
    # 744 gearDifferential
    # 748 cargoMass
    truck_cargoMass = struct.unpack("f", mm[748:752])[0]
    # 752 truckWheelRadius[16]
    # 816 gearRatiosForward[24]
    # 912 gearRatiosReverse[8]
    # 944 unitMass
    # 948 speed
    truck_speed = abs(struct.unpack("f", mm[948:952])[0]) * 3.6
    # 952 engineRpm
    truck_rpm = struct.unpack("f", mm[952:956])[0]
    # 956 userSteer
    # 960 userThrottle
    # 964 userBrake
    # 968 userClutch
    # 972 gameSteer
    # 976 gameThrottle
    truck_throttle = struct.unpack("f", mm[976:980])[0]
    # 980 gameBrake
    # 984 gameClutch
    # 988 cruiseControlSpeed
    truck_cruiseControlSpeed = struct.unpack("f", mm[988:992])[0] * 3.6
    # 992 airPressure
    truck_airPressure = struct.unpack("f", mm[992:996])[0]
    # 996 brakeTemperature
    truck_brakeTemperature = struct.unpack("f", mm[996:1000])[0]
    # 1000 FuelAmount
    truck_FuelAmount = struct.unpack("f", mm[1000:1004])[0]
    # 1004 fuelAvgConsumption
    truck_fuelAvgConsumption = struct.unpack("f", mm[1004:1008])[0]
    # 1008 fuelRange
    truck_fuelRange = struct.unpack("f", mm[1008:1012])[0]
    # 1012 adblue
    truck_adblue = struct.unpack("f", mm[1012:1016])[0]
    # 1016 oilPressure
    truck_oilPressure = struct.unpack("f", mm[1016:1020])[0]
    # 1020 oilTemperature
    truck_oilTemperature = struct.unpack("f", mm[1020:1024])[0]
    # 1024 waterTemperature
    truck_waterTemperature = struct.unpack("f", mm[1024:1028])[0]
    # 1028 batteryVoltage
    truck_batteryVoltage = struct.unpack("f", mm[1028:1032])[0]
    # 1032 lightsDashboard
    truck_lightsDashboard = struct.unpack("f", mm[1032:1036])[0]
    # 1036 wearEngine
    truck_wearEngine = struct.unpack("f", mm[1036:1040])[0]
    # 1040 wearTransmission
    truck_wearTransmission = struct.unpack("f", mm[1040:1044])[0]
    # 1044 wearCabin
    truck_wearCabin = struct.unpack("f", mm[1044:1048])[0]
    # 1048 wearChassis
    truck_wearChassis = struct.unpack("f", mm[1048:1052])[0]
    # 1052 wearWheels
    truck_wearWheels = struct.unpack("f", mm[1052:1056])[0]
    # 1056 truckOdometer
    truck_truckOdometer = struct.unpack("f", mm[1056:1060])[0]
    # 1060 routeDistance
    truck_routeDistance = struct.unpack("f", mm[1060:1064])[0]
    # 1064 routeTime
    truck_routeTime = struct.unpack("f", mm[1064:1068])[0]
    # 1068 speedLimit
    truck_speedLimit = struct.unpack("f", mm[1068:1072])[0]
    # 1072 truck_wheelSuspDeflection[16]
    # 1136 truck_wheelVelocity[16]
    # 1200 truck_wheelSteering[16]
    # 1264 truck_wheelRotation[16]
    # 1328 truck_wheelLift[16]
    # 1392 truck_wheelLiftOffset[16]
    # 1456 jobDeliveredCargoDamage
    # 1460 jobDeliveredDistanceKm
    # 1464 refuelAmount
    # 1468 cargoDamage

    # offset 1500 bools - "?'
    # 1500 truckWheelSteerable[16]
    # 1516 truckWheelSimulated[16]
    # 1532 truckWheelPowered[16]
    # 1548 truckWheelLiftable[16]
    # 1564 isCargoLoaded
    # 1565 specialJob
    # 1566 parkBrake
    indicator_parkBrake = struct.unpack("?", mm[1566:1567])[0]
    # 1567 motorBrake
    indicator_motorBrake = struct.unpack("?", mm[1567:1568])[0]
    # 1568 airPressureWarning
    indicator_airPressureWarning = struct.unpack("?", mm[1568:1569])[0]
    # 1569 airPressureEmergency
    indicator_airPressureEmergency = struct.unpack("?", mm[1569:1570])[0]
    # 1570 fuelWarning
    indicator_fuelWarning = struct.unpack("?", mm[1570:1571])[0]
    # 1571 adblueWarning
    indicator_adblueWarning = struct.unpack("?", mm[1571:1572])[0]
    # 1572 oilPressureWarning
    indicator_oilPressureWarning = struct.unpack("?", mm[1572:1573])[0]
    # 1573 waterTemperatureWarning
    indicator_waterTemperatureWarning = struct.unpack("?", mm[1573:1574])[0]
    # 1574 batteryVoltageWarning
    indicator_batteryVoltageWarning = struct.unpack("?", mm[1574:1575])[0]
    # 1575 electricEnabled
    indicator_electricEnabled = struct.unpack("?", mm[1575:1576])[0]
    # 1576 engineEnabled
    indicator_engineEnabled = struct.unpack("?", mm[1576:1577])[0]
    # 1577 wipers
    # 1578 blinkerLeftActive
    # 1579 blinkerRightActive
    # 1580 blinkerLeftOn
    indicator_blinkerLeftOn = struct.unpack("?", mm[1580:1581])[0]
    # 1581 blinkerRightOn
    indicator_blinkerRightOn = struct.unpack("?", mm[1581:1582])[0]
    # 1582 lightsParking
    indicator_lightsParking = struct.unpack("?", mm[1582:1583])[0]
    # 1583 lightsBeamLow
    indicator_lightsBeamLow = struct.unpack("?", mm[1583:1584])[0]
    # 1584 lightsBeamHigh
    indicator_lightsBeamHigh = struct.unpack("?", mm[1584:1585])[0]
    # 1585 lightsBeacon
    indicator_lightsBeacon = struct.unpack("?", mm[1585:1586])[0]
    # 1586 lightsBrake
    # 1587 lightsReverse
    # 1588 lightsHazard
    indicator_lightsHazard = struct.unpack("?", mm[1588:1589])[0]
    # 1589 cruiseControl
    indicator_cruiseControl = struct.unpack("?", mm[1589:1590])[0]
    # 1590 truck_wheelOnGround[16]
    # 1606 shifterToggle[2]
    indicator_shifterToggle0 = struct.unpack("?", mm[1606:1607])[0]
    indicator_shifterToggle1 = struct.unpack("?", mm[1607:1608])[0]
    # 1608 differentialLock
    indicator_differentialLock = struct.unpack("?", mm[1608:1609])[0]
    # 1609 liftAxle
    # 1610 liftAxleIndicator
    indicator_liftAxleIndicator = struct.unpack("?", mm[1610:1611])[0]
    # 1611 trailerLiftAxle
    # 1612 trailerLiftAxleIndicator
    indicator_trailerLiftAxleIndicator = struct.unpack("?", mm[1612:1613])[0]
    # 1613 jobDeliveredAutoparkUsed
    # 1614 jobDeliveredAutoloadUsed


    ser.flushInput()
    ser.flushOutput()

# 0 - Paused (bool. Game Pause)
# 1 - Time Hours (integer. Game Current Hours)
# 2 - Time Minutes (integer. Game Current Minutes)
# 3 - Rest stop (integer. Time to next rest stop in minutes)

    gamearr = [int(game_paused),
               int(game_time_h),
               int(game_time_m),
               int(rest_stop)]

    DAF['Game'] = gamearr

# 0 - Speed                         (integer, absolute, x10. example: 45.8kmh = 458)
# 1 - RPM                           (integer. example: 2100rpm)
# 2 - Fuel Capacity                 (integer. Example: 1000l)
# 3 - Fuel Amount                   (integer. Example: 223l)
# 4 - Fuel Range                    (integer. Example: 650km)
# 5 - Fuel Average Consumption      (integer. x1000. Example: 29.5l/100km = 295)
# 6 - Adblue Capacity               (integer. Example: 40l)
# 7 - Adblue Amount                 (integer. Example: 25l)
# 8 - Oil Pressure                  (integer. example 70psi)
# 9 - Oil Temp                      (integer. example 55 Celsius)
# 10 - Water Temp                   (integer. example: 85 Celsius)
# 11 - Air Pressure                 (integer. example: 125psi)
# 12 - Battery Voltage              (integer, x10. example 27.1V = 271)
# 13 - Cruise Control Speed         (integer. example: 85kmh)
# 14 - Current gear                 (integer. example: 3)
# 15 - Effective Throttle           (integer. x100. Example: 56%)
# 16 - Brakes Temperature           (integer. Example: 200 Celsius)
# 17 - Ignition                     (bool. Current Ignition State)
# 18 - Engine on                    (bool. Current Engine Running State)
# 19 - Trailer Attached             (bool. Trailer Attached?)
# 20 - Odometer                     (integer, x10. example 13250.8km = 132508)
# 21 - Speed Limit                  (integer, Example: 90kmh)

    paramsarr = [int(truck_speed * 10),
                 int(truck_rpm),
                 int(truck_fuelCapacity),
                 int(truck_FuelAmount),
                 int(truck_fuelRange),
                 int(truck_fuelAvgConsumption * 1000),
                 int(truck_adblueCapacity),
                 int(truck_adblue),
                 int(truck_oilPressure),
                 int(truck_oilTemperature),
                 int(truck_waterTemperature),
                 int(truck_airPressure),
                 int(truck_batteryVoltage * 10),
                 int(truck_cruiseControlSpeed),
                 int(truck_gearDashboard),
                 int(truck_throttle) * 100,
                 int(truck_brakeTemperature),
                 int(indicator_electricEnabled),
                 int(indicator_engineEnabled),
                 int(1),
                 int(truck_truckOdometer * 10),
                 int(truck_speedLimit)]

    DAF['Params'] = paramsarr

# 0 - Oil Pressure                  (bool. Is Oil Pressure Low?)
# 1 - Battery                       (bool. Is Battery Voltage Low?)
# 2 - Air Pressure Warning          (bool. Is Air Pressure Low?)
# 3 - Air Pressure Emergency        (bool. Is Air Pressure Critically Low)
# 4 - Water Temp                    (bool. Is Water Temperature too high?)
# 5 - Low Fuel Warning              (bool. Is Fuel Level Low?)
# 6 - Adblue Warning                (bool. Is AdBlue Level Low?)

    warnarr = [
        int(indicator_oilPressureWarning),
        int(indicator_batteryVoltageWarning),
        int(indicator_airPressureWarning),
        int(indicator_airPressureEmergency),
        int(indicator_waterTemperatureWarning),
        int(indicator_fuelWarning),
        int(indicator_adblueWarning)]

    DAF['Warn'] = warnarr

# 0 - Parking Brake                 (bool. Is Parking Brake applied?)
# 1 - Left Blinker                  (bool. Is Left Blinker On?)
# 2 - Right Blinker                 (bool. Is Right Blinker On?)
# 3 - Lights Parking                (bool. Is Parking Lights On?)
# 4 - Lights Low Beam               (bool. Is Low Beam Lights On?)
# 5 - Lights High Beam              (bool. Is High Beam Lights On?)
# 6 - Lights beacon                 (bool. Is beacon Lights On?)
# 7 - Retarder                      (bool. Is Retarder On?)
# 8 - TruckLiftAxle                 (bool. Is Truck liftable axle lifted ?)
# 9 - TrailerLiftAxle               (bool. Is Trailer liftable axle lifted ?)
# 10 - DifferentialLock             (bool. Is Differential Locked ?)
# 11 - Dashboard Brightness         (integer, x10. Dashboard backlight brightness)

    iconarr = [
        int(indicator_parkBrake),
        int(indicator_blinkerLeftOn),
        int(indicator_blinkerRightOn),
        int(indicator_lightsParking),
        int(indicator_lightsBeamLow),
        int(indicator_lightsBeamHigh),
        int(indicator_lightsBeacon),
        int(truck_retarderBrake),
        int(indicator_liftAxleIndicator),
        int(indicator_trailerLiftAxleIndicator),
        int(indicator_differentialLock),
        int(truck_lightsDashboard) * 100]

    DAF['Icon'] = iconarr

# 0 - Engine Wear
# 1 - Transmission Wear
# 2 - Cabin Wear
# 3 - Chassis Wear
# 4 - Wheels Wear

    weararr = [int(truck_wearEngine * 100),
               int(truck_wearTransmission * 100),
               int(truck_wearCabin * 100),
               int(truck_wearChassis * 100),
               int(truck_wearWheels * 100)]

    DAF['Wear'] = weararr

    SendString = json.dumps(DAF)

    ser.write((SendString + '\n').encode())
    
    time.sleep(0.1)

    SerialCommand = ""
    SerialCommandArrived = False

    while ser.in_waiting:
        SerialCommand = ser.readline().decode()
        SerialCommandArrived = True

    if(SerialCommandArrived):
        print(SerialCommand)
        if(SerialCommand == "MileageReset\n"):
            print("Reset Mileage Button")
            Keyboard.press(mileagerstbtn)
            Keyboard.release(mileagerstbtn)
        if(SerialCommand == "nxtpgOk\n"):
            DAF['Cmd'] = [0]
        SerialCommandArrived = False

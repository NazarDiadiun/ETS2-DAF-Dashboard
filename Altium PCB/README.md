# PCB

The dashboard needs two CAN interfaces to function properly and fully. I decided not to invent solutions on a breadboard, but to design a printed circuit board on which I placed all the necessary components.
Project sources, archive with Gerber files, some screenshots and schematic I uploaded to this directory

## Microcontroller
The microcontroller selected ATmega32u4 as the microcontroller - it is easy to plug, supports work as a HID device and other.

## CAN Bus
CAN built on MCP2515 and TJA1050
There are switches to connect 120 Ohm resistors parallel to CAN Low and CAN High outputs

## USB
Two USB Type-C connectors are used to transfer data and power the dashboard.

## Dashboard Power
To power the dashboard it is necessary 24B, which is obtained by means of the MT3608 converter.

## Other Peripheral
Also added some buttons just in case.
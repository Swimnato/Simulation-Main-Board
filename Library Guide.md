# Programming with the Simulation Main Board
## Table Of contents
[Analog Inputs](#AnalogInputs)

## General Prerequisites
Copy the necessary folder of dependancies over to ```/lib```. Include the API by calling ```#include <MainBoard.h>``` at the top of your main.cpp file.

To initialize the main board please call ```MainBoardStart()``` This should be called before making any calls to any other functions in the main board API or any of its resources (Ideally this should be your first line of code in ```void setup()```)

## Analog Inputs
After [initializing the board](#GeneralPrerequisites) you can call ```mainBoardGetAnalogMux(PIN_ADDRESS)``` to read an an analog input. PIN_ADDRESS should be replaced with whatever the address for your pin is.

## Digital IO
After [initializing the board](#GeneralPrerequisites) you are given access to the following commands:

```mainBoardDigitalPinMode(uint8_t pin, uint8_t mode)``` This Initializes the given pin to the funciton you want (INPUT, OUTPUT, the same way as you would for a standard arduino pin. There are no built in pullup/pulldown resistors to enable in software, rather this will depend on what resistors have been installed on the physical board).

```mainBoard16DigitalPinMode(uint8_t chip, uint16_t mode)``` This lets you set all 16 pins at once. This is a wrapper around someone elses' code and best I can tell a 1 bit is setting that pin as an input, and a 0 is setting it as an output.

```mainBoardGetDigitalInput(uint8_t pin)``` Reads a digital pin and returns the result.

```uint16_t mainBoardGet16DigitalInput(uint8_t chip)``` Reads an entire port, returns a 16 bit int, lsb is pin 1 and msb is pin 16

```mainBoardWriteDigitalOutput(uint8_t pin, uint8_t val)``` Writes a single digital output.

```bool mainBoardWrite16DigitalOutput(uint8_t chip, uint16_t val)``` Writes an entire port's digital output. lsb is pin 1 and msb is pin 16

## UART 1
After [initializing the board](#GeneralPrerequisites)

## I2C Mux
After [initializing the board](#GeneralPrerequisites)

## SD
After [initializing the board](#GeneralPrerequisites)

## Ethernet
After [initializing the board](#GeneralPrerequisites)

## Servos
Just use an ESP32Servo library (I have tested madhephaestus/ESP32Servo@^3.0.9 and it works fine) like usual.
Your available servo pins are:
```
MAIN_BOARD_SERVO_1_PIN
MAIN_BOARD_SERVO_2_PIN
MAIN_BOARD_SERVO_3_PIN
MAIN_BOARD_SERVO_4_PIN
```

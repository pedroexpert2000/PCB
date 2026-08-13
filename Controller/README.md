# PCB — PS4-controlled motor driver (ESP32 + Bluepad32)

A simple controller firmware that lets a PS4 gamepad drive two DC motors (left/right) via an H-bridge using an ESP32 and the Bluepad32 library over Bluetooth. The left joystick controls the left motor and the right joystick controls the right motor. Deadzone and PWM mapping are implemented in the sketch.

## Stack
- Language: C++ (Arduino / ESP32)
- Runtime: Arduino core for ESP32
- Notable library: Bluepad32 (Bluetooth gamepad support)

## Features
- Connect a PS4 controller over Bluetooth (Bluepad32).
- Tank-style control: left joystick -> left motor, right joystick -> right motor.
- PWM speed control and direction via an H-bridge.
- Motor safety: standby pin and frenarTodo() stop routine.

## Pinout / Wiring (as used in Controller/Controller.ino)
Match these to your ESP32 board pins and your H-bridge inputs.

- STBY (Standby / enable H-bridge): GPIO 4
- Right motor (Motor Derecho — mapped to the right joystick):
  - PWMA (PWM speed): GPIO 16
  - AIN1 (direction): GPIO 18
  - AIN2 (direction): GPIO 19
- Left motor (Motor Izquierdo — mapped to the left joystick):
  - PWMB (PWM speed): GPIO 17
  - BIN1 (direction): GPIO 22
  - BIN2 (direction): GPIO 23

Wiring notes:
- Connect the H-bridge inputs (AIN1/AIN2/PWMA and BIN1/BIN2/PWMB) to the ESP32 pins above.
- Connect STBY to the H-bridge standby/enable pin (or VCC via a transistor if required), controlled by GPIO 4.
- Power motors from an appropriate motor power supply (separate from the ESP32 3.3V/5V supply). Connect grounds together (ESP32 GND and motor supply GND).
- Protect motors with appropriate diodes/fuses and ensure the H-bridge can handle motor stall current.

## Software / prerequisites
- Arduino IDE or PlatformIO configured for ESP32.
- ESP32 Arduino core installed.
- Bluepad32 library (Bluepad32 for ESP32) installed. See: https://github.com/bluepad32/bluepad32 (or use the Library Manager / PlatformIO registry).

## Usage / how to upload
1. Open the Arduino IDE (or PlatformIO).
2. Install the ESP32 boards package and select your ESP32 board and COM/serial port.
3. Install the Bluepad32 library.
4. Open `Controller/Controller.ino` (the sketch).
5. Compile & upload to the ESP32.
6. Power the motor power supply and the ESP32.
7. Pair/connect a PS4 controller. The sketch prints to Serial:
   - "Esperando conexion del control de PS4..." while waiting
   - "Mando PS4 conectado" when paired
   - "Mando PS4 desconectado" on disconnect

When a controller is connected:
- Left joystick vertical axis (axisY) controls the left motor.
- Right joystick vertical axis (axisRY) controls the right motor.
- A deadzone of ±30 on the joystick is used to avoid small unintended movements.
- Joystick values are read from approximately -511 to +512 and mapped to 0–255 PWM.

Example minimal Arduino upload commands (Arduino CLI):
```bash
# install board package (example for esp32 - adjust to your board)
arduino-cli core update-index
arduino-cli core install esp32:esp32

# compile and upload (replace board/port and path as needed)
arduino-cli compile --fqbn esp32:esp32:esp32dev /path/to/Controller
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32dev /path/to/Controller

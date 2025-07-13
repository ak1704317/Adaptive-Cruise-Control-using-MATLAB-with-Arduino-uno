# Adaptive-Cruise-Control-using-MATLAB-with-Arduino-uno


This project implements an **Adaptive Cruise Control (ACC)** system using **Arduino Uno**, **MATLAB**, an **ultrasonic sensor**, push buttons, LEDs, and a **DC motor** with PWM speed control.

The system features **three operating modes**:
- **Normal Mode**: Manual acceleration/deceleration using buttons
- **Cruise Mode**: Maintain a set speed
- **ACC Mode**: Automatically adjusts speed based on obstacle distance

---

## Components Required

| Component                 | Description                        |
|--------------------------|------------------------------------|
| Arduino Uno              | Main controller                    |
| Ultrasonic Sensor (HC-SR04)| For obstacle detection (Trig: D10, Echo: D8) |
| Push Buttons (x5)        | Speed+, Speed–, Cancel, Set, ACC  |
| TT Motor with L298N      | Motor + PWM speed control          |
| LEDs (x2)                | Mode indicators (Normal, Cruise, ACC) |
| External Power (optional)| For driving motor                  |
| MATLAB (Simulink Optional)| For logic, sensor reading & control |

---

## Pin Mapping

| Function        | Arduino Pin |
|----------------|-------------|
| Motor In1      | D5          |
| Motor In2      | D6          |
| Motor Enable   | D9 (PWM)    |
| Trig (Ultrasonic) | D10     |
| Echo (Ultrasonic) | D8      |
| LED1           | D13         |
| LED2           | D12         |
| Button: Speed+ | A0          |
| Button: Speed– | A1          |
| Button: Cancel | A2          |
| Button: Set    | A3          |
| Button: ACC    | A4          |

---

##  How It Works

###  Normal Mode (Mode = 0)
- Activated by default or via **Cancel button**
- Manually increase/decrease speed using buttons
- Motor speed decays slowly when no input
- LED1 = OFF, LED2 = ON

###  Cruise Mode (Mode = 1)
- Activated using **Set button**
- Locks in current speed as **setpoint**
- Buttons change setpoint
- LED1 = ON, LED2 = OFF

###  Adaptive Cruise Control Mode (Mode = 2)
- Activated using **ACC button**
- Monitors distance using ultrasonic sensor
- Reduces speed if object too close (`< 20 cm`)
- Increases back to `constant` speed when safe
- Both LEDs ON when safe, **blinking** if too close

---

##  MATLAB Code Logic

- Uses `arduino()` object and `ultrasonic()` sensor
- Buttons read via `readVoltage()`
- PWM duty cycle scaled: `speed / 100`
- Motor direction set via digital pins
- Speed clamped to `[0, 100]`
- Distance dynamically controls speed in ACC mode

---

## 🖥 Code Snippet (MATLAB)

```matlab
if speed > 0
    writeDigitalPin(ar, motorIn1, 1);
    writeDigitalPin(ar, motorIn2, 0);
    writePWMDutyCycle(ar, motorEnable, speed / 100);
else
    writeDigitalPin(ar, motorIn1, 0);
    writeDigitalPin(ar, motorIn2, 0);
end

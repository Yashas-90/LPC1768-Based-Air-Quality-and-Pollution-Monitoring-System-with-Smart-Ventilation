# 🌫️ LPC1768-Based Air Quality and Pollution Monitoring System with Smart Ventilation

## 📌 Project Overview
This project is an **Air Quality and Pollution Monitoring System** using the **LPC1768 (ARM Cortex-M3)** microcontroller.  
It continuously monitors **gas pollution level** and **temperature**, displays the values on a **16x2 LCD**, and automatically controls a **12V fan (ventilation)** using a relay.

✅ Gas/Pollution Monitoring using MQ Sensor  
✅ Temperature Monitoring using LM35  
✅ Smart Fan Control using Relay  
✅ Alert Buzzer for High Pollution / High Temperature  
✅ Live Data Display on 16x2 LCD  

---

## 🎯 Objectives
- Detect air pollution levels in real time
- Automatically activate ventilation when pollution rises
- Provide safe environmental monitoring using buzzer alert
- Display live pollution and temperature readings on LCD

---

## 🔧 Hardware Components Used
- LPC1768 Development Board
- Gas Sensor (MQ Series – Example MQ-135)
- Temperature Sensor (LM35)
- 16x2 LCD Display (4-bit Mode)
- 12V Brushless DC Fan
- Relay Module (12V fan control)
- Buzzer
- Power Supply (5V for sensors + 12V for fan)
- Connecting wires / Breadboard

---

## 🔌 Pin Connections

### ✅ Gas Sensor (MQ Sensor)
| Sensor Pin | LPC1768 Connection |
|-----------|---------------------|
| VCC | 5V |
| GND | Common Ground |
| AO | P0.23 (ADC) |

---

### ✅ Temperature Sensor (LM35)
| Sensor Pin | LPC1768 Connection |
|-----------|---------------------|
| VCC | 3.3V |
| GND | Common Ground |
| OUT | P0.25 (ADC) |

---

### ✅ LCD Display (16x2, 4-bit Mode)
| LCD Pin | LPC1768 Pin |
|--------|-------------|
| RS | P0.10 |
| EN | P0.11 |
| D4 | P0.19 |
| D5 | P0.20 |
| D6 | P0.21 |
| D7 | P0.22 |
| VCC | 5V |
| GND | Common Ground |

---

### ✅ 12V Brushless DC Fan + Relay
| Relay / Fan | Connection |
|------------|------------|
| Relay COM | 12V |
| Relay NC | Fan (+ve) |
| Relay Control Pin | P1.28 |
| Fan (-ve) | Common Ground |

---

### ✅ Buzzer
| Buzzer Pin | LPC1768 Connection |
|-----------|---------------------|
| Control | P1.27 |
| GND | Common Ground |

---

## ⚙️ Working Principle
1. LPC1768 reads:
   - Gas level using ADC
   - Temperature using LM35 sensor ADC
2. Sensor values are displayed continuously on LCD.
3. If:
   - Gas level crosses threshold **OR**
   - Temperature crosses threshold  
   ✅ Fan turns ON (ventilation)
   ✅ Buzzer turns ON (alert)

---

## 🧠 Software Used
- **Keil µVision** (Embedded C programming)
- **Flash Magic** (HEX file upload)
- LPC17xx CMSIS Libraries

---

▶️ How to Run


Connect the sensors and LCD as per pin connections.


Compile the program in Keil µVision.


Generate HEX file.


Upload HEX to LPC1768 using Flash Magic.


Observe live sensor readings on LCD.


Fan and buzzer activate automatically when pollution/temperature increases.


✅ Output Display


LCD shows:


Temperature in °C


Gas/Pollution percentage (%)


Example:


T: 32.5C


Gas: 45.1%


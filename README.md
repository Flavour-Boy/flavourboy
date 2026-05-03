# Wiring Instructions

## Game Display — 20x4 LCD

| Display Pin | Connection |
|---|---|
| SCL | D15 |
| SDA | D14 |
| VCC | 5V |
| GND | GND |

---

## HUD Display — 16x2 LCD

| LCD Pin | Connection |
|---|---|
| Pin 1 | GND |
| Pin 2 | 5V |
| Pin 3 | 9.1kΩ resistor → GND |
| Pin 4 | D1 |
| Pin 5 | GND |
| Pin 6 | D2 |
| Pin 7 | Not connected |
| Pin 8 | Not connected |
| Pin 9 | Not connected |
| Pin 10 | Not connected |
| Pin 11 | D4 |
| Pin 12 | D5 |
| Pin 13 | D6 |
| Pin 14 | D7 |
| Pin 15 | 33Ω resistor → 5V |
| Pin 16 | GND |

---

## Speaker

| Speaker Lead | Connection |
|---|---|
| Positive | D3 |
| Negative | GND |

---

## Buttons

| Button | Connection |
|---|---|
| Up | D8 |
| Down | D9 |
| Shoot | D10 |
| Ability | D11 |
| Pause | D12 |

---

## Pin Summary

| Pin | Connected To |
|---|---|
| D1 | HUD LCD Pin 4 |
| D2 | HUD LCD Pin 6 |
| D3 | Speaker positive |
| D4 | HUD LCD Pin 11 |
| D5 | HUD LCD Pin 12 |
| D6 | HUD LCD Pin 13 |
| D7 | HUD LCD Pin 14 |
| D8 | Up button |
| D9 | Down button |
| D10 | Shoot button |
| D11 | Ability button |
| D12 | Pause button |
| D14 | Game display SDA |
| D15 | Game display SCL |
| 5V | Game display VCC, HUD LCD Pin 2, HUD LCD Pin 15 via resistor |
| GND | Game display GND, HUD LCD Pins 1, 5, and 16, speaker negative |

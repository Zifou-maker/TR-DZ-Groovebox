# 🔌 TR-DZ V11 Wiring Diagram

This document describes the hardware connections for the TR-DZ prototype using the **Daisy Seed v1.1** and the **CD74HC4067** (16-channel Multiplexer).

## 1. Multiplexer (CD74HC4067) Connections

The multiplexer allows us to read 16 potentiometers using only one analog pin on the Daisy Seed.

| MUX Pin | Daisy Seed Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 3V3 Digital (Pin 38) | Power |
| **GND** | DGND (Pin 40) | Ground |
| **SIG** | AGND (Pin 20) / A0 (Pin 22) | Analog Signal Output |
| **S0** | D0 (Pin 1) | Address Select 0 |
| **S1** | D1 (Pin 2) | Address Select 1 |
| **S2** | D2 (Pin 3) | Address Select 2 |
| **S3** | D3 (Pin 4) | Address Select 3 |
| **EN** | DGND (Pin 40) | Enable (Active Low) |

### Potentiometer Mapping (Channels)
- **C0:** BPM Control
- **C1:** Filter Cutoff
- **C2:** Swing Amount
- **C3:** Kick Decay

---

## 2. OLED Display (SH1106 I2C)

| OLED Pin | Daisy Seed Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 3V3 Digital (Pin 38) | Power |
| **GND** | DGND (Pin 40) | Ground |
| **SCL** | SCL (Pin 11) | I2C Clock |
| **SDA** | SDA (Pin 12) | I2C Data |

---

## 3. Interaction & Audio

| Component | Daisy Seed Pin | Note |
| :--- | :--- | :--- |
| **Step Button** | Pin 28 | Connect to GND when pressed |
| **Audio Out L** | Pin 18 | Left Channel Output |
| **Audio Out R** | Pin 19 | Right Channel Output |
| **Audio GND** | AGND (Pin 20) | Use for Audio shielding |

---

## 4. Visual Schematic (Simplified)

```text
      DAISY SEED                    CD74HC4067 (MUX)
    +-----------+                  +----------------+
    |        D0 |------------------| S0             |
    |        D1 |------------------| S1             |
    |        D2 |------------------| S2             |
    |        D3 |------------------| S3             |
    |        A0 |------------------| SIG            |
    |           |                  |                |
    |       SCL |-------+          |      C0 --- [POT BPM]
    |       SDA |-----+ |          |      C1 --- [POT CUT]
    +-----------+     | |          |      C2 --- [POT SWG]
                      | |          |      C3 --- [POT DCY]
      SH1106 OLED     | |          +----------------+
    +-----------+     | |
    |       SDA |-----+ |
    |       SCL |-------+
    +-----------+

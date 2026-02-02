
# 🎹 TR-DZ: The Ultimate Tech-House Kick & Bass Synthesizer
[![Démo TR-DZ sur YouTube](https://img.youtube.com/vi/K2HNfEA7Zsw/0.jpg)](https://www.youtube.com/shorts/K2HNfEA7Zsw)
> 📹 **Click on the image to see the demo (Sound enabled!)**

![Version](https://img.shields.io/badge/Version-V11-blue)
![Hardware](https://img.shields.io/badge/Platform-Daisy_Seed-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

**TR-DZ** is a high-performance hybrid drum machine and bass synthesizer built on the **Daisy Seed (STM32H7)** platform. It is specifically engineered for the "Zero Friction" creation of the fundamental Tech-House couple: the **Kick** and the **Bass**.

> "The music is not in the menu. It's in the interaction." — TR-DZ Manifesto.

---

## 🔥 Key Features

- **Native Sidechain (Ducking):** A real-time DSP link where the Kick envelope dynamically carves space in the Bass signal. No external compression needed.
- **Hybrid DSP Engine:** - **Kick:** Analog-modeled sine oscillator with ultra-fast exponential pitch envelopes.
    - **Bass:** Rich polyblep oscillators with resonant filters.
    - **Drums:** Dedicated Snare and Hi-Hat synthesis engines.
- **Hardware Agnostic UI:** Optimized for a **CD74HC4067** 16-channel multiplexer and **SH1106** OLED display.
- **Performance Sequencer:** 16-step grid with real-time swing and parameter control.

---

## 🛠 Hardware Specifications

| Component | Specification |
| :--- | :--- |
| **MCU** | Daisy Seed v1.1 (Cortex-M7) |
| **Audio** | 24-bit / 48kHz Stereo |
| **Multiplexer** | CD74HC4067 (up to 16 potentiometers) |
| **Display** | OLED SH1106 (I2C) |
| **Inputs** | 4-knob core (BPM, Cutoff, Swing, Kick Decay) + Step Button |

---

## 📂 Project Structure

- `src/`: Core C++ firmware (libdaisy / daisysp).
- `TR-DZ_V11.ino`: The main integrated engine.
- `Schematics/`: [Full wiring and pinout documentation](./Schematics/Wiring.md)
- `SPECIFICATIONS.md`: Deep dive into the AI generative roadmap and DSP logic.

---

## 🚀 Quick Start

1. **Hardware Setup:** Connect your CD74HC4067 multiplexer to pins D0-D3 and A0.
2. **Library Installation:** Ensure you have the [DaisyDuino](https://github.com/electro-smith/DaisyDuino) library in your Arduino IDE.
3. **Flash:** Upload `TR-DZ_V11.ino` to your Daisy Seed.
4. **Play:** Start tweaking the pots to feel the native sidechain pumping.

---

## 📋 Roadmap

- [x] **V11:** Functional Kick/Bass engine with Native Sidechain.
- [ ] **V12:** Hysteresis implementation for stable potentiometer reading.
- [ ] **V13:** SD Card integration for sample-based percussion.
- [ ] **Future:** AI-Assisted pattern mutation (VAE models).

---

## 🤝 Contribution & License

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

Distributed under the **MIT License**.

**Developer:** [Zifou-maker](https://github.com/Zifou-maker)

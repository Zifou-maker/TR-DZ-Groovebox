# 🥁 TR-DZ : Daisy Seed Tech House Drum Machine
![Prototype TR-DZ](tr-dz-prototype.jpg)
![Version](https://img.shields.io/badge/firmware-v8.0-blue) ![Platform](https://img.shields.io/badge/platform-Daisy%20Seed-green) ![License](https://img.shields.io/badge/license-MIT-lightgrey)

**TR-DZ** est un projet de performance dédié à la création de Kicks Tech House percutants et organique, basé sur le microcontrôleur **Daisy Seed**. Conçue pour la **Tech House**, elle génère des percussions synthétiques en temps réel avec un moteur audio 48kHz / 24-bit. Elle combine une synthèse analogique modélisée (DSP) avec une interface de contrôle avancée via multiplexage.
---

## 🚀 Fonctionnalités Actuelles (Firmware V8.0)

### Moteur Sonore (Synthesis Engine)
* **Kick :** Oscillateur Sinusoïdal + Enveloppe de Pitch (Punch) + Enveloppe d'Amplitude.
* **Snare / Clap :** Mix hybride (Oscillateur Triangle + Bruit filtré passe-bande) sur les temps 2 et 4 (Backbeat).
* **Hi-Hat :** Bruit blanc filtré passe-haut, joué automatiquement sur les contre-temps (Offbeat).
* **Master Effects :**
    * **Filtre SVF (State Variable Filter) :** Filtre passe-bas résonant (contrôlable sur le Kick).
    * **Soft Clipper :** Limiteur en sortie pour éviter la saturation numérique.

### Interface & Séquenceur
* **Séquenceur 16 pas :**
    * Kick : Séquençable manuellement (Toggle).
    * Snare & Hat : Motifs automatiques (Auto-generated patterns).
* **Bouton "Intelligent" :**
    * *Appui Court :* Active/Désactive un pas de Kick.
    * *Appui Long (1s) :* Efface toute la séquence de Kick ("Clear").
* **Écran OLED :**
    * Visualisation en temps réel (BPM, Fréquence de coupure).
    * Grille de séquenceur avec symboles distincts (■ = Kick, x = Snare, • = Hat).
    * VU-mètre stéréo dynamique.

---

## 🛠 Matériel Requis (Hardware)

| Composant | Quantité | Notes |
| :--- | :--- | :--- |
| **Daisy Seed v1.1** | 1 | Le cerveau du projet |
| **Écran OLED 1.3"** | 1 | Driver **SH1106** (I2C) |
| **Bouton Poussoir** | 1 | Momentané (Tactile Switch 12x12mm) |
| **Potentiomètres** | 2 | 10k Linéaire (B10K) |
| **Breadboard** | 1 | Plaque d'essai |
| **Fils de connexion** | - | Jumper wires |
| **Sortie Audio** | 1 | Jack 6.35mm ou Jack 3.5mm stéréo |

*(À venir dans la V9.0 : Intégration de puces 74HC165 pour 16 boutons physiques)*

---

## 🔌 Plan de Câblage (Wiring)

### Schéma Logique
```mermaid
graph TD
    subgraph DAISY [Daisy Seed v1.1]
        P12[Pin 12 - SCL]
        P13[Pin 13 - SDA]
        P18[Pin 18 - Audio OUT L]
        P19[Pin 19 - Audio OUT R]
        P22[Pin 22 - ADC0]
        P23[Pin 23 - ADC1]
        P35[Pin 35 - D28]
        DGND[Pin 40 - DGND]
        V33[Pin 38 - 3V3 Out]
    end

    subgraph SCREEN [Écran OLED SH1106]
        O_VCC[VCC]
        O_GND[GND]
        O_SCL[SCL]
        O_SDA[SDA]
    end

    subgraph CONTROLS [Contrôles]
        POT1[Potentiomètre BPM]
        POT2[Potentiomètre Filtre]
        BTN1[Bouton Kick]
    end

    %% Connexions
    V33 --> O_VCC & POT1 & POT2
    DGND --> O_GND & POT1 & POT2 & BTN1
    P12 --> O_SCL
    P13 --> O_SDA
    P22 --> POT1
    P23 --> POT2
    P35 --> BTN1

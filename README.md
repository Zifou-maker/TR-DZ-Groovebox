code Markdown
downloadcontent_copy
expand_less
# TR-DZ : The Ultimate Tech House Kick Synthesizer
**Powered by Daisy Seed (Electro-Smith)**

TR-DZ est un instrument de performance dédié à la création de Kicks Tech House percutants et organiques. Conçu sur la plateforme Daisy Seed, il combine une synthèse analogique modélisée (DSP) avec une interface de contrôle avancée via multiplexage.

## ✨ Points Forts (Features)
*   **Dual-Envelope Engine :** Gestion indépendante de l'enveloppe d'amplitude (le corps) et de l'enveloppe de pitch (le "punch").
*   **Rumble Generator :** Algorithme DSP interne de reverb + distorsion pour le sub-bass techno, contrôlable par un seul potentiomètre.
*   **K/B Lock System :** Architecture logicielle permettant de verrouiller le Kick et la Basse tout en changeant les kits de percussions à la volée.
*   **Interface Hybride :** 2 écrans OLED (Waveform Scope + Mixer View) et 12 Faders physiques.

## 🛠️ Spécifications Techniques
**Hardware**
*   **Cœur :** Daisy Seed (STM32H7) - 480MHz / 24-bit audio.
*   **Multiplexage :** CD4051 (Expansion pour 36 potentiomètres).
*   **Alimentation :** Module MB102 filtré (3.3V Logic) pour réduction de bruit.
*   **Audio :** Sortie Stéréo Neutrik + Cue Output.

**Signal Flow (DSP)**
1.  **Oscillateur :** Waveform Triangle/Sinus à fréquence glissante.
2.  **Impact :** Pitch Envelope ultrarapide (50ms).
3.  **Traitement :** Saturation Soft-clipping -> Filtre Résonant -> VCA -> Rumble Send.

## 🚀 Installation & Build
1.  **Prérequis :** VS Code avec PlatformIO ou Arduino IDE (DaisyDuino).
2.  **Bibliothèques :** `DaisySP`, `U8g2` (pour OLED).
3.  **Flash :** Connectez la Daisy en mode DFU et téléversez `TR-DZ_Firmware_v0.1`.

## 🤝 Contribution
Projet Open Source. Les Pull Requests pour l'optimisation des algorithmes de "Generative Sequencing" sont les bienvenues.

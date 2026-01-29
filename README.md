TR-DZ : The Ultimate Tech House Kick Synthesizer
Powered by Daisy Seed (Electro-Smith)
TR-DZ est un instrument de performance dédié à la création de Kicks Tech House percutants et organiques. Conçu sur la plateforme Daisy Seed, il combine une synthèse analogique modélisée (DSP) avec une interface de contrôle avancée via multiplexage.

✨ Points Forts (Features)
Dual-Envelope Engine : Gestion indépendante de l'enveloppe d'amplitude (le corps) et de l'enveloppe de pitch (le "punch").
Analog-Style Saturation : Algorithme de saturation non-linéaire basé sur la fonction arc-tangente ($\arctan$) pour une chaleur harmonique authentique.
Ghost Mode : Algorithme de variation organique introduisant des micro-différences subtiles sur chaque coup pour éviter la monotonie.
Interface 16-Paramètres : Contrôle total de l'ADN du kick via multiplexeur CD4051.
OLED UI : Retour visuel en temps réel sur écran SH1106.

🛠️ Spécifications Techniques
Hardware
Cœur : Daisy Seed (STM32H7) - 480MHz / 24-bit audio.
Multiplexage : CD4051 (Expansion 8 à 16 potentiomètres).
Affichage : OLED 1.3" I2C (SH1106).
Alimentation : Rail filtré via AGND pour un silence radio total.
Signal Flow (DSP)
Oscillateur : Waveform Triangle/Sinus à fréquence glissante.
Impact : Pitch Envelope ultrarapide (50ms) pour le claquement transitoire.
Traitement : Saturation Soft-clipping $\rightarrow$ Filtre Résonant $\rightarrow$ VCA.

🚀 Installation & Build
Prérequis : Arduino IDE avec le support DaisyDuino.
Câblage : Consultez le dossier /schematics pour les connexions du CD4051.
Flash : Téléversez TR-DZ_V5.1.ino sur votre Daisy Seed.

🗺️ Roadmap
[x] Stabilisation des entrées analogiques (Hystérésis/Lissage).
[x] Moteur audio Dual-Envelope.
[ ] Implémentation du Séquençage Euclidien.
[ ] Sauvegarde de "Snapshots" (Presets).
[ ] Conception du boîtier en aluminium brossé.

🤝 Contribution
Les idées de design sonore et d'optimisation DSP sont les bienvenues. N'hésitez pas à ouvrir une Issue ou à proposer une Pull Request.
Développé par D.ZIF


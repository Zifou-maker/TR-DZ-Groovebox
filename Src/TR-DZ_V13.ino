/* -------------------------------------------------------
   TR-DZ V13 - UPDATED EDITION
   -------------------------------------------------------
   Hardware: Daisy Seed v1.1 + CD74HC4067 Mux + OLED SH1106
   
   NEW FEATURES IN V13:
   - DSP: Optimized Cubic SoftClip (CPU efficient saturation).
   - MIX: Quadratic Sidechain (Daft Punk style "pumping").
   - SEQ: Procedural Rhythm Generator (Long Press to randomize).
   - UX: Inverted color playhead on OLED.

   POTENTIOMETER MAPPING (MUX 16 Channels):
   - C0: Global BPM
   - C1: Kick Decay
   - C2: Filter Cutoff (Master + Bass)
   - C3: Bass Resonance (Acid factor)
   - C4: Snare Tone (Osc/Noise balance)
   - C5: Hi-Hat Volume
   - C6: Swing Amount
   - C7: Master Drive / Distortion
   -------------------------------------------------------
*/

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <Wire.h>

using namespace daisysp;

// --- DISPLAY CONFIGURATION ---
// Using SH1106 (Common 1.3" OLED). Change to U8G2_SSD1306... if using 0.96" screen.
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// --- PINS DEFINITION ---
// ATTENTION: Pins moved to D3-D6 to avoid conflict with USB Serial (D0/D1)
const int MUX_S0 = 3;
const int MUX_S1 = 4;
const int MUX_S2 = 5;
const int MUX_S3 = 6;
const int MUX_SIG = A0; // ADC Input

#define PIN_STEP_BUTTON 28

static DaisyHardware hw;
static Metro         metro;

// --- AUDIO MODULES ---
static Oscillator    osc_kick, osc_bass, osc_snare;
static WhiteNoise    noise_hat, noise_snare;
static AdEnv         env_amp_kick, env_pitch_kick, env_hat, env_snare_tone, env_snare_noise, env_bass;
static Svf           filt_kick, filt_hat, filt_snare, filt_bass;

// --- SYSTEM VARIABLES ---
// Hysteresis for pot stability
float last_mux_values[16]; 
const float HYSTERESIS_THRESHOLD = 0.008f; 

// Parameters
volatile float p_bpm = 120.0f;
volatile float p_kick_decay = 0.4f;
volatile float p_cutoff = 1000.0f;
volatile float p_bass_res = 0.0f;
volatile float p_snare_tone = 0.5f;
volatile float p_hat_vol = 0.5f;
volatile float p_swing = 0.0f;
volatile float p_drive = 0.0f;

volatile int   current_step = 0;
volatile float peak_level = 0.0f;

// Sequencer Data
bool seq_kick[16]  = {true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false};
bool seq_hat[16]   = {false, false, true, false, false, false, true, false, false, false, true, false, false, false, true, false};
bool seq_snare[16] = {false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false};
int  seq_bass[16]  = {0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 1, 2, 0};

// Button Logic
unsigned long button_timer = 0;
bool last_button_state = HIGH;
bool is_long_press = false;
bool show_popup = false;
String popup_msg = "";

// --- HELPER: STABLE MUX READING ---
float ReadMuxStable(int channel) {
    digitalWrite(MUX_S0, (channel & 1));
    digitalWrite(MUX_S1, (channel & 2));
    digitalWrite(MUX_S2, (channel & 4));
    digitalWrite(MUX_S3, (channel & 8));
    delayMicroseconds(5); // Stabilization time
    
    float raw = analogRead(MUX_SIG) / 1023.0f;
    
    // Hysteresis logic to prevent jitter
    if (abs(raw - last_mux_values[channel]) > HYSTERESIS_THRESHOLD) {
        last_mux_values[channel] = raw;
    }
    return last_mux_values[channel];
}

// --- HELPER: PROCEDURAL PATTERN GENERATOR ---
void RandomizePattern() {
    for(int i=0; i<16; i++) {
        // Clear everything first
        seq_kick[i] = false; seq_snare[i] = false; seq_hat[i] = false; seq_bass[i] = 0;

        // Kick Logic: High probability on downbeats (0, 4, 8, 12)
        if(i % 4 == 0) { if(random(100) < 90) seq_kick[i] = true; }
        else { if(random(100) < 15) seq_kick[i] = true; } // Low chance off-beat

        // Snare Logic: Backbeat focus (4, 12)
        if(i == 4 || i == 12) { seq_snare[i] = true; }
        else { if(random(100) < 10) seq_snare[i] = true; } // Ghost notes

        // Hat Logic: Offbeats (2, 6, 10, 14) + Random
        if(i % 2 != 0) seq_hat[i] = true;
        else { if(random(100) < 30) seq_hat[i] = true; }

        // Bass Logic: Avoid Kicks
        if(!seq_kick[i] && random(100) < 60) {
            seq_bass[i] = (random(100) < 70) ? 1 : 2; // Note 1 or 2
        }
    }
}

// --- AUDIO CALLBACK ---
void AudioCallback(float **in, float **out, size_t size) {
    float base_freq = (p_bpm / 60.0f) * 4.0f;
    
    // 1. Swing Handling
    if (current_step % 2 != 0) metro.SetFreq(base_freq * (1.0f - p_swing));
    else metro.SetFreq(base_freq * (1.0f + p_swing));

    // 2. Update Audio Parameters
    env_amp_kick.SetTime(ADENV_SEG_DECAY, 0.1f + (p_kick_decay * 1.5f));
    
    filt_kick.SetFreq(p_cutoff);
    // Bass filter follows master cutoff but scaled
    float bass_cut = p_cutoff * 0.6f; 
    
    float temp_peak = 0.0f;

    for (size_t i = 0; i < size; i++) {
        // --- SEQUENCER TRIGGER ---
        if (metro.Process()) {
            current_step = (current_step + 1) % 16;
            
            if (seq_kick[current_step]) { osc_kick.Reset(); env_amp_kick.Trigger(); env_pitch_kick.Trigger(); }
            if (seq_hat[current_step])  { env_hat.Trigger(); }
            if (seq_snare[current_step]){ env_snare_tone.Trigger(); env_snare_noise.Trigger(); }
            if (seq_bass[current_step] > 0) {
                env_bass.Trigger();
                osc_bass.SetFreq((seq_bass[current_step] == 1) ? 55.0f : 110.0f);
            }
        }

        // --- SYNTHESIS ---

        // KICK (Sine + Pitch Env)
        osc_kick.SetFreq(40.0f + (env_pitch_kick.Process() * 180.0f));
        float s_kick = osc_kick.Process() * env_amp_kick.Process();
        filt_kick.Process(s_kick); s_kick = filt_kick.Low();

        // HAT (Filtered Noise)
        float s_hat = noise_hat.Process() * env_hat.Process() * p_hat_vol;
        filt_hat.Process(s_hat); s_hat = filt_hat.High();

        // SNARE (Tone + Noise Blend)
        osc_snare.SetFreq(180.0f + (env_snare_tone.Process() * 60.0f));
        float s_snare_t = osc_snare.Process() * env_snare_tone.Process();
        float s_snare_n = noise_snare.Process() * env_snare_noise.Process();
        filt_snare.Process(s_snare_n);
        // Blend based on p_snare_tone (C4)
        float s_snare = (s_snare_t * (1.0f - p_snare_tone)) + (filt_snare.Band() * p_snare_tone);

        // BASS (Sawtooth + Resonant Filter)
        float env_b = env_bass.Process();
        filt_bass.SetFreq(bass_cut + (env_b * 600.0f));
        filt_bass.SetRes(p_bass_res * 0.9f); // C3 controls resonance
        filt_bass.Process(osc_bass.Process());
        float s_bass = filt_bass.Low() * env_b;

        // --- MIXING & FX ---
        
        // Quadratic Sidechain (Daft Punk style pumping)
        // More aggressive than linear sidechain
        float kick_env_val = env_amp_kick.GetValue();
        float duck_raw = 1.0f - kick_env_val;
        if(duck_raw < 0) duck_raw = 0;
        float sidechain_factor = duck_raw * duck_raw; // Quadratic curve
        
        s_bass *= sidechain_factor; 

        // Summing
        float mixed = s_kick + (s_bass * 0.6f) + (s_snare * 0.5f) + (s_hat * 0.3f);

        // Output Drive (Soft Clip)
        // Cubic approximation of tanh: x - x^3/3 (Efficient)
        float drive_amt = 1.0f + (p_drive * 2.0f); // 1.0 to 3.0 gain
        float driven = mixed * drive_amt;
        
        // Soft clipper
        if (driven < -1.5f) driven = -1.5f;
        else if (driven > 1.5f) driven = 1.5f;
        driven = driven - (driven * driven * driven) / 27.0f; // Soft curve

        if (abs(driven) > temp_peak) temp_peak = abs(driven);
        out[0][i] = out[1][i] = driven * 0.7f; // Master volume comp
    }
    peak_level = temp_peak;
}

// --- SETUP ---
void setup() {
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    float sr = DAISY.get_samplerate();
    Wire.setClock(400000); u8g2.begin();

    // Mux Pins Setup
    pinMode(MUX_S0, OUTPUT); pinMode(MUX_S1, OUTPUT); 
    pinMode(MUX_S2, OUTPUT); pinMode(MUX_S3, OUTPUT);
    pinMode(PIN_STEP_BUTTON, INPUT_PULLUP);
    
    // Random Seed
    randomSeed(analogRead(A1));

    // Audio Init
    // Kick
    osc_kick.Init(sr); osc_kick.SetWaveform(Oscillator::WAVE_SIN);
    env_amp_kick.Init(sr); env_pitch_kick.Init(sr); env_pitch_kick.SetTime(ADENV_SEG_DECAY, 0.05f);
    filt_kick.Init(sr); filt_kick.SetRes(0.5f);
    
    // Hat
    noise_hat.Init(); env_hat.Init(sr); env_hat.SetTime(ADENV_SEG_DECAY, 0.05f);
    filt_hat.Init(sr); filt_hat.SetFreq(8000.0f); filt_hat.SetRes(0.5f);

    // Snare
    osc_snare.Init(sr); osc_snare.SetWaveform(Oscillator::WAVE_TRI);
    noise_snare.Init(); env_snare_tone.Init(sr); env_snare_tone.SetTime(ADENV_SEG_DECAY, 0.15f);
    env_snare_noise.Init(sr); env_snare_noise.SetTime(ADENV_SEG_DECAY, 0.20f);
    filt_snare.Init(sr); filt_snare.SetRes(0.4f); filt_snare.SetFreq(1500.0f);

    // Bass (Changed to Saw for more "bite")
    osc_bass.Init(sr); osc_bass.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    env_bass.Init(sr); env_bass.SetTime(ADENV_SEG_DECAY, 0.2f);
    filt_bass.Init(sr); 

    metro.Init(2.0f, sr);
    DAISY.begin(AudioCallback);
}

// --- MAIN LOOP ---
void loop() {
    // 1. Scan Mux (Only first 8 channels used for now)
    p_bpm        = 60.0f + (ReadMuxStable(0) * 140.0f);
    p_kick_decay = ReadMuxStable(1);
    p_cutoff     = 50.0f + (ReadMuxStable(2) * 10000.0f);
    p_bass_res   = ReadMuxStable(3);
    p_snare_tone = ReadMuxStable(4);
    p_hat_vol    = ReadMuxStable(5);
    p_swing      = ReadMuxStable(6) * 0.25f;
    p_drive      = ReadMuxStable(7);

    // 2. Button Handling
    bool btn = digitalRead(PIN_STEP_BUTTON);
    
    // Press
    if (btn == LOW && last_button_state == HIGH) { 
        button_timer = millis(); 
        is_long_press = false; 
    }
    
    // Long Press -> RANDOMIZE
    if (btn == LOW && (millis() - button_timer > 1000) && !is_long_press) {
        RandomizePattern();
        is_long_press = true;
        show_popup = true;
        popup_msg = "* RANDOMIZED *";
    }
    
    // Release (Short Press) -> TOGGLE KICK
    if (btn == HIGH && last_button_state == LOW && !is_long_press) {
        seq_kick[current_step] = !seq_kick[current_step];
    }
    last_button_state = btn;
    
    // Popup timer
    if(show_popup && (millis() - button_timer > 2000)) show_popup = false;

    // 3. UI Draw
    u8g2.clearBuffer();
    
    // Top Bar
    u8g2.setFont(u8g2_font_micro_tr); 
    u8g2.setCursor(0, 6);   u8g2.print("BPM:"); u8g2.print((int)p_bpm);
    u8g2.setCursor(40, 6);  u8g2.print("DRV:"); u8g2.print((int)(p_drive*100));
    u8g2.setCursor(80, 6);  u8g2.print("SWG:"); u8g2.print((int)(p_swing*100));
    
    // VU Meter (Right side)
    int vu_h = (int)(peak_level * 60.0f);
    u8g2.drawBox(124, 64-vu_h, 4, vu_h);

    // Sequencer Grid
    int y_pos = 20;
    for(int i=0; i<16; i++) {
        int x = i * 7 + 2; // Spacing
        
        // Active Step Highlighting (Inverted Color)
        if(i == current_step) {
            u8g2.setDrawColor(1);
            u8g2.drawBox(x-1, y_pos-2, 8, 30);
            u8g2.setDrawColor(0); // Draw Black on White
        } else {
            u8g2.setDrawColor(1); // Draw White on Black
            u8g2.drawFrame(x, y_pos, 6, 26);
        }

        // Draw Instruments
        // Kick (Bottom square)
        if(seq_kick[i]) u8g2.drawBox(x+1, y_pos+18, 4, 4);
        // Snare (Middle line)
        if(seq_snare[i]) u8g2.drawHLine(x+1, y_pos+12, 4);
        // Hat (Top dot)
        if(seq_hat[i]) u8g2.drawPixel(x+2, y_pos+4);
        // Bass (Vertical line on side)
        if(seq_bass[i] > 0) u8g2.drawVLine(x+5, y_pos+2, 20);
    }
    
    // Reset Color
    u8g2.setDrawColor(1);

    // Popup Overlay
    if(show_popup) {
        u8g2.setDrawColor(0);
        u8g2.drawBox(10, 25, 100, 20);
        u8g2.setDrawColor(1);
        u8g2.drawFrame(10, 25, 100, 20);
        u8g2.setFont(u8g2_font_6x12_tr);
        u8g2.setCursor(20, 38);
        u8g2.print(popup_msg);
    }

    u8g2.sendBuffer();
}

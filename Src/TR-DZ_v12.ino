/* -------------------------------------------------------
   TR-DZ V12 - HYSTERESIS & STABILITY EDITION
   -------------------------------------------------------
   Version: 1.2.0 (English)
   Goal: Eliminate "jitter" using a threshold algorithm.
   -------------------------------------------------------
*/

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <Wire.h>

using namespace daisysp;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// MUX & PINS
const int MUX_S0 = 0, MUX_S1 = 1, MUX_S2 = 2, MUX_S3 = 3, MUX_SIG = A0;
#define PIN_STEP_BUTTON 28

static DaisyHardware hw;
static Metro         metro;

// AUDIO MODULES (Same as V11)
static Oscillator osc_kick, osc_bass, osc_snare;
static WhiteNoise noise_hat, noise_snare;
static AdEnv env_amp_kick, env_pitch_kick, env_hat, env_snare_tone, env_snare_noise, env_bass;
static Svf filt_kick, filt_hat, filt_snare, filt_bass;

// GLOBAL VARIABLES
volatile float current_bpm = 120.0f, current_cutoff = 1000.0f;
volatile float current_swing = 0.05f, current_kick_decay = 0.4f;
volatile int current_step = 0;
volatile float peak_level = 0.0f;

// --- HYSTERESIS VARIABLES ---
float last_mux_values[16] = {0.0f};
const float HYSTERESIS_THRESHOLD = 0.008f; // 0.8% sensitivity threshold

// Sequences
bool seq_kick[16] = {true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false};
int seq_bass[16] = {0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 1, 2, 0};

float readMux(int channel) {
    digitalWrite(MUX_S0, (channel & 1) ? HIGH : LOW);
    digitalWrite(MUX_S1, (channel & 2) ? HIGH : LOW);
    digitalWrite(MUX_S2, (channel & 4) ? HIGH : LOW);
    digitalWrite(MUX_S3, (channel & 8) ? HIGH : LOW);
    delayMicroseconds(20);
    float newVal = analogRead(MUX_SIG) / 1023.0f;

    // HYSTERESIS LOGIC: Only update if change > threshold
    if (abs(newVal - last_mux_values[channel]) > HYSTERESIS_THRESHOLD) {
        last_mux_values[channel] = newVal;
        return newVal;
    }
    return last_mux_values[channel];
}

void MyCallback(float **in, float **out, size_t size) {
    // DSP Logic remains similar to V11 but with cleaner inputs
    float base_freq = (current_bpm / 60.0f) * 4.0f;
    metro.SetFreq(base_freq * (current_step % 2 != 0 ? (1.0f - current_swing) : (1.0f + current_swing)));
    
    filt_kick.SetFreq(current_cutoff);
    env_amp_kick.SetTime(ADENV_SEG_DECAY, current_kick_decay);

    for (size_t i = 0; i < size; i++) {
        if (metro.Process()) {
            current_step = (current_step + 1) % 16;
            if (seq_kick[current_step]) { env_amp_kick.Trigger(); env_pitch_kick.Trigger(); }
            if (seq_bass[current_step] > 0) {
                env_bass.Trigger();
                osc_bass.SetFreq(seq_bass[current_step] == 1 ? 55.0f : 110.0f);
            }
        }

        float kick_env = env_amp_kick.Process();
        osc_kick.SetFreq(40.0f + (env_pitch_kick.Process() * 160.0f));
        float sig_kick = osc_kick.Process() * kick_env;
        filt_kick.Process(sig_kick);
        sig_kick = filt_kick.Low();

        // NATIVE SIDECHAIN
        float ducking = 1.0f - (kick_env * 0.95f);
        float sig_bass = (filt_bass.Low() * env_bass.Process()) * (ducking < 0 ? 0 : ducking);

        float final_mix = sig_kick + (sig_bass * 0.6f);
        out[0][i] = out[1][i] = final_mix;
    }
}

void setup() {
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    u8g2.begin();
    pinMode(MUX_S0, OUTPUT); pinMode(MUX_S1, OUTPUT); 
    pinMode(MUX_S2, OUTPUT); pinMode(MUX_S3, OUTPUT);
    
    // Audio Init (Kick/Bass)
    osc_kick.Init(DAISY.get_samplerate()); osc_kick.SetWaveform(Oscillator::WAVE_SIN);
    env_amp_kick.Init(DAISY.get_samplerate());
    osc_bass.Init(DAISY.get_samplerate()); osc_bass.SetWaveform(Oscillator::WAVE_POLYBLEP_SQUARE);
    env_bass.Init(DAISY.get_samplerate());

    metro.Init(2.0f, DAISY.get_samplerate());
    DAISY.begin(MyCallback);
}

void loop() {
    // Read and assign with Hysteresis
    current_bpm = 60.0f + (readMux(0) * 140.0f);
    current_cutoff = 20.0f + (readMux(1) * 10000.0f);
    current_swing = readMux(2) * 0.25f;
    current_kick_decay = 0.05f + (readMux(3) * 0.8f);

    // UI Rendering
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(30, 10, "TR-DZ V12 STABLE");
    u8g2.setCursor(10, 25); u8g2.print("BPM:"); u8g2.print((int)current_bpm);
    u8g2.sendBuffer();
}

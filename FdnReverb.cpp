#include "FdnReverb.h"
#include <math.h>
#include <string.h>   // memset
#if !(defined(__arm__) || defined(__aarch64__))
  #include <xmmintrin.h>   // x86 flush-to-zero
#endif

using namespace FDNReverb;

// ---------------------------------------------------------------------------
//  init / setup
// ---------------------------------------------------------------------------
void FdnReverb::init(float sampleRate)
{
    Fs = sampleRate;

    // Param smoothing: ~10 ms time constant. One-pole toward target each sample.
    pSmooth = 1.0f - expf(-1.0f / (0.010f * Fs));

    // Base delay times in ms, spread and roughly mutually prime so modes don't
    // pile up. Scaled by `size` at run time.
    const float ms[NUM_LINES] = {43.5f, 37.1f, 29.7f, 23.3f};
    for (int i = 0; i < NUM_LINES; ++i)
        baseDelay[i] = ms[i] * 0.001f * Fs;

    // Clear buffers / state.
    memset(line, 0, sizeof(line));
    memset(ap,   0, sizeof(ap));
    memset(lpState, 0, sizeof(lpState));
    writeIdx = 0;

    // Sine LUT (one period) and Hann window LUT.
    for (int n = 0; n < SIN_LEN; ++n)
        sinTab[n] = sinf(2.0f * (float)M_PI * (float)n / (float)SIN_LEN);
    for (int n = 0; n < HANN_LEN; ++n)
        hannTab[n] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * (float)n /
                                         (float)(HANN_LEN - 1)));

    // Grain layers: spread the four phases evenly (75% overlap) so the windows
    // sum to ~constant; give each a random starting target.
    rngState = 0x1234567u;
    for (int i = 0; i < NUM_LINES; ++i)
        for (int k = 0; k < NUM_GRAIN; ++k) {
            grainPhase[i][k]  = (float)k / (float)NUM_GRAIN;
            grainTarget[i][k] = rndBipolar();
        }

    // Default control-derived increments.
    setSineRate(0.7f);  setSineDepth(2.0f);
    setGrainRate(8.0f); setGrainDepth(0.0f);
}

void FdnReverb::enableFTZ()
{
#if defined(__arm__)
    // Cortex-M7: set FZ (bit 24) in FPSCR so denormals flush to zero.
    uint32_t fpscr;
    __asm__ volatile("vmrs %0, fpscr" : "=r"(fpscr));
    fpscr |= (1u << 24);
    __asm__ volatile("vmsr fpscr, %0" : : "r"(fpscr));
#elif !defined(__aarch64__)
    // x86 (MSVC / MinGW / clang): flush-to-zero.
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
}

// ---------------------------------------------------------------------------
//  parameter setters (store targets; smoothing happens in process())
// ---------------------------------------------------------------------------
void FdnReverb::setSize(float s)      { if (s < 0.05f) s = 0.05f; if (s > 2.5f) s = 2.5f; sizeTgt = s; }
void FdnReverb::setDecay(float d)     { if (d < 0.0f) d = 0.0f;  if (d > 1.0f) d = 1.0f;  decayTgt = d; }
void FdnReverb::setDamping(float d)   { if (d < 0.0f) d = 0.0f;  if (d > 1.0f) d = 1.0f;
                                        dampTgt = 1.0f - 0.97f * d; }          // ->coef
void FdnReverb::setDiffusion(float d) { if (d < 0.0f) d = 0.0f;  if (d > 0.8f) d = 0.8f;  diffTgt = d; }
void FdnReverb::setTilt(float t)      { if (t < -1.0f) t = -1.0f; if (t > 1.0f) t = 1.0f; tiltTgt = t; }
void FdnReverb::setMix(float m)       { if (m < 0.0f) m = 0.0f;  if (m > 1.0f) m = 1.0f;  mixTgt = m; }

void FdnReverb::setSineRate(float hz)   { sineRateInc = hz / Fs; }
void FdnReverb::setSineDepth(float ms)  { sineDepthS  = ms * 0.001f * Fs; }
void FdnReverb::setGrainRate(float hz)  { grainRateInc = hz / Fs; }
void FdnReverb::setGrainDepth(float ms) { grainDepthS  = ms * 0.001f * Fs; }

// ---------------------------------------------------------------------------
//  input diffusion: 4 series allpass with fixed integer delays
// ---------------------------------------------------------------------------
float FdnReverb::inputDiffuse(float x)
{
    for (int i = 0; i < NUM_LINES; ++i) {
        int   rd  = (apIdx[i] - apLen[i] + AP_SIZE) & AP_MASK;
        float old = ap[i][rd];
        float v   = x + old * diffusion;
        ap[i][apIdx[i]] = v;
        x = old - v * diffusion;
        apIdx[i] = (apIdx[i] + 1) & AP_MASK;
    }
    return x;
}

// ---------------------------------------------------------------------------
//  per-sample engine
// ---------------------------------------------------------------------------
void FdnReverb::process(float in)
{
    // 1) smooth all parameters (one-pole toward target)
    size      += (sizeTgt  - size)      * pSmooth;
    decay     += (decayTgt - decay)     * pSmooth;
    dampCoef  += (dampTgt  - dampCoef)  * pSmooth;
    diffusion += (diffTgt  - diffusion) * pSmooth;
    tilt      += (tiltTgt  - tilt)      * pSmooth;
    mix       += (mixTgt   - mix)       * pSmooth;

    // 2) input diffusion
    float xin = inputDiffuse(in);

    // 3) read the four delay lines at their modulated, size-scaled delays
    float s[NUM_LINES];
    for (int i = 0; i < NUM_LINES; ++i) {
        // --- sine modulation (smooth periodic) ---
        sinePhase[i] += sineRateInc;
        if (sinePhase[i] >= 1.0f) sinePhase[i] -= 1.0f;
        float sineMod = sinLUT(sinePhase[i]) * sineDepthS;

        // --- Hann grain modulation (random, no Doppler) ---
        float wsum = 0.0f, dsum = 0.0f;
        for (int k = 0; k < NUM_GRAIN; ++k) {
            grainPhase[i][k] += grainRateInc;
            if (grainPhase[i][k] >= 1.0f) {
                grainPhase[i][k] -= 1.0f;
                grainTarget[i][k] = rndBipolar();   // new random delay for this grain
            }
            int wi = (int)(grainPhase[i][k] * (float)(HANN_LEN - 1));
            float w = hannTab[wi];
            wsum += w;
            dsum += w * grainTarget[i][k];
        }
        float grainMod = (wsum > 1e-6f) ? (dsum / wsum) * grainDepthS : 0.0f;

        float effDelay = baseDelay[i] * size + sineMod + grainMod;
        s[i] = readHermite(line[i], writeIdx, effDelay);
    }

    // 4) per-line damping (one-pole low pass inside the loop = absorption)
    for (int i = 0; i < NUM_LINES; ++i) {
        lpState[i] += (s[i] - lpState[i]) * dampCoef;
        s[i] = lpState[i];
    }

    // 5) unitary Householder feedback matrix (lossless), then decay gain
    float sum = s[0] + s[1] + s[2] + s[3];
    float fb[NUM_LINES];
    for (int i = 0; i < NUM_LINES; ++i)
        fb[i] = (s[i] - 0.5f * sum) * decay;

    // 6) inject input (alternating signs) and write the lines
    static const float sign[NUM_LINES] = {1.0f, -1.0f, 1.0f, -1.0f};
    for (int i = 0; i < NUM_LINES; ++i)
        line[i][writeIdx & LINE_MASK] = xin * sign[i] + fb[i];
    // Keep writeIdx inside one buffer span so (float)writeIdx stays exact.
    writeIdx = (writeIdx + 1) & LINE_MASK;

    // 7) stereo output from decorrelated line combinations
    float wetL = 0.5f * (s[0] - s[2]);
    float wetR = 0.5f * (s[1] - s[3]);

    // 8) tilt filter: split LP/HP around a fixed pivot, vary their balance
    tiltLpL += (wetL - tiltLpL) * tiltCoef;
    tiltLpR += (wetR - tiltLpR) * tiltCoef;
    float gLow  = (tilt <= 0.0f) ? 1.0f : (1.0f - tilt);
    float gHigh = (tilt >= 0.0f) ? 1.0f : (1.0f + tilt);
    wetL = gLow * tiltLpL + gHigh * (wetL - tiltLpL);
    wetR = gLow * tiltLpR + gHigh * (wetR - tiltLpR);

    // 9) wet/dry mix
    ResultL = in * (1.0f - mix) + wetL * mix;
    ResultR = in * (1.0f - mix) + wetR * mix;
}

void FdnReverb::processBlock(const float* in, float* outL, float* outR, int n)
{
    for (int i = 0; i < n; ++i) {
        process(in[i]);
        outL[i] = ResultL;
        outR[i] = ResultR;
    }
}

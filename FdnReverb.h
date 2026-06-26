#ifndef FDNREVERB_H
#define FDNREVERB_H
//
//  FdnReverb - Erbe-Verb-inspired 4-line Feedback Delay Network reverb.
//
//  Design notes (read me):
//  - ONE class, C style. All state is plain members / arrays, no virtual calls,
//    no per-sample allocation. Drops straight onto a Cortex-M7 or compiles on
//    Windows/Linux for testing.
//  - Two layers that work together (see process()):
//      1) The FDN engine     = makes the reverb (4 delays, Householder feedback,
//                              per-line damping, decay).
//      2) Modulators on top  = decide the *delay time* each line reads at:
//             effDelay[i] = baseDelay[i]*size  +  sineMod[i]  +  grainMod[i]
//         - global "size" : one smoothed multiplier on every delay (and the
//                           input diffusers), giving coordinated Doppler.
//         - sine LFO      : smooth periodic deviation (chorus / Doppler).
//         - Hann grains   : 4 overlapping raised-cosine grains per line, each
//                           jumping to a random delay -> breaks up resonance
//                           with NO chorus/Doppler (the Erbe-Verb "random" mod).
//      All three are read back through cubic Hermite interpolation so fractional
//      / moving delay lengths stay clean.
//
//  Float only (single-precision FPU on M7). Math is table-driven where it
//  matters (sin, Hann). Enable flush-to-zero once at startup (enableFTZ()).
//

#include <cstdint>
#include <math.h>   // floorf

namespace FDNReverb {

class FdnReverb {
public:
    // ---- lifecycle ----
    void  init(float sampleRate);          // clears buffers, builds tables
    void  enableFTZ();                      // denormal flush (M7 + x86)

    // ---- per-sample processing (results land in ResultL / ResultR) ----
    void  process(float in);
    void  processBlock(const float* in, float* outL, float* outR, int n);

    float ResultL = 0.0f;
    float ResultR = 0.0f;

    // ---- user parameters (set freely; they are smoothed internally) ----
    void setSize(float s);        // 0.05 .. 2.5   global room scale
    void setDecay(float d);       // 0.0 .. 1.0    feedback gain (1.0 ~ infinite)
    void setDamping(float d);     // 0.0 .. 1.0    0 = bright, 1 = dark
    void setDiffusion(float d);   // 0.0 .. 0.8    input allpass gain
    void setSineRate(float hz);   // 0.1 .. 30 Hz  periodic mod speed
    void setSineDepth(float ms);  // 0 .. ~20 ms   periodic mod depth
    void setGrainRate(float hz);  // 0.1 .. 200 Hz grain creation speed
    void setGrainDepth(float ms); // 0 .. ~30 ms   random delay spread
    void setTilt(float t);        // -1 dark .. 0 flat .. +1 bright
    void setMix(float m);         // 0 dry .. 1 wet

    // ---- sizing constants ----
    static const int   NUM_LINES   = 4;
    static const int   NUM_GRAIN   = 4;        // grain layers per line
    static const int   LINE_BITS   = 13;       // 8192 samples / line
    static const int   LINE_SIZE   = 1 << LINE_BITS;
    static const int   LINE_MASK   = LINE_SIZE - 1;
    static const int   AP_BITS     = 10;       // 1024 samples / input allpass
    static const int   AP_SIZE     = 1 << AP_BITS;
    static const int   AP_MASK     = AP_SIZE - 1;
    static const int   SIN_LEN     = 1024;     // LFO sine LUT
    static const int   HANN_LEN    = 1024;     // grain window LUT

private:
    // ---- helpers ----
    static inline float lerp(float a, float b, float c) { return a + (b - a) * c; }

    // 4-point Catmull-Rom Hermite. frac in [0,1) sits between y0 and yp1.
    static inline float hermite4(float frac, float ym1, float y0,
                                 float yp1, float yp2)
    {
        float c0 = y0;
        float c1 = 0.5f * (yp1 - ym1);
        float c2 = ym1 - 2.5f * y0 + 2.0f * yp1 - 0.5f * yp2;
        float c3 = 0.5f * (yp2 - ym1) + 1.5f * (y0 - yp1);
        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }
    inline float readHermite(const float* buf, uint32_t w, float delay) const
    {
        if (delay < 4.0f) delay = 4.0f;            // keep read off the write head
        float rp = (float)w - delay;
        int32_t i = (int32_t)floorf(rp);
        float frac = rp - (float)i;
        float ym1 = buf[(uint32_t)(i - 1) & LINE_MASK];
        float y0  = buf[(uint32_t)(i)     & LINE_MASK];
        float yp1 = buf[(uint32_t)(i + 1) & LINE_MASK];
        float yp2 = buf[(uint32_t)(i + 2) & LINE_MASK];
        return hermite4(frac, ym1, y0, yp1, yp2);
    }
    inline float rndBipolar()                      // xorshift32 -> [-1,1]
    {
        rngState ^= rngState << 13;
        rngState ^= rngState >> 17;
        rngState ^= rngState << 5;
        return (float)(int32_t)rngState * (1.0f / 2147483648.0f);
    }
    inline float sinLUT(float phase01) const       // phase in [0,1)
    {
        float fi = phase01 * (float)SIN_LEN;
        int   i  = (int)fi & (SIN_LEN - 1);
        float fr = fi - (float)((int)fi);
        return lerp(sinTab[i], sinTab[(i + 1) & (SIN_LEN - 1)], fr);
    }
    float inputDiffuse(float x);                   // 4 series allpass

    // ---- runtime constants ----
    float Fs        = 48000.0f;
    float pSmooth   = 0.0f;                         // param 1-pole coefficient

    // ---- delay lines ----
    float    line[NUM_LINES][LINE_SIZE];
    uint32_t writeIdx = 0;
    float    baseDelay[NUM_LINES];                  // samples at size = 1.0
    float    lpState[NUM_LINES] = {0,0,0,0};        // damping 1-pole state

    // ---- input diffusers (fixed integer delays) ----
    float ap[NUM_LINES][AP_SIZE];
    int   apLen[NUM_LINES] = {142, 107, 379, 277};
    int   apIdx[NUM_LINES] = {0, 0, 0, 0};

    // ---- modulation ----
    float sinePhase[NUM_LINES]  = {0.0f, 0.25f, 0.5f, 0.75f};
    float grainPhase[NUM_LINES][NUM_GRAIN];
    float grainTarget[NUM_LINES][NUM_GRAIN];        // current random deviation
    uint32_t rngState = 0x1234567u;

    // ---- LUTs ----
    float sinTab[SIN_LEN];
    float hannTab[HANN_LEN];

    // ---- smoothed parameters (cur = smoothed, *Tgt = target) ----
    float size = 1.0f,      sizeTgt = 1.0f;
    float decay = 0.85f,    decayTgt = 0.85f;
    float dampCoef = 0.5f,  dampTgt = 0.5f;         // 1-pole LP coef, 1 = bright
    float diffusion = 0.7f, diffTgt = 0.7f;
    float tilt = 0.0f,      tiltTgt = 0.0f;
    float mix = 1.0f,       mixTgt = 1.0f;

    float sineRateInc = 0.0f;                        // phase inc / sample
    float sineDepthS  = 0.0f;                        // samples
    float grainRateInc = 0.0f;
    float grainDepthS  = 0.0f;

    // ---- tilt filter state (output) ----
    float tiltLpL = 0.0f, tiltLpR = 0.0f;
    float tiltCoef = 0.2f;                           // fixed pivot ~ a few kHz
};

} // namespace PlateR
#endif

#ifndef PLATEREVERB_H
#define PLATEREVERB_H
namespace PlateR
{
    class Reverb
    {
        public:
            void ModTick();
            void Input(float input);
            void Tick();
            void LoTick();
            void CalculateVars();
            float Gain = 0.8f;
            float Diffusion = 0.5f;
            float Damping = 0.1f;
            float ResultL;
            float ResultR;
            float LFO1freq = 0.1;
            float LFO2freq = 0.08;
            float LFO1ampl = 0.2;
            float LFO2ampl = 0.2;

        private:
            const int Sample_Rate = 44100;
            float allPass(float input, float gain, float(&delayLine)[], int& play, int& rec);

            // use this together with calculateTime. CalculateTime should give the value for the parameter delayInSamples.
            // calculateTime parameter "milliSec" is the modulation control input
            float modAllPass(float input, float gain, float(&delayLine)[], int& play, int& rec, int delaySize, float &delayInSamples);
            float calculateTime(float milliSec, int &play, int &rec, int delaySize);

            float longDelay(float input, float(&delayLine)[], int& play, int& rec);
            float pipe1 = 0.0f;
            float pipe2 = 0.0f;
            float loPass1(float input, float gain);
            float loPass2(float input, float gain);
            float modWave1 = 0.0f;
            float modWave2 = 0.0f;
            float angle1 = 0; 
            float angle2 = 0;
            float modWaveRad = 0;
            float allPassMilliSec_1 = 30;
            float allPassMilliSec_2 = 22;
            float delayInSamples1 = 0;
            float delayInSamples2 = 0;
            inline float lerp(float a, float b, float c)
            {
                return a * (1.0 - c) + (b * c);
            }
            const float doublePI = 6.2831853;

        


    };
}
#endif
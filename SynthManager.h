#ifndef SYNTHMANAGER_H
#define SYNTHMANAGER_H
#include "PlateReverb.h"
#include "FdnReverb.h"
namespace SynthManager
{
    class Manager
    {
        public:
            float OutLeft();
            float OutRight();
            void Input(float* in);
            void Tick();
            void LoTick();
            void SetSize(float s);
            void SetDecay(float d);
            void SetTime(float g);
            void SetDamp(float d);
            void SetDiffusion(float d);
            void SetLFO1(float l);
            void setLFO2(float l);
            void setMod(float m);
            void setGrainDepth(float m);
            void setTilt(float t);
            void setMix(float m);
            void Init();

        private:
            // PlateR::Reverb reverb;
            FDNReverb::FdnReverb reverb;

    };
}

#endif
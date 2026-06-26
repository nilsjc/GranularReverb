#include "SynthManager.h"

float SynthManager::Manager::OutLeft()
{
    return reverb.ResultL;
}

float SynthManager::Manager::OutRight()
{
    return reverb.ResultR;
}

void SynthManager::Manager::Input(float *in)
{
    reverb.process(in[0]);
}

void SynthManager::Manager::Tick()
{
    // reverb.Tick();
    // reverb.ModTick();
}

void SynthManager::Manager::LoTick()
{
    // reverb.LoTick();
}

void SynthManager::Manager::SetSize(float s)
{
    // 0.05 .. 2.5
    reverb.setSize(s);
}

void SynthManager::Manager::SetDecay(float d)
{
    // 0.0 .. 1.0
    reverb.setDecay(d);
}

void SynthManager::Manager::SetTime(float g)
{
    // reverb.Gain = g; 0.0 ... 1.0
}

void SynthManager::Manager::SetDamp(float d)
{
    reverb.setDamping(d); // 0.0 ... 1.0
}

void SynthManager::Manager::SetDiffusion(float d)
{
    reverb.setDiffusion(d); // 0.0 ... 0.8
}

void SynthManager::Manager::SetLFO1(float l)
{
    reverb.setSineRate(l); // 0.1 ... 30.0 hz
}

void SynthManager::Manager::setLFO2(float l)
{
    reverb.setGrainRate(l); // 0.1 ... 200 hz
}

void SynthManager::Manager::setMod(float m)
{
    reverb.setSineDepth(m); // 0 ... 20ms
}

void SynthManager::Manager::setGrainDepth(float m)
{
    reverb.setGrainDepth(m); // 0 .. 30ms
}

void SynthManager::Manager::setTilt(float t)
{
    reverb.setTilt(t); // -1 dark .. 0 flat .. +1 bright
}

void SynthManager::Manager::setMix(float m)
{
    reverb.setMix(m); // 0 ... 1
}

void SynthManager::Manager::Init()
{
    reverb.init(44100.0f);
}

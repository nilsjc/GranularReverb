#include "PlateReverb.h" 
#include <cstdlib> 
#include <iostream> 
#include <math.h>
const int N=100;
// all pass filters
int ap1play=1, ap2play=1, ap3play=2, ap4play=2;
int ap1rec=0, ap2rec=0, ap3rec=0, ap4rec=0;
float ap1ring[210], ap2ring[158], ap3ring[561], ap4ring[410];
float ap1old, ap2old, ap3old, ap4old;
// all pass filters in pipes
float ap5ring[3931], ap6ring[2664];
int ap5play=3, ap6play=3;// was 2
int ap5rec=0,ap6rec=0;
// Modulated all pass filters
float mod1ring[13430], mod2ring[13430];
const int mod1ringSize = 13430;
const int mod2ringSize = 13430;
int mod1play=4, mod1rec=0, mod2play=4, mod2rec=0;
int mod1max = 13429, mod2max = 13429, direction=0;
// long delays
int ld1play=1, ld2play=1, ld3play=1, ld4play=1;
int ld1rec=0, ld2rec=0, ld3rec=0, ld4rec=0;
float ld1ring[6241], ld2ring[4641], ld3ring[6590], ld4ring[5505];
// lo pass ilters
float lp1Old = 0.0f, lp2Old = 0.0f;
// left taps
int leftTap1 = 394, leftTap2 = 4401, leftTap3 = 2831, leftTap4 = 2954, leftTap5 = 2945, leftTap6 = 277, leftTap7 = 1578;
int rightTap1 = 522, rightTap2 = 5368, rightTap3 = 1817, rightTap4 = 3956, rightTap5 = 3124, rightTap6 = 496, rightTap7 = 179;
void PlateR::Reverb::Input(float inp)
{
    // allpass * 4
    inp = allPass(inp, 0.75, ap1ring, ap1play, ap1rec);
    inp = allPass(inp, 0.75, ap2ring, ap2play, ap2rec);
    inp = allPass(inp, 0.625,ap3ring, ap3play, ap3rec);
    inp = allPass(inp, 0.625, ap4ring, ap4play, ap4rec);

    // pipe1: mod apf -> delay -> lpf -> apf -> delay
    float pipe1inp = inp + (pipe2 * Gain);
    //pipe1 = allPass(pipe1inp, (0.7 + modWave1), mod1ring, mod1play, mod1rec);
    pipe1 = modAllPass(pipe1inp, 0.7, mod1ring, mod1play, mod1rec, mod1ringSize, delayInSamples1);
    pipe1 = longDelay(pipe1, ld1ring, ld1play, ld1rec);
    pipe1 = loPass1(pipe1, Damping);
    pipe1 = allPass(pipe1, 0.5, ap5ring, ap5play, ap5rec);
    pipe1 = longDelay(pipe1, ld2ring, ld2play, ld2rec);
    // pipe2: mod apf -> delay -> lpf -> apf -> delay
    float pipe2inp = inp + (pipe1 * Gain);
    //pipe2 = allPass(pipe2inp, (0.7 + modWave2), mod2ring, mod2play, mod2rec);
    pipe2 = modAllPass(pipe1inp, 0.7, mod2ring, mod2play, mod2rec, mod2ringSize, delayInSamples2);
    pipe2 = longDelay(pipe2, ld3ring, ld3play, ld3rec);
    pipe2 = loPass2(pipe2, Damping);
    pipe2 = allPass(pipe2, 0.5, ap6ring, ap6play, ap6rec);
    pipe2 = longDelay(pipe2, ld4ring, ld4play, ld4rec);
    //result LEFT
    ResultL = ld1ring[leftTap1];
    ResultL += ld1ring[leftTap2];
    ResultL -= ap5ring[leftTap3];
    ResultL += ld2ring[leftTap4];
    ResultL -= ld3ring[leftTap5];
    ResultL -= ap6ring[leftTap6];
    ResultL -= ld4ring[leftTap7];
    //result RIGHT
    ResultR = ld3ring[rightTap1];
    ResultR += ld3ring[rightTap2];
    ResultR -= ap6ring[rightTap3];
    ResultR += ld4ring[rightTap4];
    ResultR -= ld1ring[rightTap5];
    ResultR -= ap5ring[rightTap6];
    ResultR -= ld2ring[rightTap7];
}

// int mod1max = 1343, mod2max = 994, modIndex = 0;
void PlateR::Reverb::ModTick()
{
    int rndN = rand()%N;
    if(rndN<98){
        
        
    }else{
        // // modulate between +-12 (-24 -12 0)
        // if(direction==1)
        // {
        //     mod1max++;
        //     mod2max++;
        //     if(mod1max>1342)
        //     {
        //         direction=0;
        //     }
        // }else{
        //     mod1max--;
        //     mod2max--;
        //     if(mod1max<1319)
        //     {
        //         direction=1;
        //     }
        // }
    }
    mod1play++;
    mod1rec++;
    mod2play++;
    mod2rec++;
    if(mod1play>mod1max)mod1play=0;
    if(mod2play>mod2max)mod2play=0;
    if(mod1rec>mod1max)mod1rec=0;
    if(mod2rec>mod2max)mod2rec=0;
    if(mod1play == mod1rec)mod1play++;
    if(mod2play == mod2rec)mod2play++;
}
void PlateR::Reverb::Tick()
{
    ap1play++;
    ap1rec++;
    ap2play++;
    ap2rec++;
    ap3play++;
    ap3rec++;
    ap4play++;
    ap4rec++;

    if(ap1play>209)ap1play=0;
    if(ap1rec>209)ap1rec=0;

    if(ap2play>157)ap2play=0;
    if(ap2rec>157)ap2rec=0;

    if(ap3play>560)ap3play=0;
    if(ap3rec>560)ap3rec=0;

    if(ap4play>409)ap4play=0;
    if(ap4rec>409)ap4rec=0;

    // long delays
    ld1play++;
    ld1rec++;
    ld2play++;
    ld2rec++;
    ld3play++;
    ld3rec++;
    ld4play++;
    ld4rec++;

    if(ld1play>6240)ld1play=0;
    if(ld1rec>6240)ld1rec=0;

    if(ld2play>4640)ld2play=0;
    if(ld2rec>4640)ld2rec=0;

    if(ld3play>6589)ld3play=0;
    if(ld3rec>6589)ld3rec=0;

    if(ld4play>5504)ld4play=0;
    if(ld4rec>5504)ld4rec=0;

    // pipe all pass filters ap5ring[3931], ap6ring[2664];

    ap5play++;
    ap5rec++;
    ap6play++;
    ap6rec++;

    if(ap5play>3930)ap5play=0;
    if(ap5rec>3930)ap5rec=0;

    if(ap6play>2663)ap6play=0;
    if(ap6rec>2663)ap6rec=0;

    // handle taps
    leftTap1++;
    leftTap2++;
    leftTap3++;
    leftTap4++;
    leftTap5++;
    leftTap6++;
    leftTap7++;
    if(leftTap1>6240)leftTap1=0;
    if(leftTap2>6240)leftTap2=0;
    if(leftTap3>3930)leftTap3=0;
    if(leftTap4>4640)leftTap4=0;
    if(leftTap5>6589)leftTap5=0;
    if(leftTap6>2663)leftTap6=0;
    if(leftTap7>5504)leftTap7=0;
    // int rightTap1 = 522, rightTap2 = 5368, rightTap3 = 1817, rightTap4 = 3956, 
    // rightTap5 = 3124, rightTap6 = 496, rightTap7 = 179;

    rightTap1++;
    rightTap2++;
    rightTap3++;
    rightTap4++;
    rightTap5++;
    rightTap6++;
    rightTap7++;
    if(rightTap1>6589)rightTap1=0;
    if(rightTap2>6589)rightTap2=0;
    if(rightTap3>2663)rightTap3=0;
    if(rightTap4>5504)rightTap4=0;
    if(rightTap5>6240)rightTap5=0;
    if(rightTap6>3930)rightTap6=0;
    if(rightTap7>4640)rightTap7=0;
}

void PlateR::Reverb::LoTick()
{
    // LFO 1 & 2 generation
    modWave1 = sin(angle1) * LFO1ampl;
    angle1 += modWaveRad * LFO1freq;
    if(angle1>doublePI){
        angle1 -= doublePI;
    }

    modWave2 = sin(angle2) * LFO2ampl;
    angle2 += modWaveRad * LFO2freq;
    if(angle2>doublePI){
        angle2 -= doublePI;
    }

    delayInSamples1 = calculateTime(modWave1 + allPassMilliSec_1, mod1play, mod1rec, mod1ringSize);
    delayInSamples2 = calculateTime(modWave2 + allPassMilliSec_2, mod2play, mod2rec, mod2ringSize);
}

void PlateR::Reverb::CalculateVars()
{
    modWaveRad = (2 * M_PI) / Sample_Rate;
}

float PlateR::Reverb::allPass(float input, float gain, 
    float(&delayLine)[], int& play, int& rec)
{
    if(gain>1.0)gain=1.0;
    float old = delayLine[play];
    input += (old * gain);
    delayLine[rec]=input;
    input *= -gain;
    return input + old;
}

float PlateR::Reverb::modAllPass(float input, float gain, float(&delayLine)[], int& play, int& rec, int delaySize, float &delayInSamples)
{
    if(gain>1.0)gain=1.0;
    float old = delayLine[play];
    if(play==rec && delayInSamples < 1.00){
        old = input;
    }
    int play_1 = play - 1;
    if(play_1 < 0){
        play_1 = delaySize-1;
    }
    float yn_1 = delayLine[play_1];
    // interpolate (0,yn) and (1,yn_1)by the amount fracdelay
    float fracDelay = delayInSamples - (int)delayInSamples;
    float finterp = lerp(old, yn_1, fracDelay);
    if(delayInSamples == 0)
    {
        old = input;
    }else{
        old = finterp;
    }
    input += (old * gain);
    delayLine[rec]=input;
    input *= -gain;
    return input + old;
}
float PlateR::Reverb::calculateTime(float milliSec, int &play, int &rec, int delaySize)
{
        float delayInSamples = milliSec * (float)Sample_Rate / 1000.0;
        play = rec - (int)delayInSamples;
        if(play<0)play += delaySize;
        return delayInSamples;
}

float PlateR::Reverb::longDelay(float input, float(&delayLine)[], int &play, int &rec)
{
    delayLine[rec]=input;
    return delayLine[play];
}

float PlateR::Reverb::loPass1(float input, float gain)
{
    input *= 1.0f-gain;
    input = (input + (lp1Old * gain));
    lp1Old = input;
    return input;
}

float PlateR::Reverb::loPass2(float input, float gain)
{
    input *= 1.0f-gain;
    input = (input + (lp2Old * gain));
    lp2Old = input;
    return input;
}

#include "AudioRecord.h"
#include <cmath>
namespace AudioInput
{
    TapeRecorder::TapeRecorder()
    {
        bufferSize = 0;
        bufferPos = 0;
        isPlaying = false;
        isRecording = false;
        playbackSpeed = 1.0; // Standard är normal hastighet
        readIndex = 0.0;
        audioBuffer.reserve(44100 * 4); 
    }

    // ... Input, Play, Stop, Rec är oförändrade ...

    void TapeRecorder::Input(float input)
    {
        this->input = input;
        if (isRecording)
        {
            // RÄTT: Skriv direkt på indexet istället för push_back.
            // Eftersom resize() redan har skapat platserna är detta säkert och blixtsnabbt.
            if (bufferPos < bufferSize)
            {
                audioBuffer[bufferPos] = input;
                bufferPos++;
            }

            if (bufferPos >= recEnd)
            {
                isRecording = false; // Stoppa när buffern är full
                bufferPos = 0; 
            }
        }
    }

    #include <cmath> // Krävs för std::floor

float TapeRecorder::MonoOut()
{
    if(!isRecording && !isPlaying){
        return this->input;
    }
    if (isRecording)
    {
        return 0.0f;
    }
    else if (isPlaying && bufferSize > 0)
    {
        // 1. Flytta läsindexet först (eller behåll nuvarande och flytta sist, 
        // men gränskontrollen MÅSTE göras innan vi beräknar index1 och index2)
        readIndex += playbackSpeed;

        // 2. Hantera loop-gränser direkt (både framlänges och baklänges)
        if (readIndex >= loopEnd)
        {
            readIndex -= loopEnd;
        }
        if (readIndex < 0.0)
        {
            readIndex += loopEnd;
        }

        // Extra säkerhetsbälte om loopEnd av någon anledning är större än bufferSize
        if (readIndex >= bufferSize) readIndex = 0.0;
        if (readIndex < 0.0) readIndex = 0.0;

        // 3. Skapa index med std::floor för att hantera negativa flyttal korrekt
        int index1 = (int)std::floor(readIndex);
        int index2 = index1 + 1;

        // 4. Skydda indexen så de håller sig inom [0, bufferSize - 1]
        if (index1 >= bufferSize) index1 = 0; // fallback
        if (index1 < 0)           index1 = bufferSize - 1; 
        
        if (index2 >= bufferSize) 
        {
            index2 = 0; 
        }
        if (index2 < 0)
        {
            index2 = bufferSize - 1;
        }

        // 5. Räkna ut fractional-delen för interpolationen (alltid positiv nu)
        float frac = (float)(readIndex - std::floor(readIndex));

        // 6. Hämta samplen och interpolera
        float sample1 = audioBuffer[index1];
        float sample2 = audioBuffer[index2];
        float outputSample = sample1 + frac * (sample2 - sample1);

        return outputSample;
    }
    return 0.0f; 
}


    void TapeRecorder::PitchChange(double speed)
    {
        //if (speed < 0.0) speed = 0.0; // Förhindra negativ hastighet (backa) i denna logik
        playbackSpeed = speed;
    }

    void TapeRecorder::ChangeLoopLength(int newLength)
    {
        if(newLength < readIndex) {
            readIndex = 0.0; // Om den nya längden är kortare än nuvarande läsindex, återställ till början
        }
        loopEnd = std::max(1, newLength); // Förhindra noll eller negativ längd
    }

    void TapeRecorder::Play()
    {
        isPlaying = true;
        isRecording = false;
        bufferPos = 0; // Starta uppspelning från början
    }

    void TapeRecorder::Stop()
    {
        isPlaying = false;
        isRecording = false;
    }

    void TapeRecorder::Rec(int targetBufferSize, int endOfRecord)
    {
        // resize() görs här. Se till att anropa Rec() från UI / huvudtråd, 
        // och INTE inuti själva ljud-callbacken, för att undvika ljudklick.
        recEnd = endOfRecord;
        audioBuffer.resize(targetBufferSize);
        audioBuffer.assign(audioBuffer.size(), 0.0f);
        this->bufferSize = targetBufferSize;
        bufferPos = 0;
        isPlaying = false;
        isRecording = true;
    }

}

#include "AudioRecord.h"

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
            // 1. Skapa index utifrån nuvarande läsindex
            int index1 = (int)readIndex;
            int index2 = index1 + 1;

            // 2. Skydda index2 om det hamnar utanför buffern
            if (index2 >= bufferSize) 
            {
                index2 = 0; 
            }

            // 3. Räkna ut fractional-delen för interpolationen
            float frac = (float)(readIndex - index1);

            // 4. Hämta samplen och interpolera
            float sample1 = audioBuffer[index1];
            float sample2 = audioBuffer[index2];
            float outputSample = sample1 + frac * (sample2 - sample1);

            // 5. Flytta läsindexet framåt baserat på hastighet
            readIndex += playbackSpeed;

            // 6. RÄTT: Loopa om hela readIndex om vi har passerat slutet
            if (readIndex >= bufferSize)
            {
                // Vi drar av bufferSize istället för att sätta till 0.0,
                // då behåller vi sub-sampel-precisionen för nästa varv!
                readIndex -= bufferSize; 
            }
            if(readIndex >= loopEnd)
            {
                readIndex -= loopEnd;
            }

            return outputSample;
        }
        return 0.0f; 
    }

    void TapeRecorder::PitchChange(double speed)
    {
        if (speed < 0.0) speed = 0.0; // Förhindra negativ hastighet (backa) i denna logik
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

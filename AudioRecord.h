#ifndef AUDIORECORD_H
#define AUDIORECORD_H

#include <vector>

namespace AudioInput
{
    class TapeRecorder
    {
        public:
            TapeRecorder();
            
            void Input(float input);
            float MonoOut();
            void Play();
            void Stop();
            void Rec(int targetBufferSize);
            void PitchChange(double speed);

        private:
            std::vector<float> audioBuffer;
            int bufferSize;
            int bufferPos;
            double playbackSpeed = 1.0; // 1.0 = normal, 2.0 = dubbel hastighet, 0.5 = halv hastighet
            bool isPlaying;
            bool isRecording;
            double readIndex = 0;
            float input;
    };
}

#endif // AUDIORECORD_H

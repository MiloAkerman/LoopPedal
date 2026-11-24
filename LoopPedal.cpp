#include "daisy_seed.h"

using namespace daisy;

DaisySeed hardware;

enum class LoopState
{
    Idle,
    Recording,
    Playing,
};

// Audio callback owns the state. Button events will drive transitions later.
LoopState loop_state = LoopState::Idle;

float ProcessSample(float input)
{
    switch(loop_state)
    {
        case LoopState::Idle:
            // Monitor input while waiting for a recording request.
            return input;
        case LoopState::Recording:
            // TODO: Store input in the loop buffer and transition when finished.
            return input;
        case LoopState::Playing:
            // TODO: Read and wrap the recorded loop's playback position.
            return 0.0f;
    }
    return 0.0f;
}

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    // TODO: Consume button events here to update loop_state.
    for(size_t i = 0; i < size; ++i)
    {
        const float sample = ProcessSample(in[0][i]);
        out[0][i] = sample;
        out[1][i] = sample;
    }
}

int main(void)
{
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hardware.StartAudio(AudioCallback);

    for(;;)
    {
        // TODO: Poll/debounce buttons and publish events to the audio callback.
        System::Delay(1);
    }
}

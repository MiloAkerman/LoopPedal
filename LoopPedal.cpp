#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;

DaisySeed hardware;
daisysp::Oscillator oscillator;

// Shared with the main loop; printing stays outside the audio interrupt.
volatile int input_peak_per_mille = 0;

enum class AudioTest
{
    Tone,              // Generate a tone on both outputs; inputs are ignored.
    StereoPassthrough, // Input 1 -> output 1; input 2 -> output 2.
    MonoPassthrough,   // Input 1 -> both outputs (useful for a mono pedal).
};

// Change this mode, rebuild, and flash to run each wiring test.
constexpr AudioTest kTest = AudioTest::MonoPassthrough;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    float input_peak = 0.0f;
    for(size_t i = 0; i < size; ++i)
    {
        const float magnitude = in[0][i] < 0.0f ? -in[0][i] : in[0][i];
        if(magnitude > input_peak)
            input_peak = magnitude;

        switch(kTest)
        {
            case AudioTest::Tone:
            {
                const float sample = oscillator.Process();
                out[0][i] = sample;
                out[1][i] = sample;
                break;
            }
            case AudioTest::StereoPassthrough:
                out[0][i] = in[0][i];
                out[1][i] = in[1][i];
                break;
            case AudioTest::MonoPassthrough:
                out[0][i] = in[0][i] * 70.0f;
                out[1][i] = in[0][i] * 70.0f;
                break;
        }
    }
    input_peak_per_mille = static_cast<int>(input_peak * 1000.0f);
}

int main(void)
{
    hardware.Configure();
    hardware.Init();
    hardware.StartLog();

    oscillator.Init(hardware.AudioSampleRate());
    oscillator.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    oscillator.SetFreq(440.0f);
    oscillator.SetAmp(0.99f);

    hardware.StartAudio(AudioCallback);

    for(;;)
    {
        hardware.PrintLine("Input 1 block peak: %d / 1000", input_peak_per_mille);
        System::Delay(100);
    }
}

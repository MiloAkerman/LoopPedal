#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;

DaisySeed hardware;
daisysp::Oscillator oscillator;

// Shared with the main loop; printing stays outside the audio interrupt.
volatile int input_peak_per_mille = 0;

constexpr size_t kSampleRate = 48000;
constexpr size_t kWaitSamples = 2 * kSampleRate;
constexpr size_t kLoopSamples = 5 * kSampleRate;
constexpr float kLoopOutputGain = 70.0f;

float DSY_SDRAM_BSS loop_buffer[kLoopSamples];

enum class LoopState
{
    Waiting,
    Recording,
    Playing,
};

volatile LoopState loop_state = LoopState::Waiting;
size_t wait_position = 0;
size_t loop_position = 0;

float ProcessAutoLoop(float input)
{
    float sample = input;
    switch(loop_state)
    {
        case LoopState::Waiting:
            if(++wait_position == kWaitSamples)
                loop_state = LoopState::Recording;
            break;
        case LoopState::Recording:
            // Store the raw input, so monitoring gain is not baked into the loop.
            loop_buffer[loop_position++] = input;
            if(loop_position == kLoopSamples)
            {
                loop_position = 0;
                loop_state = LoopState::Playing;
            }
            break;
        case LoopState::Playing:
            // Playback only: new input is not mixed in or recorded.
            sample = loop_buffer[loop_position++];
            if(loop_position == kLoopSamples)
                loop_position = 0;
            break;
    }

    sample *= kLoopOutputGain;
    // Bound codec output values. Excessive gain still causes audible clipping.
    return sample > 1.0f ? 1.0f : (sample < -1.0f ? -1.0f : sample);
}

enum class AudioTest
{
    Tone,              // Generate a tone on both outputs; inputs are ignored.
    StereoPassthrough, // Input 1 -> output 1; input 2 -> output 2.
    MonoPassthrough,   // Input 1 -> both outputs (useful for a mono pedal).
    AutoLoop,          // Wait 2 seconds, record 5 seconds, then repeat.
};

// Change this mode, rebuild, and flash to run each wiring test.
constexpr AudioTest kTest = AudioTest::AutoLoop;

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
            case AudioTest::AutoLoop:
            {
                const float sample = ProcessAutoLoop(in[0][i]);
                out[0][i] = sample;
                out[1][i] = sample;
                break;
            }
        }
    }
    input_peak_per_mille = static_cast<int>(input_peak * 1000.0f);
}

int main(void)
{
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hardware.StartLog();

    oscillator.Init(hardware.AudioSampleRate());
    oscillator.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    oscillator.SetFreq(440.0f);
    oscillator.SetAmp(0.99f);

    hardware.StartAudio(AudioCallback);

    for(;;)
    {
        if(kTest == AudioTest::AutoLoop)
        {
            const LoopState state = loop_state;
            const char* label = state == LoopState::Waiting ? "waiting"
                                : state == LoopState::Recording ? "recording"
                                                               : "playing";
            hardware.SetLed(state == LoopState::Recording);
            hardware.PrintLine("Loop: %s | Input 1 block peak: %d / 1000",
                               label,
                               input_peak_per_mille);
        }
        else
            hardware.PrintLine("Input 1 block peak: %d / 1000", input_peak_per_mille);
        System::Delay(100);
    }
}

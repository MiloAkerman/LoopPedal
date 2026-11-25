#include "daisy_seed.h"
#include "button_input.h" // 

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

float DSY_SDRAM_BSS loopBuffer[MAX_SAMPLES];// buffer to hold the recorded loop, size defined in button_input.h

size_t recordIndex = 0;
size_t playIndex = 0;
size_t loopLength = 0;

float ProcessSample(float input)
{
    bool held = flag_held;
    bool pressed = flag_down;

    flag_held = false;
    flag_down = false;

    if(held){
        pressed = false; // ignore press if held
    }
    switch(loop_state)
    {
        case LoopState::Idle:
            // Monitor input while waiting for a recording request.
            if(pressed) // start recording on press, ignore held
            {
                recordIndex = 0;
                playIndex = 0;
                loopLength = 0;

                loop_state = LoopState::Recording;
            }
            return input;
        case LoopState::Recording:
            // TODO: Store input in the loop buffer and transition when finished.
            if(held){// discard current recording if held
                recordIndex = 0;
                playIndex = 0;
                loopLength = 0;
                loop_state = LoopState::Idle;

                return input;
            }
            else if(pressed){// finish recording on press and save 
                loopLength = recordIndex;// save the length of the loop
                playIndex = 0;// reset the play index for playback

                if(loopLength > 0){// only transition to playing if we have a valid loop
                    loop_state = LoopState::Playing;
                }
                else{
                    loop_state = LoopState::Idle; // stay in idle if no valid loop
                }
                
                return input;
            }
            // save samples to the loop buffer
            if(recordIndex < 48000){// limit recording to 1 second (48000 samples at 48kHz)
                // TODO: Implement loop buffer storage
                loopBuffer[recordIndex] = input;
                recordIndex++;// increment the record index
            }

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

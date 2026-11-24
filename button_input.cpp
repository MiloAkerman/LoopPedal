#include "daisy_seed.h"
#include "button_input.h"
using namespace daisy;

//only button 1 is used for 'press' and 'held'
static GPIO button1;
static GPIO button2;
static TimerHandle buttonTimer;                          // NEW

volatile bool flag_down = false;
volatile bool flag_held = false;
const uint32_t t_hold = 1000;

static bool heldReported = false;
static uint32_t downStart = 0;
static bool isDown = false;

// --- your functions: unchanged ---
bool Button1Pressed(){
    if (button1.Read()){
        isDown = false;
        return false;
    }
    else{
        if (!isDown){
            downStart = System::GetNow();
            flag_down = true;
        }
        isDown = true;
        return true;
    }
}

bool Button2Pressed(){
    return !button2.Read();
}

bool Button1Held(){
    if (isDown && !heldReported && (System::GetNow() - downStart >= t_hold)){
        flag_held    = true;
        heldReported = true;
        return true;
    }
    if (!isDown) heldReported = false;
    return false;
}

// --- the ISR: hardware calls this every 1 ms ---      // NEW
static void ButtonISR(void* data)                        // NEW
{                                                        // NEW
    Button1Pressed();                                    // NEW
    Button1Held();                                       // NEW
}                                                        // NEW

void InitButtons()
{
    button1.Init(seed::D0, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);   // was D3
    button2.Init(seed::D1, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);   // was D4
    TimerHandle::Config cfg;                             // NEW
    cfg.periph     = TimerHandle::Config::Peripheral::TIM_5;  // NEW
    cfg.enable_irq = true;                               // NEW
    cfg.period     = System::GetPClk2Freq() / 100;      // NEW: 1 ms
    buttonTimer.Init(cfg);                               // NEW
    buttonTimer.SetCallback(ButtonISR);                  // NEW
    buttonTimer.Start();                                 // NEW
}
#include "daisy_seed.h"
#include "button_input.h"
using namespace daisy;

DaisySeed hw;

int main(void)
{
    hw.Init();
    hw.StartLog(true);
    InitButtons();          // now also starts the timer ISR
    hw.PrintLine("Ready");

    for(;;)
    {
        // Button1Pressed();      // REMOVED: the ISR calls it now
        // Button1Held();         // REMOVED: the ISR calls it now

        if (flag_down) { hw.PrintLine("pressed"); flag_down = false; }
        if (flag_held) { hw.PrintLine("held");    flag_held = false; }

        // System::Delay(10);     // REMOVED: the timer handles timing
    }
}
#include "daisy_seed.h"
#include "button_input.h"   
using namespace daisy;

//only button 1 is used for 'press' and 'held'
static GPIO button1;
static GPIO button2;
static uint32_t downStart = 0;
static bool isDown = false;

void InitButtons()
{
    button1.Init(seed::D3, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    button2.Init(seed::D4, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
}

bool Button1Pressed(){
    if (button1.Read()){
        isDown = false;
        return false;
    }
    else{
        if (!isDown){
            downStart = System::GetNow();
        }
        isDown = true;
        return true;
    }
    return !button1.Read();
}

bool Button2Pressed(){
    return !button2.Read();
}

bool Button1Held(){
    uint32_t now  = System::GetNow();
    if (isDown && ((now - downStart) >= 1000)){
        return true;
    }
    return false;
}
#include "daisy_seed.h"

using namespace daisy;

DaisySeed hardware;

int main(void)
{
    hardware.Configure();
    hardware.Init();
    AdcChannelConfig adc_config;
    adc_config.InitSingle(seed::A0);
    hardware.adc.Init(&adc_config, 1);
    hardware.adc.Start();
    hardware.StartLog();

    for(;;)
    {
        const int sample = static_cast<int>(*hardware.adc.GetPtr(0));
        hardware.PrintLine("ADC 0: %d", sample);
        System::Delay(1);
    }
}
//test

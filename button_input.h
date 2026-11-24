#pragma once
#include <cstdint>

extern volatile bool flag_down;
extern volatile bool flag_held;

void InitButtons();
bool Button1Pressed();
bool Button2Pressed();
bool Button1Held();
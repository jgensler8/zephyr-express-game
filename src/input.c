#include "input.h"

joypads_t joypads;
joypads_t last_joy;

void input_scan(void)
{
    last_joy = joypads;
    joypad_ex(&joypads);
}
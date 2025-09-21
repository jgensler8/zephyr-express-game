#include "sound.h"
#include <gbdk/platform.h>

// https://gbdev.gg8.se/wiki/articles/Gameboy_sound_hardware
//        Noise
//      FF1F ---- ---- Not used
// NR41 FF20 --LL LLLL Length load (64-L)
// NR42 FF21 VVVV APPP Starting volume, Envelope add mode, period
// NR43 FF22 SSSS WDDD Clock shift, Width mode of LFSR, Divisor code
// NR44 FF23 TL-- ---- Trigger, Length enable
#ifdef GAMEBOY
static inline void sound_light(void)
{
    NR41_REG = 0x01; // Sound length
    NR42_REG = 0xF1; // Envelope
    NR43_REG = 0x30; // Frequency
    NR44_REG = 0x80; // Trigger
}
static inline void sound_medium(void)
{
    NR41_REG = 0x01;
    NR42_REG = 0xF2;
    NR43_REG = 0x3F;
    NR44_REG = 0x60;
}
static inline void sound_low_plop(void)
{
    NR41_REG = 0x01;
    NR42_REG = 0xF1;
    NR43_REG = 0x4F;
    NR44_REG = 0x80;
}
static inline void sound_high_buzz(void)
{
    NR41_REG = 0x01;
    NR42_REG = 0xF1;
    NR43_REG = 0x2F;
    NR44_REG = 0x80;
}
static inline void sound_high(void)
{
    NR41_REG = 0x01;
    NR42_REG = 0xF1;
    NR43_REG = 0x1F;
    NR44_REG = 0x80;
}
#endif
#ifdef NINTENDO_NES
static inline void sound_light(void)
{
}
static inline void sound_medium(void)
{
}
static inline void sound_low_plop(void)
{
}
static inline void sound_high_buzz(void)
{
}
static inline void sound_high(void)
{
}
#endif

void sound_init(void)
{
#ifdef GAMEBOY
    // Turn on the sound hardware
    NR52_REG = 0x80; // Turn on sound
    NR51_REG = 0xFF; // Enable all channels on both speakers
    NR50_REG = 0x77; // Max volume
#endif
}
void sound_on_menu_back(void)
{
    sound_low_plop();
}
void sound_on_menu_select(void)
{
    sound_light();
}
void sound_on_menu_confirm(void)
{
    sound_high_buzz();
}
void sound_on_round_start(void)
{
}
void sound_on_tool_acquired(void)
{
    sound_high_buzz();
}
void sound_on_tool_released(void)
{
    sound_low_plop();
}
void sound_on_task_tick(void)
{
    sound_light();
}
void sound_on_task_complete(void)
{
    sound_high();
}
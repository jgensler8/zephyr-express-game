#include "sound.h"
#include <gbdk/platform.h>

uint8_t ticks;
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
static inline void sound_stop(void)
{
}
#endif
#ifdef NINTENDO_NES
// https://github.com/untoxa/BlackCastle/blob/3202330aac50815fe56de8009d70a78c3c1df984/include/music.h#L102
// the auto formatter messes these lines up, sorry
__REG(0x4000)
_SND4000;
__REG(0x4001)
_SND4001;
__REG(0x4002)
_SND4002;
__REG(0x4003)
_SND4003;
__REG(0x4004)
_SND4004;
__REG(0x4005)
_SND4005;
__REG(0x4006)
_SND4006;
__REG(0x4007)
_SND4007;
__REG(0x4008)
_SND4008;
__REG(0x4009)
_SND4009;
__REG(0x400A)
_SND400A;
__REG(0x400B)
_SND400B;
__REG(0x400C)
_SND400C;
__REG(0x400D)
_SND400D;
__REG(0x400E)
_SND400E;
__REG(0x400F)
_SND400F;
__REG(0x4010)
_SND4010;
__REG(0x4011)
_SND4011;
__REG(0x4012)
_SND4012;
__REG(0x4013)
_SND4013;
__REG(0x4015)
_SND4015;
// https://www.nesdev.org/wiki/APU_basics
static inline void sound_light(void)
{
    // $4000	$4004	%DD11VVVV	Duty cycle and volume
    // DD: 00=12.5% 01=25% 10=50% 11=75%
    // VVVV: 0000=silence 1111=maximum
    // $4002	$4006	%LLLLLLLL	Low 8 bits of raw period
    // $4003	$4007	%-----HHH	High 3 bits of raw period
    _SND4000 = 0b01111111;
    _SND4001 = 0b01111111;
    _SND4002 = 0b01011111;
    _SND4003 = 0b00000011;
    ticks = 2;
}
static inline void sound_low_plop(void)
{
    ticks = 2;
    _SND4000 = 0b11111111;
    _SND4001 = 0b01111111;
    _SND4002 = 0b11111111;
    _SND4003 = 0b01010101;
}
static inline void sound_high_buzz(void)
{
    ticks = 4;
    _SND4000 = 0b11111111;
    _SND4001 = 0b11111111;
    _SND4002 = 0b00000000;
    _SND4003 = 0b11111111;
}
static inline void sound_high(void)
{
    ticks = 2;
    _SND4000 = 0b01111111;
    _SND4001 = 0b01111111;
    _SND4002 = 0b01010101;
    _SND4003 = 0b00000000;
}
static inline void sound_stop(void)
{
    _SND4000 = 0b01111111;
    _SND4001 = 0b00000000;
    _SND4002 = 0b00000000;
    _SND4003 = 0b00000000;
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
#ifdef NINTENDO_NES
    _SND4015 = 0x0F;
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
void sound_tick(void)
{
    if (ticks > 0)
    {
        ticks -= 1;
    }
    else
    {
        sound_stop();
    }
}
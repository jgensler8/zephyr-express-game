#pragma once
#include <gbdk/platform.h>

extern joypads_t joypads;
extern joypads_t last_joy;

#define PRESSED(player, key) ((joypads.joypads[player] & (key)) && !(last_joy.joypads[player] & (key)))
#define RELEASED(player, key) (!(joypads.joypads[player] & (key)) && (last_joy.joypads[player] & (key)))

void input_scan(void);
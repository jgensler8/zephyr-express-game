#include "font.h"
#include <gbdk/font.h>

#define ALPHABET_CHARS 26
uint8_t unpacked_font[ALPHABET_CHARS * 4 * 4];

// copy the font out of the default struct
void font_unpack(void)
{
    font_init();
    font_load(font_min);
    get_bkg_data(11, ALPHABET_CHARS, unpacked_font);
}

uint8_t font_load_tile;
// load the font at a particular tile. this tile is saved and used in font_print
void font_set_bkg_data(uint8_t tile)
{
    font_load_tile = tile;
    set_bkg_data(font_load_tile, ALPHABET_CHARS, unpacked_font);
}

void font_print(uint8_t x, uint8_t y, char *str)
{

    uint8_t *next = str;
    while (*next != 0)
    {
        uint8_t offset = *next - 'A';
        uint8_t tile = font_load_tile + offset;
        if (offset >= ALPHABET_CHARS)
        {
            tile = 0;
        }
        set_bkg_tile_xy(x, y, tile);
        next++;
        x++;
    }
}
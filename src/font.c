#include "font.h"
#include "gen/font_alphabet.h"

uint8_t font_load_tile;
// load the font at a particular tile. this tile is saved and used in font_print
void font_set_bkg_data(uint8_t tile)
{
    font_load_tile = tile;
    set_bkg_data(font_load_tile, font_alphabet_TILE_COUNT, font_alphabet_tiles);
}

void font_print(uint8_t x, uint8_t y, char *str)
{

    uint8_t *next = str;
    while (*next != 0)
    {
        uint8_t offset = *next - 'A';
        uint8_t tile = font_load_tile + offset;
        if (offset >= font_alphabet_TILE_COUNT)
        {
            tile = 0;
        }
        set_bkg_tile_xy(x, y, tile);
        next++;
        x++;
    }
}
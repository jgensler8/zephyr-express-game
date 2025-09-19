#include "font.h"
#include "gen/font_alphabet.h"
#include "gen/font_numeric.h"

uint8_t font_load_tile;
// load the font at a particular tile. this tile is saved and used in font_print
void font_set_bkg_data(uint8_t tile)
{
    font_load_tile = tile;
    set_bkg_data(font_load_tile, font_alphabet_TILE_COUNT, font_alphabet_tiles);
}

uint8_t numeric_load_tile;
void font_set_bkg_data_numeric(uint8_t tile)
{
    numeric_load_tile = tile;
    set_bkg_data(numeric_load_tile, font_numeric_TILE_COUNT, font_numeric_tiles);
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

// prints number right to left (for implementation simplicity)
void font_print_numeric(uint8_t x, uint8_t y, uint8_t num)
{
    do
    {
        uint8_t place = num % 10;
        set_bkg_tile_xy(x, y, numeric_load_tile + place);
        x--;
        num = num / 10;
    } while (num != 0);
}
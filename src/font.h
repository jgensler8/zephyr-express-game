#include <gbdk/platform.h>

#define FONT_GAMEPLAY_SAFE_TILE 198
void font_set_bkg_data(uint8_t tile);
void font_set_bkg_data_numeric(uint8_t tile);
void font_print(uint8_t x, uint8_t y, char *str);
void font_print_numeric(uint8_t x, uint8_t y, uint8_t num);
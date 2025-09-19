#include <gbdk/platform.h>
#include "types.h"
#include "input.h"
#include <rand.h>
#include "scenes.h"
#include "gen/conductor_0.h"
#include "gen/conductor_1.h"
#include "gen/conductor_2.h"
#include "gen/conductor_3.h"
#ifdef NINTENDO_NEX
#include "gen/player_banner_0.h"
#include "gen/player_banner_1.h"
#endif
#include "gen/train_map_0.h"
#include "gen/train_player_logo_0.h"
#include "gen/train_player_logo_1.h"
#include "gen/train_player_logo_2.h"
#include "gen/train_player_logo_3.h"
#include "gen/tool_wifi.h"
#include "gen/tool_wrench.h"
#include "gen/tool_drink.h"
#include "gen/tool_cat.h"
#include "gen/tool_music.h"
#include "gen/bg_train_passenger.h"
#include "gen/tasks.h"
#include "gen/customer_status.h"
#include "gen/map_progress.h"
#define TRAIN_CAR_LEN bg_train_passenger_WIDTH
#define TRAIN_CAR_HEIGHT bg_train_passenger_HEIGHT
#define TRAIN_DOOR_TELEPORT_MARGIN 4
// adjust for sprite height
#define TRAIN_FLOOR_BASELINE (40)
#define TRAIN_FLOOR_SEPARATION 16
#define TRAIN_LOWER_FLOOR (TRAIN_FLOOR_BASELINE + TRAIN_FLOOR_SEPARATION)
#define TRAIN_UPPER_FLOOR (TRAIN_FLOOR_BASELINE - TRAIN_FLOOR_SEPARATION)

#ifdef NINTENDO_NES
// #define PLAYER_X_ADJUST(current_player) ((current_player == 0 || current_player == 2) ? 0 : 120)
#define PLAYER_X_ADJUST(current_player) (0)
#define PLAYER_X_ADJUST_TILE(current_player) (0)
uint8_t _player_y_adjust[4] = {TRAIN_CAR_HEIGHT * 0, TRAIN_CAR_HEIGHT * 1, TRAIN_CAR_HEIGHT * 2, TRAIN_CAR_HEIGHT * 3};
#define PLAYER_Y_ADJUST(current_player) (_player_y_adjust[current_player])
uint8_t _player_y_adjust_tile[4] = {TRAIN_CAR_HEIGHT / 8 * 0, TRAIN_CAR_HEIGHT / 8 * 1, TRAIN_CAR_HEIGHT / 8 * 2, TRAIN_CAR_HEIGHT / 8 * 3};
#define PLAYER_Y_ADJUST_TILE(current_player) (_player_y_adjust_tile[current_player])
#else
#define PLAYER_X_ADJUST(current_player) (0)
#define PLAYER_Y_ADJUST(current_player) (0)
#define PLAYER_X_ADJUST_TILE(current_player) (0)
#define PLAYER_Y_ADJUST_TILE(current_player) (0)
#endif

#define RANDOM_LOWER_FLOOR_POSITION (40 + (rand() % 40))

#define TOOL_RAISE_PIXELS 4
void update_tool_cars(struct game_state *state)
{
    for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
    {
        if (state->tools[tool].player_holding != PLAYER_HOLDING_NONE)
        {
            state->tools[tool].car = state->player_positions[state->tools[tool].player_holding].car;
        }
    }
}
void reset_player_car_changed(struct game_state *state)
{
    for (uint8_t player = 0; player < MAX_PLAYERS; player++)
    {
        state->player_car_changed[player] = 0;
    }
}
uint8_t tool_held(struct game_state *state, uint8_t player)
{
    for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
    {
        if (state->tools[tool].player_holding == player)
        {
            return tool;
        }
    }
    return TOOL_NONE;
}
uint8_t tool_near(struct game_state *state, enum TOOL tool, uint8_t player)
{
    uint8_t nearness_margin_x = 16;
    uint8_t nearness_margin_y = 16;
    return state->tools[tool].x >= state->player_positions[player].x - nearness_margin_x &&
           state->tools[tool].x <= state->player_positions[player].x + nearness_margin_x &&
           state->tools[tool].y >= state->player_positions[player].y - nearness_margin_y &&
           state->tools[tool].y <= state->player_positions[player].y + nearness_margin_y;
}
void maybe_interpolate_direction(struct game_state *state, uint8_t player)
{
#define PLAYER_X state->player_positions[player].x
#define PLAYER_Y state->player_positions[player].y
#define PLAYER_DIRECTION state->player_positions[player].direction
#define TRAIN_RIGHT_STAIRS_UPPER 119
#define TRAIN_STAIRS_LEN TRAIN_FLOOR_SEPARATION
    // right ladder enter top
    if (PLAYER_X == TRAIN_RIGHT_STAIRS_UPPER && PLAYER_Y == TRAIN_FLOOR_BASELINE && PLAYER_DIRECTION == DIRECTION_LEFT)
    {
        // right stays same but left goes up
        PLAYER_X -= 1;
        PLAYER_Y -= 1;
    }
    else if ((PLAYER_X == TRAIN_RIGHT_STAIRS_UPPER - TRAIN_STAIRS_LEN) && PLAYER_Y == TRAIN_UPPER_FLOOR && PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
        // left stays same but right is down
        PLAYER_X += 1;
        PLAYER_Y += 1;
    }
    else if ((TRAIN_RIGHT_STAIRS_UPPER - TRAIN_STAIRS_LEN) < PLAYER_X && PLAYER_X < TRAIN_RIGHT_STAIRS_UPPER && TRAIN_UPPER_FLOOR < PLAYER_Y && PLAYER_Y < TRAIN_FLOOR_BASELINE)
    {
        if (PLAYER_DIRECTION == DIRECTION_LEFT)
        {
            PLAYER_X -= 1;
            PLAYER_Y -= 1;
        }
        else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
        {
            PLAYER_X += 1;
            PLAYER_Y += 1;
        }
    }
    // left drop point top level
    else if (PLAYER_X == 24 && PLAYER_Y == TRAIN_UPPER_FLOOR)
    {
        PLAYER_Y = TRAIN_FLOOR_BASELINE;
    }
    else if (PLAYER_X == 40 && PLAYER_Y == TRAIN_FLOOR_BASELINE)
    {
        PLAYER_Y = TRAIN_LOWER_FLOOR;
    }
    // right stirs leave bottom
#define TRAIN_RIGHT_STAIRS_LOWER 120
    else if (PLAYER_X == TRAIN_RIGHT_STAIRS_LOWER && PLAYER_Y == TRAIN_LOWER_FLOOR && PLAYER_DIRECTION == DIRECTION_RIGHT)
    {
        // left stays same but right goes up
        PLAYER_X += 1;
        PLAYER_Y -= 1;
    }
    // note: left stairs lead onto main floor so no exception needed for backtracking
    else if (TRAIN_RIGHT_STAIRS_LOWER < PLAYER_X && PLAYER_X < (TRAIN_RIGHT_STAIRS_LOWER + TRAIN_STAIRS_LEN) && TRAIN_LOWER_FLOOR > PLAYER_Y && PLAYER_Y > TRAIN_FLOOR_BASELINE)
    {
        if (PLAYER_DIRECTION == DIRECTION_LEFT)
        {
            PLAYER_X -= 1;
            PLAYER_Y += 1;
        }
        else if (PLAYER_DIRECTION == DIRECTION_RIGHT)
        {
            PLAYER_X += 1;
            PLAYER_Y -= 1;
        }
    }
    // all other movement
    else
    {
        if (PLAYER_DIRECTION == DIRECTION_LEFT && PLAYER_X > 0)
        {
            PLAYER_X += DIRECTION_LEFT;
        }
        else if (PLAYER_DIRECTION == DIRECTION_RIGHT && PLAYER_X < TRAIN_CAR_LEN)
        {
            PLAYER_X += DIRECTION_RIGHT;
        }
    }
}
void handle_input(struct game_state *state, uint8_t player)
{
    // uint8_t joy = joypad();
    uint8_t joy = joypads.joypads[player];
    // directional movment
    if (joy & J_LEFT)
    {
        state->player_positions[player].direction = DIRECTION_LEFT;
    }
    else if (joy & J_RIGHT)
    {
        state->player_positions[player].direction = DIRECTION_RIGHT;
    }
    else
    {
        state->player_positions[player].direction = DIRECTION_NONE;
    }
    maybe_interpolate_direction(state, player);

    // car teleports
    if (state->player_positions[player].x < TRAIN_DOOR_TELEPORT_MARGIN)
    {

        if (state->player_positions[player].car > 0)
        {
            state->player_car_changed[player] = 1;
            state->player_positions[player].car -= 1;
            state->player_positions[player].x = TRAIN_CAR_LEN - TRAIN_DOOR_TELEPORT_MARGIN;
            update_tool_cars(state);
        }
    }
    else if (state->player_positions[player].x > TRAIN_CAR_LEN - TRAIN_DOOR_TELEPORT_MARGIN)
    {
        if (state->player_positions[player].car < state->cars - 1)
        {
            state->player_car_changed[player] = 1;
            state->player_positions[player].car += 1;
            state->player_positions[player].x = TRAIN_DOOR_TELEPORT_MARGIN;
            update_tool_cars(state);
        }
    }

    // handle tools + tasks
    uint8_t j_a_pressed = PRESSED(player, J_A);
    if (j_a_pressed)
    {
        uint8_t held = tool_held(state, player);
        if (held != TOOL_NONE)
        {
            // drop tool
            state->tools[held].player_holding = PLAYER_HOLDING_NONE;
            state->tools[held].y += TOOL_RAISE_PIXELS;
        }
        else
        {
            // maybe pick up tool
            for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
            {
                if (state->tools[tool].unlocked &&
                    state->tools[tool].car == state->player_positions[player].car &&
                    state->tools[tool].player_holding == PLAYER_HOLDING_NONE &&
                    tool_near(state, tool, player))
                {
                    if (state->tools[tool].player_holding == PLAYER_HOLDING_NONE)
                    {
                        state->tools[tool].player_holding = player;
                        // state->tools[tool].y -= 16;
                        break;
                    }
                    else if (state->tools[tool].player_holding == player)
                    {
                        state->tools[tool].player_holding = PLAYER_HOLDING_NONE;
                        // state->tools[tool].y += 16;
                        break;
                    }
                }
            }
        }
    }
}

void animation_progress(const struct animation_const *data, struct animation_state *state)
{
    // handle frame wait
    if (state->frame_tick > 0)
    {
        state->frame_tick -= 1;
        return;
    }
    state->frame_tick = data->frame_wait;
    // advance frame
    if (state->frame == 0)
    {
        state->direction = 1;
    }
    else if (state->frame == data->frames - 1)
    {
        state->direction = -1;
    }
    state->frame = state->frame + state->direction;
    set_sprite_data(data->data_idx, data->data_tile_count, data->frame_datas[state->frame]);
}

void animation_idle(const struct animation_const *data, struct animation_state *state)
{
    state->frame = data->neutral_frame;
    state->frame_tick = 0;
    state->direction = 1;
    set_sprite_data(data->data_idx, data->data_tile_count, data->frame_datas[state->frame]);
}
#define TRAIN_MAP_BG_X_START 1
#define TRAIN_MAP_BG_Y_START 0
#define TRAIN_MAP_CAR_TILE_START 1
void initialize_train_map(void)
{
    set_bkg_data(TRAIN_MAP_CAR_TILE_START, train_map_0_TILE_COUNT, train_map_0_tiles);
}

#define TILES_PER_CAR 6
#define X_PER_CAR(s) { \
    s + 0,             \
    s + 1,             \
    s + 2,             \
    s + 0,             \
    s + 1,             \
    s + 2,             \
}
const uint8_t _car_x[MAX_CARS][TILES_PER_CAR] = {
    X_PER_CAR(0),
    X_PER_CAR(3),
    X_PER_CAR(6),
    X_PER_CAR(9),
    X_PER_CAR(12),
    X_PER_CAR(15),
};
void draw_train_map(struct game_state *state)
{
    // draw cars left to right
    for (uint8_t car = 0; car < state->cars; car++)
    {
        uint8_t tile = 0;
        for (uint8_t t_y = 0; t_y < (train_map_0_HEIGHT / 8); t_y++)
        {
            for (uint8_t t_x = 0; t_x < (train_map_0_WIDTH / 8); t_x++)
            {
                // TODO: figure out why the math failed on NES but not GameBoy
                uint8_t x = TRAIN_MAP_BG_X_START + _car_x[car][tile];
                uint8_t y = TRAIN_MAP_BG_Y_START + t_y;
                set_bkg_tile_xy(x, y, TRAIN_MAP_CAR_TILE_START + tile);
                tile++;
            }
        }
    }
}

#define PLAYER_MAP_DATA_START 0
#define PLAYER_MAP_SPRITE_START 0
void initialize_players_map(void)
{
    set_sprite_data(PLAYER_MAP_DATA_START, train_player_logo_0_TILE_COUNT, train_player_logo_0_tiles);
    set_sprite_tile(PLAYER_MAP_SPRITE_START + 0, GET_8x16_SPRITE_TILE(0));
    set_sprite_data(PLAYER_MAP_DATA_START + 2, train_player_logo_1_TILE_COUNT, train_player_logo_1_tiles);
    set_sprite_tile(PLAYER_MAP_SPRITE_START + 1, GET_8x16_SPRITE_TILE(2));
    set_sprite_data(PLAYER_MAP_DATA_START + 4, train_player_logo_2_TILE_COUNT, train_player_logo_2_tiles);
    set_sprite_tile(PLAYER_MAP_SPRITE_START + 2, GET_8x16_SPRITE_TILE(4));
    set_sprite_data(PLAYER_MAP_DATA_START + 6, train_player_logo_3_TILE_COUNT, train_player_logo_3_tiles);
    set_sprite_tile(PLAYER_MAP_SPRITE_START + 3, GET_8x16_SPRITE_TILE(6));
}

// 4 pixels is the width of the map sprite so it can only travel 0-20
// >>> ",".join(map(str, [round(i*(24-4)/152) for i in range(152)] ))
const uint8_t car_ratio_lookup_table[152] = {
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20};

uint8_t approx_car_ratio_lookup(uint8_t x)
{
    if (x >= 151)
    {
        return car_ratio_lookup_table[151];
    }
    return car_ratio_lookup_table[x];
}

void draw_players_map(struct game_state *state)
{
    // all players logo
    for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
    {
        uint8_t car_x_start = TRAIN_MAP_BG_X_START * 8 + (state->player_positions[player].car * 24);
        uint8_t car_x_ratio = approx_car_ratio_lookup(state->player_positions[player].x);
        // sprites were originally design to fit in a single 8x8 square
        // lets keep the sprites that way and adjust in code
        uint8_t shifted_y = player == 0 ? 0 : player == 1 ? 4
                                          : player == 2   ? 4
                                                          : 8;
        uint8_t shifted_x = (player == 0 || player == 2) ? 0 : -4;
        move_sprite(PLAYER_MAP_SPRITE_START + player, PLATFORM_X_ADJUST + car_x_start + car_x_ratio + shifted_x, PLATFORM_Y_ADJUST + shifted_y);
    }
}

#define CUSTOMER_STATUS_TILE_START (TRAIN_MAP_CAR_TILE_START + TILES_PER_CAR)
#define CUSTOMER_STATUS_START_X 13
#define CUSTOMER_STATUS_START_Y 0
#define CUSTOMER_STATUS_SPRITE_WIDTH_TILES (4 * 4 * 3)
#define CUSTOMER_STATUS_SPRITE_SHEET_WIDTH (CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 6)
void init_customer_status(void)
{
    uint8_t tile = 0;
    for (uint8_t t_y = 0; t_y < (2); t_y++)
    {
        for (uint8_t t_x = 0; t_x < (3); t_x++)
        {
            uint8_t x = CUSTOMER_STATUS_START_X + t_x;
            uint8_t y = CUSTOMER_STATUS_START_Y + t_y;
            set_bkg_tile_xy(x, y, CUSTOMER_STATUS_TILE_START + tile);
            tile++;
        }
    }
}
uint8_t last_customer_status;
void draw_customer_status(struct game_state *state)
{
    if (last_customer_status == state->round_score)
    {
        return;
    }
    last_customer_status = state->round_score;
    uint16_t customer_sprite_starts[] = {
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 0,
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 1,
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 2,
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 3,
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 4,
        CUSTOMER_STATUS_SPRITE_WIDTH_TILES * 5,
    };
    uint8_t sprite_starts_index = state->round_score;
    if (state->round_score > 5)
    {
        sprite_starts_index = 5;
    }
    uint16_t tile_start = customer_sprite_starts[sprite_starts_index];
    set_bkg_data(CUSTOMER_STATUS_TILE_START, 3, &customer_status_tiles[tile_start]);
    set_bkg_data(CUSTOMER_STATUS_TILE_START + 3, 3, &customer_status_tiles[tile_start + CUSTOMER_STATUS_SPRITE_SHEET_WIDTH]);
}

#define ROUND_PROGRESS_TILE_START (CUSTOMER_STATUS_TILE_START + 9)
#define ROUND_PROGRESS_START_X 16
#define ROUND_PROGRESS_START_Y 0
#define ROUND_PROGRESS_SPRITE_WIDTH_TILES 3
#define ROUND_PROGRESS_SPRITE_START(sprite_index) (ROUND_PROGRESS_SPRITE_WIDTH_TILES * 4 * 4 * sprite_index)
// >>> "),ROUND_PROGRESS_SPRITE_START(".join(map(str, [round(i*(19/102)) for i in range(101)] ))
const uint16_t distance_sprite_starts[ROUND_DISTANCE] = {
    ROUND_PROGRESS_SPRITE_START(0),
    ROUND_PROGRESS_SPRITE_START(0),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(1),
    ROUND_PROGRESS_SPRITE_START(2),
    ROUND_PROGRESS_SPRITE_START(2),
    ROUND_PROGRESS_SPRITE_START(2),
    ROUND_PROGRESS_SPRITE_START(2),
    ROUND_PROGRESS_SPRITE_START(2),
    ROUND_PROGRESS_SPRITE_START(3),
    ROUND_PROGRESS_SPRITE_START(3),
    ROUND_PROGRESS_SPRITE_START(3),
    ROUND_PROGRESS_SPRITE_START(3),
    ROUND_PROGRESS_SPRITE_START(3),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(4),
    ROUND_PROGRESS_SPRITE_START(5),
    ROUND_PROGRESS_SPRITE_START(5),
    ROUND_PROGRESS_SPRITE_START(5),
    ROUND_PROGRESS_SPRITE_START(5),
    ROUND_PROGRESS_SPRITE_START(5),
    ROUND_PROGRESS_SPRITE_START(6),
    ROUND_PROGRESS_SPRITE_START(6),
    ROUND_PROGRESS_SPRITE_START(6),
    ROUND_PROGRESS_SPRITE_START(6),
    ROUND_PROGRESS_SPRITE_START(6),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(7),
    ROUND_PROGRESS_SPRITE_START(8),
    ROUND_PROGRESS_SPRITE_START(8),
    ROUND_PROGRESS_SPRITE_START(8),
    ROUND_PROGRESS_SPRITE_START(8),
    ROUND_PROGRESS_SPRITE_START(8),
    ROUND_PROGRESS_SPRITE_START(9),
    ROUND_PROGRESS_SPRITE_START(9),
    ROUND_PROGRESS_SPRITE_START(9),
    ROUND_PROGRESS_SPRITE_START(9),
    ROUND_PROGRESS_SPRITE_START(9),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(10),
    ROUND_PROGRESS_SPRITE_START(11),
    ROUND_PROGRESS_SPRITE_START(11),
    ROUND_PROGRESS_SPRITE_START(11),
    ROUND_PROGRESS_SPRITE_START(11),
    ROUND_PROGRESS_SPRITE_START(11),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(12),
    ROUND_PROGRESS_SPRITE_START(13),
    ROUND_PROGRESS_SPRITE_START(13),
    ROUND_PROGRESS_SPRITE_START(13),
    ROUND_PROGRESS_SPRITE_START(13),
    ROUND_PROGRESS_SPRITE_START(13),
    ROUND_PROGRESS_SPRITE_START(14),
    ROUND_PROGRESS_SPRITE_START(14),
    ROUND_PROGRESS_SPRITE_START(14),
    ROUND_PROGRESS_SPRITE_START(14),
    ROUND_PROGRESS_SPRITE_START(14),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(15),
    ROUND_PROGRESS_SPRITE_START(16),
    ROUND_PROGRESS_SPRITE_START(16),
    ROUND_PROGRESS_SPRITE_START(16),
    ROUND_PROGRESS_SPRITE_START(16),
    ROUND_PROGRESS_SPRITE_START(16),
    ROUND_PROGRESS_SPRITE_START(17),
    ROUND_PROGRESS_SPRITE_START(17),
    ROUND_PROGRESS_SPRITE_START(17),
    ROUND_PROGRESS_SPRITE_START(17),
    ROUND_PROGRESS_SPRITE_START(17),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(18),
    ROUND_PROGRESS_SPRITE_START(19),
};
void init_round_progress(void)
{
    uint8_t tile = 0;
    for (uint8_t t_y = 0; t_y < (1); t_y++)
    {
        for (uint8_t t_x = 0; t_x < (ROUND_PROGRESS_SPRITE_WIDTH_TILES); t_x++)
        {
            uint8_t x = ROUND_PROGRESS_START_X + t_x;
            uint8_t y = ROUND_PROGRESS_START_Y + t_y;
            set_bkg_tile_xy(x, y, ROUND_PROGRESS_TILE_START + tile);
            tile++;
        }
    }
}
uint8_t last_round_distance;
void draw_round_progress(struct game_state *state)
{
    if (last_round_distance == state->current_distance)
    {
        return;
    }
    last_round_distance = state->current_distance;
    uint16_t tile_start = distance_sprite_starts[state->current_distance];
    set_bkg_data(ROUND_PROGRESS_TILE_START, ROUND_PROGRESS_SPRITE_WIDTH_TILES, &map_progress_tiles[tile_start]);
}

#define BG_TRAIN_DATA_START (ROUND_PROGRESS_TILE_START + ROUND_PROGRESS_SPRITE_WIDTH_TILES)
#define BG_START_TILE_X 0
#define BG_START_TILE_Y 2
#define BG_TILE_PLAYER_X(player) ((player == 0 || player == 2) ? 0 : 0)
#define BG_TILE_PLAYER_Y(player) (player == 0 ? 0 : player == 1 ? TRAIN_CAR_HEIGHT / 8     \
                                                : player == 2   ? TRAIN_CAR_HEIGHT / 8 * 2 \
                                                                : TRAIN_CAR_HEIGHT / 8 * 3)
void initialize_bg_train(uint8_t current_player)
{
    set_bkg_data(BG_TRAIN_DATA_START, bg_train_passenger_TILE_COUNT, bg_train_passenger_tiles);
    uint8_t tile = BG_TRAIN_DATA_START;
    for (uint8_t t_y = 0; t_y < (bg_train_passenger_HEIGHT / 8); t_y++)
    {
        for (uint8_t t_x = 0; t_x < (bg_train_passenger_WIDTH / 8); t_x++)
        {
            set_bkg_tile_xy(
                BG_TILE_PLAYER_X(current_player) + BG_START_TILE_X + t_x,
                BG_TILE_PLAYER_Y(current_player) + BG_START_TILE_Y + t_y,
                tile);
            tile++;
        }
    }
}

#define TOOL_SPRITE_START 4
#define TOOL_DATA_START 8
#define LOAD_TOOL(name, num)                                                                     \
    set_sprite_data(TOOL_DATA_START + (num * 2), tool_##name##_TILE_COUNT, tool_##name##_tiles); \
    set_sprite_tile(TOOL_SPRITE_START + num + SPRITES_PER_PLAYER * current_player, GET_8x16_SPRITE_TILE(TOOL_DATA_START + (num * 2)));

void initialize_tools(uint8_t current_player)
{
    LOAD_TOOL(wifi, TOOL_WIFI);
    LOAD_TOOL(wrench, TOOL_WRENCH);
    LOAD_TOOL(drink, TOOL_DRINK);
    LOAD_TOOL(cat, TOOL_CAT);
    LOAD_TOOL(music, TOOL_MUSIC);
}

void draw_tools(struct game_state *state, uint8_t current_player)
{
    uint8_t current_car = state->player_positions[current_player].car;
    for (uint8_t tool = 0; tool < TOOL_COUNT; tool++)
    {
        if (!state->tools[tool].unlocked || state->tools[tool].car != current_car)
        {
            hide_sprite(TOOL_SPRITE_START + SPRITES_PER_PLAYER * current_player + tool);
            continue;
        }
        if (state->tools[tool].player_holding != PLAYER_HOLDING_NONE)
        {
            // place tool in front of player
            if (state->player_positions[state->tools[tool].player_holding].direction == DIRECTION_LEFT)
            {
                state->tools[tool].x = state->player_positions[state->tools[tool].player_holding].x - 8;
            }
            else if (state->player_positions[state->tools[tool].player_holding].direction == DIRECTION_RIGHT)
            {
                state->tools[tool].x = state->player_positions[state->tools[tool].player_holding].x + 8;
            }
            // TODO: picking up tool behind you does not place in front (match the player sprite flag instead of direction)
            // place tool slightly above the ground/player
            state->tools[tool].y = state->player_positions[state->tools[tool].player_holding].y - TOOL_RAISE_PIXELS;
        }
        move_sprite(TOOL_SPRITE_START + SPRITES_PER_PLAYER * current_player + tool,
                    PLATFORM_X_ADJUST + PLAYER_X_ADJUST(current_player) + state->tools[tool].x,
                    PLATFORM_Y_ADJUST + PLAYER_Y_ADJUST(current_player) + state->tools[tool].y);
    }
}

#define PLAYER_DATA_START (TOOL_DATA_START + TOOL_COUNT * 2)
#define PLAYER_SPRITE_START (TOOL_SPRITE_START + TOOL_COUNT)
void initialize_players(uint8_t current_player)
{
    for (uint8_t player_of = 0; player_of < MAX_PLAYABLES; player_of++)
    {
        set_sprite_tile(PLAYER_SPRITE_START + player_of + SPRITES_PER_PLAYER * current_player, GET_8x16_SPRITE_TILE(PLAYER_DATA_START + player_of * 2));
    }
}

#define SP_CONDUCTOR_FRAME_TILE_COUNT 2
#define PLAYER_ANIMATION_CONST(current_player, player_of)                                \
    {                                                                                    \
        .sp_idx = PLAYER_SPRITE_START + player_of + SPRITES_PER_PLAYER * current_player, \
        .data_idx = PLAYER_DATA_START + SP_CONDUCTOR_FRAME_TILE_COUNT * player_of,       \
        .frames = 3,                                                                     \
        .neutral_frame = 1,                                                              \
        .data_tile_count = SP_CONDUCTOR_FRAME_TILE_COUNT,                                \
        .frame_wait = 8,                                                                 \
        .frame_datas = {                                                                 \
            &conductor_##player_of##_tiles[0 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],   \
            &conductor_##player_of##_tiles[1 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],   \
            &conductor_##player_of##_tiles[2 * SP_CONDUCTOR_FRAME_TILE_COUNT * 4 * 4],   \
        }                                                                                \
    }

// TODO: adjust this only require MAX_PLAYERS
const struct animation_const player_animations_const[STRUCT_MAX_PLAYABLES][STRUCT_MAX_PLAYABLES] = {
    {
        PLAYER_ANIMATION_CONST(0, 0),
        PLAYER_ANIMATION_CONST(0, 1),
        PLAYER_ANIMATION_CONST(0, 2),
        PLAYER_ANIMATION_CONST(0, 3),
    },
    {
        PLAYER_ANIMATION_CONST(1, 0),
        PLAYER_ANIMATION_CONST(1, 1),
        PLAYER_ANIMATION_CONST(1, 2),
        PLAYER_ANIMATION_CONST(1, 3),
    },
    {
        PLAYER_ANIMATION_CONST(2, 0),
        PLAYER_ANIMATION_CONST(2, 1),
        PLAYER_ANIMATION_CONST(2, 2),
        PLAYER_ANIMATION_CONST(2, 3),
    },
    {
        PLAYER_ANIMATION_CONST(3, 0),
        PLAYER_ANIMATION_CONST(3, 1),
        PLAYER_ANIMATION_CONST(3, 2),
        PLAYER_ANIMATION_CONST(3, 3),
    },
};

void draw_players(struct game_state *state, uint8_t current_player)
{
    for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
    {
        // do not render players that are not in our car
        if (state->player_positions[player].car != state->player_positions[current_player].car)
        {
            hide_sprite(player_animations_const[current_player][player].sp_idx);
            continue;
        }
        // always move sprite to make it visible
        move_sprite(
            player_animations_const[current_player][player].sp_idx,
            PLAYER_X_ADJUST(current_player) + PLATFORM_X_ADJUST + state->player_positions[player].x,
            PLAYER_Y_ADJUST(current_player) + PLATFORM_Y_ADJUST + state->player_positions[player].y);
        // progress animation
        if (state->player_positions[player].direction < 0)
        {
            animation_progress(&player_animations_const[current_player][player], &state->player_animations[player]);
            set_sprite_prop(player_animations_const[current_player][player].sp_idx, S_FLIPX);
        }
        else if (state->player_positions[player].direction > 0)
        {
            animation_progress(&player_animations_const[current_player][player], &state->player_animations[player]);
            set_sprite_prop(player_animations_const[current_player][player].sp_idx, 0);
        }
        else
        {
            animation_idle(&player_animations_const[current_player][player], &state->player_animations[player]);
        }
    }
}

#define TASK_DATA_START (BG_TRAIN_DATA_START + bg_train_passenger_TILE_COUNT)
// fewer players == fewer copies of sprites
#define TASK_SPRITE_START (PLAYER_SPRITE_START + MAX_PLAYABLES)
uint8_t tool_modifier_0[2 * 4 * 4];
uint8_t tool_modifier_1[2 * 4 * 4];
void intialize_tasks(void)
{
    set_bkg_data(TASK_DATA_START, tasks_TILE_COUNT, tasks_tiles);
}

// >>> ",".join(map(str, [8-round(i*(8/120)) for i in range(121)] ))
uint8_t task_progress_lookup_table[TASK_PROGRESS_INIT + 1] = {
    8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t task_sprite_modifier_frame = 0;
struct coordinate
{
    uint8_t x;
    uint8_t y;
};
#define UPPER_ROW_Y (BG_START_TILE_Y)
#define LOWER_ROW_Y (UPPER_ROW_Y + 4)
#define TASK_UPPER_ROW(task) {.x = 5 + task * 2, .y = UPPER_ROW_Y}
#define TASK_LOWER_ROW(task) \
    {                        \
        .x = 6 + task * 2, .y = LOWER_ROW_Y}
struct coordinate task_slot_x_y[TASK_SLOTS_PER_CAR] = {
    TASK_UPPER_ROW(0),
    TASK_UPPER_ROW(1),
    TASK_UPPER_ROW(2),
    TASK_UPPER_ROW(3),
    TASK_LOWER_ROW(0),
    TASK_LOWER_ROW(1),
    TASK_LOWER_ROW(2),
    TASK_LOWER_ROW(3),
};
void maybe_undraw_task_for_player(struct game_state *state, uint8_t car, uint8_t t_i, uint8_t current_player)
{
    if (state->player_positions[current_player].car != car)
    {
        return;
    }
#define TASK_COORDINATE task_slot_x_y[t_i]
    uint8_t tile = 0;
    if (TASK_COORDINATE.y == UPPER_ROW_Y)
    {
        tile = BG_TRAIN_DATA_START;
    }
    set_bkg_tile_xy(
        PLAYER_X_ADJUST_TILE(current_player) + TASK_COORDINATE.x,
        PLAYER_Y_ADJUST_TILE(current_player) + TASK_COORDINATE.y,
        tile);
    set_bkg_tile_xy(
        PLAYER_X_ADJUST_TILE(current_player) + TASK_COORDINATE.x + 1,
        PLAYER_Y_ADJUST_TILE(current_player) + TASK_COORDINATE.y,
        tile);
}
void maybe_undraw_task_for_all_players(struct game_state *state, uint8_t car, uint8_t t_i)
{
    for (uint8_t current_player = 0; current_player < MAX_PLAYERS; current_player++)
    {
        maybe_undraw_task_for_player(state, car, t_i, current_player);
    }
}
void draw_tasks(struct game_state *state, uint8_t current_player)
{
    uint8_t car = state->player_positions[current_player].car;
    if (state->player_car_changed[current_player])
    {
        for (uint8_t t_i = 0; t_i < TASK_SLOTS_PER_CAR; t_i++)
        {
            maybe_undraw_task_for_player(state, car, t_i, current_player);
        }
    }
    // set tiles
    for (uint8_t t_i = 0; t_i < TASK_SLOTS_PER_CAR; t_i++)
    {
        if (state->tasks[car][t_i].progress > 0)
        {
            set_bkg_tile_xy(
                PLAYER_X_ADJUST_TILE(current_player) + TASK_COORDINATE.x,
                PLAYER_Y_ADJUST_TILE(current_player) + TASK_COORDINATE.y,
                TASK_DATA_START + state->tasks[car][t_i].tool);
            set_bkg_tile_xy(
                PLAYER_X_ADJUST_TILE(current_player) + TASK_COORDINATE.x + 1,
                PLAYER_Y_ADJUST_TILE(current_player) + TASK_COORDINATE.y,
                TASK_DATA_START + 6 + task_progress_lookup_table[state->tasks[car][t_i].progress]);
        }
    }
    // swap task sprite modifier
    task_sprite_modifier_frame = (task_sprite_modifier_frame + 1) % 2;
}

void npc_replace_input(struct game_state *state, uint8_t npc)
{
    // change directions
    if (state->player_positions[npc].x >= TRAIN_CAR_LEN - 8)
    {
        state->player_positions[npc].direction = DIRECTION_LEFT;
    }
    else if (state->player_positions[npc].x <= 8)
    {
        state->player_positions[npc].direction = DIRECTION_RIGHT;
        // state->player_positions[npc].direction = 0;
    }

    // simulate directional movement
    if (state->player_positions[npc].direction == DIRECTION_RIGHT)
    {
        joypads.joypads[npc] = J_RIGHT;
    }
    else if (state->player_positions[npc].direction == DIRECTION_LEFT)
    {
        joypads.joypads[npc] = J_LEFT;
    }
    else
    {
        joypads.joypads[npc] = 0;
    }
}

#define TASK_MARGIN 12
void handle_task_progress(struct game_state *state)
{
    for (enum TOOL tool = 0; tool < TOOL_COUNT; tool++)
    {
        // skip inactive tools
        if (!state->tools[tool].unlocked || state->tools[tool].player_holding == PLAYER_HOLDING_NONE)
        {
            continue;
        }
        uint8_t car = state->tools[tool].car;
        // for each task in the tools car
        // TODO: maybe cache x and/or y lookup to only perform 3 checks instead of TASK_SLOTS_PER_CAR

        uint8_t tool_on_upper_level = state->tools[tool].y < TRAIN_UPPER_FLOOR;
        uint8_t tool_on_lower_level = state->tools[tool].y >= TRAIN_FLOOR_BASELINE;
        for (uint8_t t_i = 0; t_i < TASK_SLOTS_PER_CAR; t_i++)
        {
            if (state->tasks[car][t_i].tool == tool && state->tasks[car][t_i].progress > 0)
            {
                // tool close to task
                uint8_t task_on_upper_level = task_slot_x_y[t_i].y == UPPER_ROW_Y;
                uint8_t task_on_lower_level = task_slot_x_y[t_i].y == LOWER_ROW_Y;
                if ((tool_on_upper_level && task_on_upper_level) || (tool_on_lower_level && task_on_lower_level))
                {
                    state->tasks[car][t_i].progress -= 1;
                    // handle completed task
                    if (state->tasks[car][t_i].progress == 0)
                    {
                        state->round_score += 1;
                        state->open_task_count -= 1;
                        maybe_undraw_task_for_all_players(state, car, t_i);
                    }
                    // only handle one tasks progress
                    break;
                }
            }
        }
    }
}

void maybe_create_tasks(struct game_state *state)
{
    if (state->open_task_count >= state->max_open_tasks)
    {
        return;
    }
    // pick random unlocked tool
    uint8_t tool = rand() % TOOL_COUNT;
    uint8_t iters = 0;
    while (!state->tools[tool].unlocked && iters < TOOL_COUNT)
    {
        tool = (tool + 1) % TOOL_COUNT;
        iters++;
    }
    // pick car
    // do not spawn in the same car as the tool (except when cars == 1)
    uint8_t car = rand() % state->cars;
    if (car == state->tools[tool].car)
    {
        car = (car + 1) % state->cars;
    }
    // pick slot
    uint8_t slot = rand() % TASK_SLOTS_PER_CAR;
    for (uint8_t t_i = slot; t_i < slot + TASK_SLOTS_PER_CAR; t_i++)
    {
        uint8_t current_slot = t_i % TASK_SLOTS_PER_CAR;
        if (state->tasks[car][current_slot].progress == 0)
        {
            // create task
            state->tasks[car][current_slot].tool = tool;
            state->tasks[car][current_slot].progress = TASK_PROGRESS_INIT;
            state->open_task_count++;
            return;
        }
    }
    // at max tasks per car
}

uint8_t get_round_tasks(uint8_t round)
{
    return 12 + round * 3;
}
struct game_state default_state = {
    .cars = 4,
    .round = 0,
    .current_distance = 0,
    .current_distance_tick = 0,
    .round_score = 0,
    .max_open_tasks = 8,
    .open_task_count = 8,
#define PLAYER_START_POSITION_LEFT 8
#define PLAYER_START_POSITION_RIGHT (TRAIN_CAR_LEN - 8)
#define PLAYER_START_POSITIONS {                                                             \
    {.car = 0, .x = PLAYER_START_POSITION_LEFT, .y = TRAIN_FLOOR_BASELINE, .direction = 0},  \
    {.car = 0, .x = PLAYER_START_POSITION_RIGHT, .y = TRAIN_FLOOR_BASELINE, .direction = 0}, \
    {.car = 1, .x = PLAYER_START_POSITION_LEFT, .y = TRAIN_FLOOR_BASELINE, .direction = -1}, \
    {.car = 1, .x = PLAYER_START_POSITION_RIGHT, .y = TRAIN_FLOOR_BASELINE, .direction = 1}, \
}
    .player_positions = PLAYER_START_POSITIONS,
#define PLAYER_ANIMATION_INIT {.frame = 1, .direction = 1, .frame_tick = 0}
#define PLAYER_ANIMATIONS_INIT {PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT, PLAYER_ANIMATION_INIT}
    .player_animations = PLAYER_ANIMATIONS_INIT,
#define TOOL_START_POSITION_LEFT 12
#define TOOL_START_POSITION_RIGHT (TRAIN_CAR_LEN - 12)
    .tools = {
        // TOOL_WIFI
        {
            .unlocked = 1,
            .car = 0,
            .x = TOOL_START_POSITION_RIGHT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
        // TOOL_WRENCH
        {
            .unlocked = 1,
            .car = 1,
            .x = TOOL_START_POSITION_LEFT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
        // TOOL_DRINK
        {
            .unlocked = 1,
            .car = 1,
            .x = TOOL_START_POSITION_RIGHT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
        // TOOL_CAT
        {
            .unlocked = 1,
            .car = 2,
            .x = TOOL_START_POSITION_LEFT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
        // TOOL_MUSIC
        {
            .unlocked = 1,
            .car = 2,
            .x = TOOL_START_POSITION_RIGHT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
    },
    .tasks = {
        // car 0
        {
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
            {.progress = TASK_PROGRESS_INIT, .tool = 0},
        },
    },
};

struct game_state starter_state = {
    .round = 0,
    .current_distance = 0,
    .current_distance_tick = 0,
    .max_open_tasks = 2,
    .open_task_count = 0,
    .cars = 1,
    .round_score = 0,
    .player_animations = PLAYER_ANIMATIONS_INIT,
    .player_positions = PLAYER_START_POSITIONS,
    .tools = {
        // TOOL_WIFI
        {
            .unlocked = 1,
            .car = 0,
            .x = TOOL_START_POSITION_RIGHT,
            .y = TRAIN_FLOOR_BASELINE,
            .player_holding = PLAYER_HOLDING_NONE,
        },
    },
};

void init_state(uint8_t round)
{
    state = starter_state;
    state.round = round;
    state.round_distance_ticks = 1;
    state.round_tasks = get_round_tasks(round);
}

void advance_state()
{
    state.round += 1;
    // rounds get longer
    if (1 <= state.round && state.round <= 4)
    {
        state.round_distance_ticks = 4;
    }
    else if (5 <= state.round && state.round <= 8)
    {
        state.round_distance_ticks = 6;
    }
    else
    {
        state.round_distance_ticks = 10;
    }
    state.round_tasks = get_round_tasks(state.round);
    // reset current state
    state.current_distance = 0;
    state.current_distance_tick = 0;
}

void scene_gameplay_init(void)
{
    FILL_BKG_EMPTY;
    // train map
    SWITCH_ROM(BANK(train_map_0));
    initialize_train_map();
    // player map logo
    initialize_players_map();
    intialize_tasks();
    last_customer_status = 254;
    init_customer_status();
    last_round_distance = 254;
    init_round_progress();
    for (uint8_t player = 0; player < MAX_PLAYERS; player++)
    {
        // player sprite
        initialize_players(player);
        // tools
        initialize_tools(player);
        // bg
        initialize_bg_train(player);
    }

    draw_train_map(&state);
}

void scene_gameplay_loop(void)
{
    // handle input
    for (uint8_t npc = MAX_PLAYERS; npc < MAX_PLAYABLES; npc++)
    {
        npc_replace_input(&state, npc);
    }
    for (uint8_t player = 0; player < MAX_PLAYABLES; player++)
    {
        handle_input(&state, player);
    }

    handle_task_progress(&state);
    maybe_create_tasks(&state);

    // render
    vsync();
    draw_players_map(&state);
    draw_customer_status(&state);
    draw_round_progress(&state);
    for (uint8_t player = 0; player < MAX_PLAYERS; player++)
    {
        draw_players(&state, player);
        draw_tools(&state, player);
        draw_tasks(&state, player);
    }
    reset_player_car_changed(&state);

    // distance tick
    if (state.current_distance_tick == 0)
    {
        state.current_distance_tick = state.round_distance_ticks;
        state.current_distance++;
    }
    else
    {
        state.current_distance_tick--;
    }
    if (state.current_distance == ROUND_DISTANCE)
    {
        queue_scene(&scene_upgrade_menu);
    }
}

struct scene scene_gameplay = {
    .init = scene_gameplay_init,
    .loop = scene_gameplay_loop,
};
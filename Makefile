CC	= ../../bin/lcc -Wa-l -Wl-j
ROMUSAGE = ../../bin/romusage
PNG2ASSET = ../../bin/png2asset
GB_BIN	= train-game.gb
NES_BIN = train-game.nes
ASSETS_TOOLS = tool_wifi.png tool_wrench.png tool_drink.png tool_cat.png tool_music.png
ASSETS_PLAYER_LOGOS = train_player_logo_0.png train_player_logo_1.png train_player_logo_2.png train_player_logo_3.png
ASSETS_PER_PLAYER = conductor_0.png conductor_1.png conductor_2.png conductor_3.png player_banner_0.png player_banner_1.png
ASSETS = train_map_0.png train_player_logo_arrow.png bg_train_passenger.png task_sprite_modifier.png tasks.png cursor.png $(ASSETS_PER_PLAYER) $(ASSETS_TOOLS) $(ASSETS_PLAYER_LOGOS)
ASSET_IN = assets
ASSETS_DEP = $(addprefix assets/,$(ASSETS))
PNG2ASSET_OUT = src/gen
ASSETS_OUT = $(addprefix $(PNG2ASSET_OUT)/,$(ASSETS:.png=.c))
SRCS_CORE = input.c scene_gameplay.c scene_tutorial.c scene_main_menu.c scene_upgrade_menu.c main.c
SRCS = $(addprefix src/,$(SRCS_CORE)) $(ASSETS_OUT)

all:	$(GB_BIN) $(NES_BIN)

ASSET_ARGS_COMMON = -noflip -bpp 2 -spr8x16 -keep_palette_order -sprite_no_optimize
ASSET_ARGS_BKG = -noflip -bpp 2 -spr8x8 -keep_palette_order -sprite_no_optimize
ASSET_BANK_ARG = -b 1
$(ASSETS_OUT): $(ASSETS_DEP)
	$(PNG2ASSET) $(ASSET_IN)/cursor.png -o $(PNG2ASSET_OUT)/cursor.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/conductor_0.png -o $(PNG2ASSET_OUT)/conductor_0.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/conductor_1.png -o $(PNG2ASSET_OUT)/conductor_1.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/conductor_2.png -o $(PNG2ASSET_OUT)/conductor_2.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/conductor_3.png -o $(PNG2ASSET_OUT)/conductor_3.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_map_0.png -o $(PNG2ASSET_OUT)/train_map_0.c $(ASSET_ARGS_BKG) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_player_logo_arrow.png -o $(PNG2ASSET_OUT)/train_player_logo_arrow.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_player_logo_0.png -o $(PNG2ASSET_OUT)/train_player_logo_0.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_player_logo_1.png -o $(PNG2ASSET_OUT)/train_player_logo_1.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_player_logo_2.png -o $(PNG2ASSET_OUT)/train_player_logo_2.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/train_player_logo_3.png -o $(PNG2ASSET_OUT)/train_player_logo_3.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tool_wifi.png -o $(PNG2ASSET_OUT)/tool_wifi.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tool_wrench.png -o $(PNG2ASSET_OUT)/tool_wrench.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tool_drink.png -o $(PNG2ASSET_OUT)/tool_drink.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tool_cat.png -o $(PNG2ASSET_OUT)/tool_cat.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tool_music.png -o $(PNG2ASSET_OUT)/tool_music.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/task_sprite_modifier.png -o $(PNG2ASSET_OUT)/task_sprite_modifier.c $(ASSET_ARGS_COMMON) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/bg_train_passenger.png -o $(PNG2ASSET_OUT)/bg_train_passenger.c $(ASSET_ARGS_BKG) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/tasks.png -o $(PNG2ASSET_OUT)/tasks.c $(ASSET_ARGS_BKG) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/player_banner_0.png -o $(PNG2ASSET_OUT)/player_banner_0.c $(ASSET_ARGS_BKG) $(ASSET_BANK_ARG)
	$(PNG2ASSET) $(ASSET_IN)/player_banner_1.png -o $(PNG2ASSET_OUT)/player_banner_1.c $(ASSET_ARGS_BKG) $(ASSET_BANK_ARG)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

$(GB_BIN):	$(SRCS)
	$(CC) -msm83:gb -o $(GB_BIN) $(SRCS)

$(NES_BIN): $(SRCS)
	$(CC) -mmos6502:nes -o $(NES_BIN) $(SRCS)

NES_EMULATOR = C:\Users\jgens\Downloads\Mesen_2.1.1_Windows\Mesen.exe
run-nes:
	$(NES_EMULATOR) $(NES_BIN)

clean:
	rm -f *.o *.lst *.map *.gb *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi $(PNG2ASSET_OUT)/*.c $(PNG2ASSET_OUT)/*.h

usage:
	$(ROMUSAGE) $(GB_BIN)

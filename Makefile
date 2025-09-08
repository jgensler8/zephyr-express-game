CC	= ../../bin/lcc -Wa-l -Wl-m -Wl-j
ROMUSAGE = ../../bin/romusage
PNG2ASSET = ../../bin/png2asset
BIN	= train-game.gb
ASSETS = conductor.png
ASSET_IN = assets
ASSETS_DEP = $(addprefix assets/,$(ASSETS))
PNG2ASSET_OUT = src/gen
ASSETS_OUT = $(addprefix $(PNG2ASSET_OUT)/,$(ASSETS:.png=.c))
SRCS_CORE = main.c
SRCS = $(addprefix src/,$(SRCS_CORE)) $(ASSETS_OUT)

all:	$(BIN)

$(ASSETS_OUT): $(ASSETS_DEP)
	$(PNG2ASSET) $(ASSET_IN)/conductor.png -o $(PNG2ASSET_OUT)/conductor.c -noflip -bpp 2 -spr8x16 -max_palettes 3 -sprite_no_optimize -b 1

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

$(BIN):	$(SRCS)
	$(CC) -o $(BIN) $(SRCS)

clean:
	rm -f *.o *.lst *.map *.gb *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi $(PNG2ASSET_OUT)/*.c $(PNG2ASSET_OUT)/*.h

usage:
	$(ROMUSAGE) $(BIN)
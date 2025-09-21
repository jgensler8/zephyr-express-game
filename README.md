# Zephyr Express

This was built for the GBJAM 16.

# Platforms

This game targets Game Boy (Solo) and NES (2 Player Co-Op).

As it is built with GBDK, it may also support other platforms with a few more changes.

# Compiling

You'll need Aseprite to generate PNGs for the assets.

Then, you'll need to update some of the constants in the Makefile (`GBDK_HOME`, etc)

Finally, you should just need to run `make zephyr-express.gb` or `make zephyr-express.nes`
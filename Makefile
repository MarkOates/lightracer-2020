


ALLEGRO_DIR=/Users/markoates/Repos/allegro5/include/

PROGRAM_SOURCES := $(shell find src -name '*.cpp')




ALLEGRO_LIBS_MAIN=allegro_color allegro_font allegro_ttf allegro_memfile allegro_dialog allegro_audio allegro_acodec allegro_primitives allegro_image allegro allegro_main
ALLEGRO_LIBS_LINK_MAIN_ARGS := $(ALLEGRO_LIBS_MAIN:%=-l%)




bin/programs/lightracermax:
	g++ -std=c++17 -Wno-everything $(PROGRAM_SOURCES) $(ALLEGRO_LIBS_LINK_MAIN_ARGS) -o bin/programs/lightracermax -I./include




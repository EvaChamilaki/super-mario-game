#include <iostream>
#include <fstream>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include "csv_parser.h"
#include <stdlib.h>
#include <string>
#include <iterator>
#include <map>
#include "DisplayTools.h"
#include "Structs.h"
#include "Config.h"
#include "Map.h"

#define TILE_SIZE 16

void DrawCSV(string name, std::map<int, ALLEGRO_BITMAP *> tiles, ALLEGRO_BITMAP* target) {
	CSVParser csvp;
	csvp.CsvFileToBitmap(name);
	ifstream _file(name);
	string index = "";
	int i = 0;
	int j = 0;
	al_set_target_bitmap(target);
	while (std::getline(_file, index, ',')) {
		if (stoi(index) != -1)
		{
			al_draw_bitmap(tiles[stoi(index)], TILE_SIZE * j, TILE_SIZE * i, 0);
		}

		if (index.find("\n") != std::string::npos) {
			i++;
			j = 0;
		}
		else {
			j++;
		}
	}
}

int main()
{
	int a;

	cfg = Config();

	al_init();
	al_init_image_addon();
	al_init_font_addon();
	
	std::map<int, ALLEGRO_BITMAP *> tiles;

	ALLEGRO_DISPLAY *display= al_create_display(640, 480);;

	Map mFirstStage(cfg.GetConfig(cfg.GetConfig("config/game.json")["maps"][0]["cfg"]));


	//allegro_bitmap *bm = al_load_bitmap("super_mario_sprite_sheet.png");

	////std::cout << "height: " << al_get_bitmap_height(bm) << " width: " << al_get_bitmap_width(bm);
	//int counter = 0;
	//for (int i = 0; i < al_get_bitmap_height(bm) / tile_size; i++) {
	//	for (int j = 0; j < al_get_bitmap_width(bm) / tile_size; j++) {
	//		tiles.insert({counter, al_create_sub_bitmap(bm, j * tile_size, i * tile_size, tile_size, tile_size)});
	//		counter++;
	//	}
	//}



	Bitmap *map = al_create_bitmap(100 * TILE_SIZE, 100 * TILE_SIZE);
	DrawCSV("first_stage_Background.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Foreground.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Clouds.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Blocks.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Sea.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Castle.csv", mFirstStage.GetTiles(), map);
	DrawCSV("first_stage_Coins.csv", mFirstStage.GetTiles(), map);

	//BlitDisplay(display, 0, 0, map, 0, 100*16 - 480, 640, 460);


	for (int i = 0; i < 100; i++) {
		Blit(display, 0, 0, map, 0 + i * 16, 100 * 16 - 480, 640, 480);
		al_rest(0.1);
	}

	al_destroy_display(display);


	std::cout << cfg.GetConfig("config/game.json")["windowName"] << std::endl;
	std::cin >> a;

	return 0;
}
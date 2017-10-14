#include "fxlib.h"
#include "revolution.h"
#include "gamelib.h"
#include <stdio.h>

//The address at which the additional RAM of the 9860gii is.
//Not used by other official apps.
#define GAME_RAM 0x88040000

int AddIn_main(int isAppli, unsigned short OptionNum)
{
	sprite_t *wall;
	sprite_t *wall2;
	sprite_t *player;
	sprite_t *enemy;
	sprite_t *ghost;

	entity_t *entity1;
	entity_t *entity2;

	int ppx = 8;
	int ppy = 8;

	int kc1 = 0;
	int kc2 = 0;
	short unused;
	unsigned char running = 1;

	char buffer[21];

	Bdisp_AllClr_DDVRAM();

	// Bdisp_SetPoint_DDVRAM(0,0,1);

	game_initialize();

	// Bdisp_SetPoint_DDVRAM(2,0,1);

	wall = (sprite_t *)game_cmalloc(sizeof(sprite_t), MEM_EMPTY);
	wall->data[0] = 0xBB;
	wall->data[1] = 0xDD;
	wall->data[2] = 0xA3;
	wall->data[3] = 0x7A;
	wall->data[4] = 0xB5;
	wall->data[5] = 0xCD;
	wall->data[6] = 0xB1;
	wall->data[7] = 0xBB;

	wall2 = (sprite_t *)game_cmalloc(sizeof(sprite_t), MEM_EMPTY);
	wall2->data[0] = 0xF7;
	wall2->data[1] = 0x3A;
	wall2->data[2] = 0xDD;
	wall2->data[3] = 0xA0;
	wall2->data[4] = 0x7F;
	wall2->data[5] = 0x18;
	wall2->data[6] = 0xDD;
	wall2->data[7] = 0x14;

	// Bdisp_SetPoint_DDVRAM(4,0,1);

	player = (sprite_t *)game_cmalloc(sizeof(sprite_t), MEM_EMPTY);
	player->data[0] = 0x1C;
	player->data[1] = 0x24;
	player->data[2] = 0x7E;
	player->data[3] = 0xA5;
	player->data[4] = 0x81;
	player->data[5] = 0x42;
	player->data[6] = 0x7E;
	player->data[7] = 0x66;

	enemy = (sprite_t *)game_cmalloc(sizeof(sprite_t), MEM_EMPTY);
	enemy->data[0] = 0x3c;
	enemy->data[1] = 0x7e;
	enemy->data[2] = 0xdb;
	enemy->data[3] = 0x99;
	enemy->data[4] = 0xff;
	enemy->data[5] = 0x66;
	enemy->data[6] = 0x24;
	enemy->data[7] = 0x3c;

	ghost = (sprite_t *)game_cmalloc(sizeof(sprite_t), MEM_EMPTY);
	/*Generated from sprite */
	ghost->data[0] = 0x3e;
	ghost->data[1] = 0x7f;
	ghost->data[2] = 0xe6;
	ghost->data[3] = 0x7f;
	ghost->data[4] = 0xdb;
	ghost->data[5] = 0x08;
	ghost->data[6] = 0xa2;
	ghost->data[7] = 0x26;

	// Bdisp_SetPoint_DDVRAM(6,0,1);

	// Bdisp_SetPoint_DDVRAM(8,0,1);

	// mapcache_empty();
	mapchunk_empty(&g_mapcache->cache[0]);
	g_mapcache->cache[0].data[0] = 1;
	g_mapcache->cache[0].data[1] = 1;
	g_mapcache->cache[0].data[2] = 1;
	g_mapcache->cache[0].data[13] = 1;
	g_mapcache->cache[0].data[14] = 1;
	g_mapcache->cache[0].data[15] = 1;
	g_mapcache->cache[0].data[16] = 1;
	g_mapcache->cache[0].data[31] = 1;
	g_mapcache->cache[0].data[32] = 1;
	g_mapcache->cache[0].data[47] = 1;
	g_mapcache->cache[0].data[80] = 1;
	g_mapcache->cache[0].data[95] = 1;
	g_mapcache->cache[0].data[96] = 1;
	g_mapcache->cache[0].data[111] = 1;
	g_mapcache->cache[0].data[112] = 1;
	g_mapcache->cache[0].data[113] = 1;
	g_mapcache->cache[0].data[114] = 1;
	g_mapcache->cache[0].data[125] = 1;
	g_mapcache->cache[0].data[126] = 1;
	g_mapcache->cache[0].data[127] = 1;

	g_mapcache->cache[0].data[36] = 2;
	g_mapcache->cache[0].data[43] = 2;
	g_mapcache->cache[0].data[54] = 2;
	g_mapcache->cache[0].data[57] = 2;
	g_mapcache->cache[0].data[70] = 2;
	g_mapcache->cache[0].data[73] = 2;
	g_mapcache->cache[0].data[84] = 2;
	g_mapcache->cache[0].data[91] = 2;

	// Bdisp_SetPoint_DDVRAM(10,0,1);

	g_sprite_entity_list[0] = player;
	g_sprite_entity_list[1] = enemy;
	g_sprite_entity_list[2] = ghost;
	g_sprite_tile_list[1] = wall2;
	g_sprite_tile_list[2] = wall;

	// Draw_Mapchunk(g_mapcache[DIRECTION_MIDDLE]);

	g_player->character.x = 0x1000;
	g_player->character.y = 0x1000;
	g_player->character.speed = 512;

	entity1->speed = 256;
	entity1->x = 0x2000;
	entity1->y = 0x2000;
	entity1->sprite_i = 1;
	entity1->ai_flags = AI_MOVE_FLEE | 0x0500;
	entity_add(entity1);

	entity2->speed = 256;
	entity2->x = 0x2000;
	entity2->y = 0x2000;
	entity2->sprite_i = 2;
	entity2->ai_flags = AI_MOVE_PURSUE | 0x0500;
	entity_add(entity2);

	// Draw_Entity(entity);

	load_game();

	game_run();

	return 1;
}


//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

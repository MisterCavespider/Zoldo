#ifndef _GAMELIB_H
#define _GAMELIB_H

#include "stddef.h"
#include "revolution.h"
#include "gamedefs.h"

#define MEM_START	0x88040000
#define MEM_IGNORE	0x0
#define MEM_EMPTY	0x1

#define DIRECTION_MIDDLE	0x0
#define DIRECTION_NORTH		0x1
#define DIRECTION_EAST		0x2
#define DIRECTION_SOUTH		0x3
#define DIRECTION_WEST		0x4

#define AI_MOVE_WANDER		0x0000
#define AI_MOVE_PURSUE		0x1000
#define AI_MOVE_FLEE		0x2000
#define AI_MOVE				0xF000
#define AI_ARG0				0x0F00
#define AI_ARG1				0x00F0
#define AI_ARG2				0x000F

/* STRUCTS */
typedef struct	player		player_t;
typedef struct	mapcache	mapcache_t;

struct player {
	entity_t character;
	/*
	These values describe the position of the chunk compared to the others

	Can store at most 0xFF and 0xFF == a map with 256 by 256 chunks
	*/
	unsigned char map_x,map_y;
};

struct mapcache {
	mapchunk_t cache[5];

	unsigned char sizes;	//0xF0 | 0x0F
	entity_t entities[8];	//8 entities (loaded)
};

/* EXTERNALS */
extern unsigned char *g_vram;					//where the VRAM is on the 9860gii
extern sprite_t *g_sprite_tile_list;			//all tile sprites; used to draw chunks
extern sprite_t *g_sprite_entity_list;			//all sprites; used to draw chunks
extern sprite_t *g_sprite_projectile_list;		//all sprites; used to draw chunks
extern mapcache_t *g_mapcache;					//all currently loaded maps
extern player_t *g_player;						//the player

void sprite_register(sprite_t *sp, unsigned char i);

/* INITS */
void globals_initialize();	//initializes extern values
void game_initialize();		//initializes game values & globals
void mapcache_init();

/* LOAD */
void load_game();
void load_mapchunk(game_header_t *header, size_t coffset, unsigned char x, unsigned char y, mapchunk_t *store);

/* GAME */
void *game_cmalloc(size_t size, unsigned char calloc);

void game_run();
void game_stop();

void game_process_entities();
void game_process_projectiles();
void game_process_input(int *kc1, int* kc2, short *unused);

/* ENTITIES */
char entity_add(entity_t* e);
void entity_remove(unsigned char index);

void entity_ai(entity_t* e);
void entity_move(entity_t* e, unsigned char direction);
void entity_xmove(entity_t* e, unsigned char direction, int x);

unsigned char entity_collide_map(entity_t *e, unsigned char direction);

/* DRAW */
void Draw_Sprite(sprite_t *sprite, unsigned char x, unsigned char y);
void Draw_Entity(entity_t *entity);
void Draw_EntityState(entity_t *entity, unsigned char state);
void Draw_Mapchunk(mapchunk_t *mc);

#endif

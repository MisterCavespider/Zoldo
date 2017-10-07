#ifndef _GAMELIB_H
#define _GAMELIB_H

#include "stddef.h"
#include "revolution.h"

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
//8x8 - might change
typedef struct sprite {
	unsigned char spritedata[8];
} sprite_t;

typedef struct entity {
	unsigned char sprite_i;

	/*
	Entity dies if hp reaches 0.
	Be careful for underflows.
	*/
	unsigned char hp;
	/*
	0x-0 to 0x-F for atk	(16)
	0x0- to 0xF- for def	(16)
	*/
	unsigned char fight_stats;
	// /*
	// ------11 (0x03) for direction
	// 111111-- (0xFC) for padding
	// */
	// unsigned char move_stats;
	/*
	Tthe use of short allow us to get a precision of 1/512th of a pixel,
	with a minimum speed of 512 seconds per pixel
	*/
	unsigned short x,y;
	unsigned short speed;
	/*
	A bunch of data for the AI.

	1111 ---- ---- ---- for movement
	*/
	unsigned short ai_flags;
} entity_t;

/*
Describes an entity without dynamic parts.
This is saved/loaded in files.
*/
typedef struct entity_proto {
	unsigned char sprite_i;

	unsigned char hp;
	unsigned char fight_stats;
	unsigned short speed;	//also describes for the AI, so it has to be stored
	unsigned ai_flags;
} entity_proto_t;

typedef struct player {
	entity_t character;
	/*
	These values describe the position of the chunck compared to the others

	Can store at most 0xFF and 0xFF == a map with 256 by 256 chuncks
	*/
	unsigned char map_x,map_y;
} player_t;

/*
A single screen
*/
typedef struct mapchunck {
	unsigned char mapdata[128];	//there's 128 tiles
	/*
	These values describe the position of the chunck compared to the others
	*/
	unsigned char map_x,map_y;
} mapchunck_t;

typedef struct mapcache {
	mapchunck_t cache[5];

	unsigned char sizes;	//0xF0 | 0x0F
	entity_t entities[8];	//8 entities (loaded)
} mapcache_t;

void sprite_register_player(sprite_t *sp);
void sprite_register(sprite_t *sp, unsigned char i);
void inflate_proto_entity(entity_proto_t *proto, unsigned short x, unsigned short y, unsigned char move_stats, entity_t *store);
void mapchunck_empty(mapchunck_t *store);
void mapcache_empty();

/* INITS */
void globals_initialize();	//initializes extern values
void game_initialize();		//initializes game values & globals
void mapcache_init();

/* EXTERNALS */
extern unsigned char *g_vram;					//where the VRAM is on the 9860gii
extern sprite_t **g_sprite_tile_list;			//all tile sprites; used to draw chuncks
extern sprite_t **g_sprite_entity_list;			//all sprites; used to draw chuncks
extern sprite_t **g_sprite_projectile_list;		//all sprites; used to draw chuncks
extern mapcache_t *g_mapcache;				//all currently loaded maps
extern player_t *g_player;						//the player

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
void Draw_MapChunck(mapchunck_t *mc);

#endif

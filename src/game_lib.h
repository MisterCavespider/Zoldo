#ifndef _GAMELIB_H
#define _GAMELIB_H

// Includes
#include "stddef.h"
#include "revolution.h"
#include "src/game_defs.h"

// Defines (constants)
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

typedef char*(*sc_cpv)(void);	// Code from the web
const unsigned int sc0135[] = { 0xD201D002, 0x422B0009, 0x80010070, 0x0135 };
#define GetVRAMAddress (*(sc_cpv)sc0135)

// External values (only globals)
game_globals_t *g;

// Timer & Misc
void onIdle();	// Shouldn't be called externally, but it could

// Initializers
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

unsigned char player_onborder();

/* DRAW */
void Draw_Sprite(sprite_t *sprite, unsigned char x, unsigned char y);
void Draw_Entity(entity_t *entity);
void Draw_EntityState(entity_t *entity, unsigned char state);
void Draw_Mapchunk(mapchunk_t *mc);

#endif

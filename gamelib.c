#include "gamelib.h"
#include "stddef.h"
#include "stdlib.h"
#include "revolution.h"
#include "fxlib.h"
#include "timer.h"

void onIdle();
unsigned char to_pixel(unsigned short pos);
unsigned short to_pos(unsigned char pixel);

typedef char*(*sc_cpv)(void);
const unsigned int sc0135[] = { 0xD201D002, 0x422B0009, 0x80010070, 0x0135 };
#define GetVRAMAddress (*(sc_cpv)sc0135)

/* STRUCTS */

void sprite_register(sprite_t *sp, unsigned char i) {
	if(i > 0) {
		g_sprite_tile_list[i] = *sp;
	}
}

/* EXTERNALS */						//<desc>						<alloc>
unsigned char *g_vram;				//for revolution				C
sprite_t *g_sprite_tile_list;		//all tile sprites				ZOLDO
sprite_t *g_sprite_entity_list;		//all entity sprites			ZOLDO
sprite_t *g_sprite_projectile_list;	//all projectile sprites		ZOLDO
mapcache_t *g_mapcache;				//currently loaded map			C
player_t *g_player;					//player + embedded entity		C

unsigned char *vram = 0x00;
int g_filehandle = -1;

/* INITS */
void globals_initialize() {
	unsigned char *failsafe = (unsigned char *)MEM_START;
	*failsafe = 0xFF;
	g_vram = (unsigned char*) GetVRAMAddress();
}

void game_initialize() {
	globals_initialize();

	//only variables that aren't declared when reading a file
	g_player = (player_t *)game_cmalloc(sizeof(player_t), MEM_EMPTY);
	g_mapcache = (mapcache_t *)game_cmalloc(sizeof(mapcache_t) * 4, MEM_EMPTY);

	load_game();
}

/* LOAD */
/**
 * This loads a file ("game.zoldo") from storage as read-only.
 * It will then read a header and all the sprites.
 */
void load_game() {
	size_t read = 0;
	size_t s = 0;		//temporary value; could be anything

	game_header_t *header = (game_header_t *)malloc(sizeof(game_header_t));

	FONTCHARACTER path[]={'\\','\\','f','l','s','0','\\','G','A','M','E','.','z','o','l','d','o', 0};
	g_filehandle = Bfile_OpenFile(path, _OPENMODE_READ);

	s = sizeof(game_header_t);
	Bfile_ReadFile(g_filehandle, header, s, read);
	read += s;

	s = sizeof(sprite_t)*header->sprites_tile_length;
	g_sprite_tile_list = (sprite_t *)game_cmalloc(s, MEM_IGNORE);		//alloc
	Bfile_ReadFile(g_filehandle, g_sprite_tile_list, s, read);			//read
	read += s;

	s = sizeof(sprite_t)*header->sprites_entity_length;
	g_sprite_entity_list = (sprite_t *)game_cmalloc(s, MEM_IGNORE);		//alloc
	Bfile_ReadFile(g_filehandle, g_sprite_entity_list, s, read);		//read
	read += s;

	load_mapchunk(header, read, 0,0, &(g_mapcache->cache[0]));

	// NOT IMPLEMENTED YET
	//
	// s = sizeof(sprite_t)*header->sprites_projectile_length;
	// g_sprite_projectile_list = (sprite_t *)game_cmalloc(s, MEM_IGNORE);	//alloc
	// Bfile_ReadFile(g_filehandle, g_sprite_projectile_list, s, read);	//read
	// read += s;
}

void load_mapchunk(game_header_t *header, size_t coffset, unsigned char x, unsigned char y, mapchunk_t *store) {
	// Calculate offset
	size_t offset = coffset + (x + y * header->mapsize_x) * sizeof(mapchunk_t);
	// Read at offset; store at store
	Bfile_ReadFile(g_filehandle, store, sizeof(mapchunk_t), offset);
	return;
}

void mapcache_init() {

}

/* GAME */
unsigned long usedmemory = 0;
void *game_cmalloc(size_t size, unsigned char c) {
	int i;
	void *r = (void *)(MEM_START + usedmemory);
	if(size % 4 != 0) {
		size += 4-(size % 4);	//make sure it fits 32-bit
	}
	if(usedmemory + size > 40000) {	//more than 256 kB of ram used? -> return MEM_START
		return (void *) MEM_START;
	}

	if(c) {	//empty it
		for(i = 0; i < size; i++) {
			*((char *)(r) + i) = 0x00;
		}
	}	//else don't do anything

	usedmemory += size;
	return r;
}

char running = 0;
char idle = 0;

void game_run() {
	int *kc1 = (int *)malloc(sizeof(int *));
	int *kc2 = (int *)malloc(sizeof(int *));
	short *unused = (short *)malloc(sizeof(short *));

	running = 1;
	onIdle();

	Draw_Mapchunk(g_mapcache->cache + DIRECTION_MIDDLE);
	// Draw_Player(1);

	while(running) {
		if(idle) {
			//TODO TODO

			Draw_EntityState(&g_mapcache->entities[0], 0);
			Draw_EntityState(&g_mapcache->entities[1], 0);
			Draw_EntityState(&g_player->character, 0);

			game_process_entities();
			game_process_input(kc1, kc2, unused);

			if(player_onborder() == DIRECTION_MIDDLE) {
				Draw_EntityState(&g_mapcache->entities[0], 1);
				Draw_EntityState(&g_mapcache->entities[1], 1);
				Draw_EntityState(&g_player->character, 1);
			} else {
				switch (player_onborder()) {
					case DIRECTION_NORTH:
						break;
					case DIRECTION_NORTH:
						break;

				}
			}

			Bdisp_PutDisp_DD();

			idle = 0;
		}
	}
}

void onIdle() {
	if(running && !idle) {
		idle = 1;
		SetTimer(ID_USER_TIMER1, 25, ( void (*)(void) )(&onIdle));
	}
}

void game_stop() {
	running = 0;
}

void game_process_entities() {
	unsigned char i;
	unsigned char entity_count = (g_mapcache->sizes & 0x0F);		//should return lower part of byte

	for(i = 0; i < entity_count; i++) {
		entity_ai(&g_mapcache->entities[i]);
	}
}

void game_process_projectiles() {

}

void game_process_input(int *kc1, int* kc2, short *unused) {
	ClearArea(0, 0, 4, 0, g_vram);

	Bkey_GetKeyWait(kc1, kc2, KEYWAIT_HALTOFF_TIMEROFF, 0, 1, unused);

	if(*kc1 == 0 && *kc2 == 0) {
		Bdisp_SetPoint_DDVRAM(4,0,1);
		return;
	}

	if(*kc1 == 4 && *kc2 == 8) {
		running = 0;
	}
	if(*kc1 == 2 && *kc2 == 9) {
		entity_move(&g_player->character, DIRECTION_NORTH);
	}	//up
	if(*kc1 == 2 && *kc2 == 8) {
		entity_move(&g_player->character, DIRECTION_EAST);
	}	//right
	if(*kc1 == 3 && *kc2 == 8) {
		entity_move(&g_player->character, DIRECTION_SOUTH);
	}	//down
	if(*kc1 == 3 && *kc2 == 9) {
		entity_move(&g_player->character, DIRECTION_WEST);
	}	//left

	*kc1 = 0;
	*kc2 = 0;
	*unused = 0;

}

/* ENTITIES */
char entity_add(entity_t* e) {
	char s = g_mapcache->sizes & 0x0F;
	if(s < 8) {
		g_mapcache->entities[s] = *e;
		g_mapcache->sizes = g_mapcache->sizes | ((g_mapcache->sizes & 0x0F)+1);
		return s;
	} else {
		return -1;
	}
}

void entity_remove(unsigned char index) {

}

void entity_ai(entity_t* e) {
	unsigned short ai_f = e->ai_flags;
	int dx;
	int dy;
	short l;
	char dir = 1;

	dx = g_player->character.x - e->x;
	dy = g_player->character.y - e->y;

	switch(ai_f & AI_MOVE) {
	case AI_MOVE_FLEE:
		dir = -1;
	default:
	case AI_MOVE_PURSUE:
		if(abs(dx) < ((unsigned short)(ai_f&AI_ARG0)*16) && abs(dy) < ((unsigned short)(ai_f&AI_ARG0)*16)) {
			dx = dx * dir;
			dy = dy * dir;

			if(dx > 0) entity_xmove(e, DIRECTION_EAST, dx);
			if(dx < 0) entity_xmove(e, DIRECTION_WEST, dx);

			if(dy > 0) entity_xmove(e, DIRECTION_SOUTH, dy);
			if(dy < 0) entity_xmove(e, DIRECTION_NORTH, dy);

			return;
		}
	case AI_MOVE_WANDER:
		// entity_move(e, DIRECTION_NORTH);
		break;
	}
}

void entity_xmove(entity_t* e, unsigned char direction, int x) {
	if(direction == DIRECTION_MIDDLE || direction > DIRECTION_WEST) {
		return;
	}

	if(x < 0) x = -x;
	if(x > e->speed) x = e->speed;

	switch(direction) {
	case DIRECTION_NORTH:
		if(e->y > 0x0000 && !entity_collide_map(e, direction)) {
			e->y-=x;
		}
		break;
	case DIRECTION_EAST:
		if(e->x < 0xF000 && !entity_collide_map(e, direction)) {
			e->x+=x;
		}
		break;
	case DIRECTION_SOUTH:
		if(e->y < 0x7000 && !entity_collide_map(e, direction)) {
			e->y+=x;
		}
		break;
	case DIRECTION_WEST:
		if(e->x > 0x0000 && !entity_collide_map(e, direction)) {
			e->x-=x;
		}
		break;
	}
}

void entity_move(entity_t* e, unsigned char direction) {
	entity_xmove(e, direction, e->speed);
}

unsigned char entity_collide_map(entity_t *e, unsigned char direction) {
	unsigned char affected_i1;
	unsigned char affected_i2;

	if(direction == DIRECTION_MIDDLE || direction > DIRECTION_WEST) {
		return;
	}

	switch(direction) {
	case DIRECTION_NORTH:
		affected_i1 = ((e->y/512-1)/8)*16 + (e->x/512)/8;
		affected_i2 = ((e->y/512-1)/8)*16 + (e->x/512+7)/8;
		break;
	case DIRECTION_EAST:
		affected_i1 = ((e->y/512)/8)*16 + (e->x/512+8)/8;
		affected_i2 = ((e->y/512+7)/8)*16 + (e->x/512+8)/8;
		break;
	case DIRECTION_SOUTH:
		affected_i1 = ((e->y/512+8)/8)*16 + (e->x/512)/8;
		affected_i2 = ((e->y/512+8)/8)*16 + (e->x/512+7)/8;
		break;
	case DIRECTION_WEST:
		affected_i1 = ((e->y/512)/8)*16 + (e->x/512-1)/8;
		affected_i2 = ((e->y/512+7)/8)*16 + (e->x/512-1)/8;
		break;
	}

	return (g_mapcache->cache[DIRECTION_MIDDLE]).data[affected_i1] != 0 || g_mapcache->cache[DIRECTION_MIDDLE].data[affected_i2] != 0;
}

unsigned char player_onborder() {
	if(g_player->character.x < 0x0000) {
		return DIRECTION_WEST;
	}
	if(g_player->character.x > 0xF000) {
		return DIRECTION_EAST;
	}
	if(g_player->character.y < 0x0000) {
		return DIRECTION_NORTH;
	}
	if(g_player->character.x > 0x7000) {
		return DIRECTION_SOUTH;
	}
	return DIRECTION_MIDDLE;
}

/* DRAW */
unsigned char to_pixel(unsigned short pos) {
	return (unsigned char) (pos/512);
}

unsigned short to_pos(unsigned char pixel) {
	return ((unsigned short)pixel)*512;
}

void Draw_Sprite(sprite_t *sprite, unsigned char x, unsigned char y) {
	//DrawSprite(ppx, ppy, 8, 8, 1, player, vram, NO_TRANS);
	DrawSprite(x, y, 8, 8, 1, sprite->data, g_vram, NO_TRANS);
}

void Draw_Entity(entity_t *entity) {
	Draw_Sprite(&g_sprite_entity_list[(entity->sprite_i < 64) ? entity->sprite_i : 63], entity->x/512, entity->y/512);
}

void Draw_EntityState(entity_t *entity, unsigned char state) {
	if(state) {
		//draw
		Draw_Sprite(&g_sprite_entity_list[entity->sprite_i], entity->x/512, entity->y/512);
	} else {
		//remove player
		ClearArea(entity->x/512, entity->y/512, entity->x/512+7, entity->y/512+7, g_vram);
	}
}

void Draw_Mapchunk(mapchunk_t *mapchunk) {
	unsigned char x,y,map_i, sprite_i;

	for(y = 0; y < 8; y++) {
		for(x = 0; x < 16; x++) {
			map_i = (y*16) + x;
			sprite_i = mapchunk->data[map_i];
			if(sprite_i > 0) {
				Draw_Sprite(&g_sprite_tile_list[sprite_i - 1], x*8, y*8);
			}
		}
	}
}

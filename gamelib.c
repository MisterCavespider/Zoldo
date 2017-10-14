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
void sprite_register_player(sprite_t *sp) {
	g_sprite_tile_list[0] = sp;
}

void sprite_register(sprite_t *sp, unsigned char i) {
	if(i > 0) {
		g_sprite_tile_list[i] = sp;
	}
}

void inflate_proto_entity(entity_proto_t *proto, unsigned short x, unsigned short y, unsigned char move_stats, entity_t *store) {
	if(move_stats == 0x00) {
		move_stats = (move_stats << 4) + DIRECTION_NORTH;
	}

	store->hp = proto->hp;
	store->fight_stats = proto->fight_stats;
	store->x = x;
	store->y = y;
	store->speed = proto->speed;
	store->ai_flags = proto->ai_flags;
}

void mapchunk_empty(mapchunk_t *store) {
	unsigned char i;
	for(i=0; i<128; i++) {
		store->data[i] = 0;
	}
}

/* INITS */
void globals_initialize() {
	unsigned char *failsafe = (unsigned char *)MEM_START;
	*failsafe = 0xFF;
	g_vram = (unsigned char*) GetVRAMAddress();
}

void game_initialize() {
	globals_initialize();

	//should be first in memory
	g_player = (player_t *)game_cmalloc(sizeof(player_t), MEM_EMPTY);

	//should be second in memory
	g_mapcache = (mapcache_t *)game_cmalloc(sizeof(mapcache_t) * 4, MEM_EMPTY);
	mapcache_init();

	//memory location doesn't matter
	g_sprite_tile_list = (sprite_t **)game_cmalloc(sizeof(sprite_t *) * 128, MEM_EMPTY);
	g_sprite_entity_list = (sprite_t **)game_cmalloc(sizeof(sprite_t *) * 64, MEM_EMPTY);
	g_sprite_projectile_list = (sprite_t **)game_cmalloc(sizeof(sprite_t *) * 16, MEM_EMPTY);
}

void mapcache_init() {

}

/* EXTERNALS */
unsigned char *g_vram;
sprite_t **g_sprite_tile_list;			//all tile sprites; used to draw chunks
sprite_t **g_sprite_entity_list;			//all sprites; used to draw chunks
sprite_t **g_sprite_projectile_list;		//all sprites; used to draw chunks
mapcache_t *g_mapcache;
player_t *g_player;

unsigned char *vram = 0x00;
int g_filehandle = -1;

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

	if(c) {
		for(i = 0; i < size; i++) {
			*((char *)(r) + i) = 0x00;
		}
	}

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

			Draw_EntityState(&g_mapcache->entities[0], 1);
			Draw_EntityState(&g_mapcache->entities[1], 1);
			Draw_EntityState(&g_player->character, 1);

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
		Bdisp_SetPoint_DDVRAM(0,0,1);
	}	//up
	if(*kc1 == 2 && *kc2 == 8) {
		entity_move(&g_player->character, DIRECTION_EAST);
		Bdisp_SetPoint_DDVRAM(1,0,1);
	}	//right
	if(*kc1 == 3 && *kc2 == 8) {
		entity_move(&g_player->character, DIRECTION_SOUTH);
		Bdisp_SetPoint_DDVRAM(2,0,1);
	}	//down
	if(*kc1 == 3 && *kc2 == 9) {
		entity_move(&g_player->character, DIRECTION_WEST);
		Bdisp_SetPoint_DDVRAM(3,0,1);
	}	//left

	*kc1 = 0;
	*kc2 = 0;
	*unused = 0;

}

/* LOAD */
void load_game() {
	/*
	* This loads a file ("game.zoldo") from storage as read-only.
	* It will then read a header and all the sprites.
	*/
	int read = 0;
	int i = 0;

	game_header_t *header = (game_header_t *)malloc(sizeof(game_header_t));
	sprite_t *sprite_tile_list;

	FONTCHARACTER path[]={'\\','\\','f','l','s','0','\\','G','A','M','E','.','z','o','l','d','o', 0};
	g_filehandle = Bfile_OpenFile(path, _OPENMODE_READ);

	Bfile_ReadFile(g_filehandle, header, sizeof(game_header_t), 0);

	//allocate a certain amount of mem (specified in header)
	sprite_tile_list = (sprite_t *)game_cmalloc(sizeof(sprite_t)*header->sprites_tile_length, MEM_EMPTY);
	//read the list in the file
	Bfile_ReadFile(g_filehandle, sprite_tile_list, sizeof(sprite_t)*header->sprites_tile_length, 5);

	for(i = 0; i < header->sprites_tile_length; i++) {
		//put all pointers in the global pointer list
		g_sprite_tile_list[i] = &(sprite_tile_list[i]);
	}

	// for(i = 0; i < header->sprites_tile_length; i++) {
	//  	Bfile_ReadFile(
	// 		g_filehandle,
	// 		g_sprite_tile_list + i*sizeof(sprite_t),
	// 		sizeof(sprite_t),
	// 		sizeof(game_header_t) + i*sizeof(sprite_t));
	// }
}

void load_mapchunk(unsigned char x, unsigned char y) {

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

	return g_mapcache->cache[DIRECTION_MIDDLE].data[affected_i1] != 0 || g_mapcache->cache[DIRECTION_MIDDLE].data[affected_i2] != 0;
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
	Draw_Sprite(g_sprite_entity_list[(entity->sprite_i < 64) ? entity->sprite_i : 63], entity->x/512, entity->y/512);
}

void Draw_EntityState(entity_t *entity, unsigned char state) {
	if(state) {
		//draw
		Draw_Sprite(g_sprite_entity_list[entity->sprite_i], entity->x/512, entity->y/512);
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
				Draw_Sprite(g_sprite_tile_list[sprite_i - 1], x*8, y*8);
			}
		}
	}
}

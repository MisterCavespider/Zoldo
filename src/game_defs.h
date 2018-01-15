#ifndef GAMEDEFS_H_
#define GAMEDEFS_H_

typedef struct	sprite				sprite_t;
typedef struct	entity_proto		entity_proto_t;
typedef struct	entity_proto_ref	entity_proto_ref_t;
typedef struct	player				player_t;

typedef struct	map_chunk			map_chunk_t;
typedef struct	map_cache			map_cache_t;

typedef struct	game_header			game_header_t;
typedef struct	game_pack			game_pack_t;

/*
 * Contains everything needed
 * to draw 8x8 sprites.
 *
 * Might later also be of dynamic size.
 */
struct sprite {
	unsigned char		data[8];
};

/*
 * A combination of entity_proto and entity_proto_ref.
 * Contains all data for loaded entities.
 *
 * NOT USED
 * INSTEAD, ENTITY_PROTO_REF + ENTITY_PROTO ARE USED
 */
// struct entity {
// 	unsigned char sprite_i;
//
// 	unsigned char hp;
// 	unsigned char fight_stats;
//
// 	unsigned short x,y;
// 	unsigned short speed;
//
// 	unsigned short ai_flags;
// };

/*
 * Serializable. Contains all immutable data.
 */
struct entity_proto {
	unsigned char		sprite_index;	//'pointer' to sprite

	unsigned char		hp;				//amount og hitpoints left
	unsigned char		fight_stats;	//0xF- for attack, 0x-F for defence

	unsigned short		speed;			//speed in steps; 1 step = 1/512 px

	unsigned short		ai_flags;		//flags/modifiers for the AI
};

/*
 * Reference to the entity prototype.
 * Contains all mutable data.
 *
 * Basically the entity_t
 */
struct entity_proto_ref {
	unsigned char		proto_index;
	unsigned short		x;
	unsigned short		y;
};

struct player {
	entity_t character;
	/*
	These values describe the position of the chunk compared to the others

	Can store at most 0xFF and 0xFF == a map with 256 by 256 chunks
	*/
	unsigned char map_x;
	unsigned char map_y;
};

struct map_chunk {
	unsigned char		data[128];
	//These values are turned off because baked maps dont use them
	//unsigned char		x,y;
	//unsigned char		entity_refs_length;
};

struct map_cache {
	map_chunk_t cache[5];

	unsigned char sizes;	//0xF0 | 0x0F
	entity_t entities[8];	//8 entities (loaded)
};

struct game_header {
	unsigned char		sprites_tile_length;		// How many tile sprites?
	unsigned char		sprites_entity_length;		// How many entity sprites?
	unsigned char		sprites_projectile_length;	// How many projectile sprites?

	unsigned char		entity_protos_length;		// How many prototypes?

	unsigned char		mapsize_x, mapsize_y;
	unsigned char		player_x, player_y;		// Initial player position / spawnpoint?
};

struct game_globals {
	// Memory
	unsigned char *vram;				//where the VRAM is on the 9860gii

	// File handling
	int filehandle = -1;				// What file?
	size_t mapFileOffset;				// Where the map starts (this value is allways needed)
	game_header_t *header;				// The header containing metadata

	// Game
	sprite_t *sprite_tiles;				// All tile sprites; used to draw chunks
	sprite_t *sprite_entities;			// All entity sprites; used to draw chunks
	sprite_t *sprite_projectiles;		// All projectile sprites; used to draw chunks
	entity_proto_t *entity_protos;		// The Entity descriptors (protos)
	map_cache_t *g_mapcache;				// All currently loaded mapchunks
	player_t *g_player;					// The player
};

#endif /* GAMEDEFS_H_ */

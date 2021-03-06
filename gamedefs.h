#ifndef GAMEDEFS_H_
#define GAMEDEFS_H_

typedef struct	sprite				sprite_t;
typedef struct	entity				entity_t;
typedef struct	entity_proto		entity_proto_t;
typedef struct	entity_proto_ref	entity_proto_ref_t;

typedef struct	game_header			game_header_t;

typedef struct	game_pack			game_pack_t;
typedef struct	mapchunk			mapchunk_t;


struct game_header {
	unsigned char		sprites_tile_length;
	unsigned char		sprites_entity_length;
	// unsigned char		sprites_projectile_length;

	unsigned char		entity_protos_length;

	unsigned char		mapsize_x;
	unsigned char		mapsize_y;
	//unsigned char		player_x, player_y;
};

struct game_pack {
	game_header_t		game_header;

	sprite_t			*sprites_wall;
	sprite_t			*sprites_entity;
	entity_proto_t		*entity_protos;

	mapchunk_t			*map;
};

struct sprite {
	unsigned char		data[8];
};

/*
 * A combination of entity_proto and entity_proto_ref.
 * Contains all data for loaded entities.
 */
struct entity {
	unsigned char sprite_i;

	unsigned char hp;
	unsigned char fight_stats;

	unsigned short x,y;
	unsigned short speed;

	unsigned short ai_flags;
};

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
 */
struct entity_proto_ref {
	unsigned char		proto_index;
	unsigned short		x;
	unsigned short		y;
};

struct mapchunk {
	unsigned char		data[128];
	//These values are turned off because baked maps dont use them
	//unsigned char		x,y;
	//unsigned char		entity_refs_length;
};


#endif /* GAMEDEFS_H_ */

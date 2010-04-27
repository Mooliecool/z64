#include "zelda64.h"

#ifndef PLAYAS_H
 #define PLAYAS_H

typedef struct
{
    u8 block_f;
    u8 attack_f;
    u8 stand_f;
    u8 walk_f;
    u8 jump_f;
    u8 hit_f;
    u16 anim_off;	/* animation of offset in actor's RAM */
    u32 block;	/* unused at current */
    u32 attack;	/* -1 = no attack = spin actor */
    u32 stand;
    u32 walk;
    u32 jump;
    u32 hit;
} MPAnim;

typedef struct
{ /* FIXME: this is really hackish */
    char buff[4];
    u32 hierarchy;
    u32 ani_off;
    u32 two[2];
    f32 noFrames;
    f32 current_f;
    f32 speed;
    u32 vals_ptr;
} RamAnimation;

typedef struct
{
    u16 anum;
    u16 obj;
    u16 var;
} act_tospawn;


# if 0
MPAnim actors[] = {
 /*frames */			/* anim_off*/	/* block */	/* attack */	/* stand */ /* walk */
{-1, -1, -1,    -1,-1,-1,-1,			-1,		-1,		-1,		-1,		-1,    -1,      -1},	/* blank first entry */
{0x0002, 14,0,	0x4120,0x4100,0x4150,0x4120,	0x188,		0x06001C20,	0x0600B00C,	0x0600316C, 0x060026D4, 0x0032, 2},	/* Stalfo */
//{0x016C, 16,0,	0x41F0,0x41F0,0x4198,0x41F0,	0x14C,		0x06000700,	-1,	0x0600195C, 0x06000700, 0x0145, 0},	/* Graveyard boy */
//{0x01BC, 17,0,	0x4190,0x4190,0x41E0,0x4190,	0x14C,		0x06000C44,	-1,	0x06000600, 0x06000C44, 0x0122, 0x103},	/* Worker */
{0x000D, 15,0,	0x4180,0x4100,0x4180,0x4180,	0x14C,		0x06000A60,	0x060001A8,	0x06000A60, 0x06000A60, 0x0009, 0},	/* Poe */
//{0x001B, 15,0,  0x4130,0x4170,0x4130,0x41A0,	0x14C,		0x06000C70,	0x060004F8,	0x06000C70, 0x060012E4, 0x0016, 0},	/* Tektite */
{0x01AF, 15,0,	0x4080,0x41F0,0x41E8,0x4110,	0x188,		0x06004CA4,	0x06004638,	0x0600A4AC, 0x060057A0, 0x0183, 1},	/* Wolfos */
//{0x01B0, 15,0,	0x4180,0x4130,0x4180,0x4180,	0x14C,		0x060047E0,	0x06000460,	0x060047E0, 0x060047E0, 0x0184, 0},	/* Stalchild */
//{0x0028, 15,0,	0x4180,0x4100,0x4180,0x4120,	0x14C,		0x06001548,	0x06000544,	0x06001548, 0x06003D78, 0x001E, 0},	/* gohma larva */
//{0x0038, 15,0,	0x426C,0x426C,0x426C,0x426C,	0x14C,		0x06000228,	-1,	0x06000228, 0x06000228, 0x0025, 0},	/* torch slug */
};
#endif

const int mesh_tot = 4;

u32 (*actors_ram)[][3] = (void*)0x80700000;
int * init=(int*)0x80701008;
RamActor * link = (void*)LINK;

#endif /* PLAYAS_H */


/* Thanks to ZZT32 for some stuff in here */
#include "../mips.h"

/* spawners */
int ActorSpawn(u32 const_1, u32 const_2, int actor, f32 fx, f32 fy, f32 fz, u16 rx, u16 ry, u16 rz, int variable); /* 0x80031F50 */
void ObjectSpawn(u32 const_1, u16 object_number); /* 0x80097C00 */

/* Text functions (Must have proper hook) */
void SetTextRGBA(void * DList, u8 r, u8 g, u8 b, u8 a);
void SetTextXY(void * DList, u16 x, u16 y);
void SetTextString(void * DList, char * format, ...);

/* Some math functions */
extern f32  _sqrt( f32 );
extern f32  _abs( f32 );

/* ??? */
void func_80035118(void * a, void * o );

/* OoT controller layout */
typedef struct
{
    /* Buttons */
    u16 pad;
    
    /* Joystick */
    signed x : 8;
    signed y : 8;
} 
Controller;

typedef struct
{
	u16 x;
	u16 y;
	u16 z;
} rotation;

union FloatU32
{
	f32 	f;
	u32 	u;
};

/* Coord triad */
typedef struct
{
	float x;
	float y;
	float z;
} 
Coord;

/* Coord triad (w/ RAW) */
typedef struct
{
	union FloatU32 x;
	union FloatU32 y;
	union FloatU32 z;
} 
CoordU;
#define OOT_TOTALACTORS	470

/* Item amounts (incomplete!!) */
#define MAGIC		0x8015E693
#define BOMBS		0x8015E6EE
/* Misc */
#define ACT_TABLE	0x801162B8
#define OBJ_TABLE	0x80127528
#define FREEZE_ACTORS	0x80211174	/* Make any Non-Zero value to "freeze" all actors */
#define AS_A1		0x80212020	/* Actor spawn argument 1 */
#define CAMERA_PTR	0x80212290
#define AS_A0		0x80213C44	/* Actor spawn argument 0 */
#define ITEM_ACTORL	0x80213C8C
#define OS_A0		0x802237C4	/* Object spawn argument 0 */
#define MAP_POINTER	0x80223CE8
#define	NO_MAP_ACTORS	0x80223E0B
#define MAP_ACTOR_LIST	0x80223E18
#define LINK		0x802245B0
#define ITEM_PULLED	0x8022495C

/* Controllers */
#define P1_CONTROLLER	0x80166AF0
#define P2_CONTROLLER   0x80166B08
#define P3_CONTROLLER	0x80166B20
#define P4_CONTROLLER	0x80166B38

/* Buttons */
#define BUTTON_NONE	0x0000
#define BUTTON_CR	0x0001
#define BUTTON_CL	0x0002
#define BUTTON_CD	0x0004
#define BUTTON_CU	0x0008
#define BUTTON_R	0x0010
#define BUTTON_L	0x0020
#define BUTTON_DPAD_R	0x0100
#define BUTTON_DPAD_L	0x0200
#define BUTTON_DPAD_D	0x0400
#define BUTTON_DPAD_U	0x0800
#define BUTTON_START	0x1000
#define BUTTON_Z	0x2000
#define BUTTON_B	0x4000
#define BUTTON_A	0x8000

#include "god.h"

/* Globals */
#define MENULEN	6
#define DELAY_AMOUNT	5
#define NULL	0

extern void _start (void * a0) __attribute__ ((section (".main")));
extern int dma_write_ram ( void *ram_ptr, void *rom_ptr, u32 length );
char * menuOptions[MENULEN] = {
	"Main Menu\n",
	"Spawn Actor\n",
	"Spawn Object\n",
	"Loaded Actors\n",
	"Env Settings\n",
	"Memory Editor\n"
	};
char * offon[2] = {"Off", "On"}; /* for printing boolean values */

u8 currMenu=0;
int inc = 1;
int delay = 0;

/* Controller */
Controller *input = (void*)P1_CONTROLLER;


/* Main menu */
u8 currMenuOption=1;

void mainMenu(void * a0)
{
    int i;
    for (i=1; i<MENULEN; i++)
    {
        if (i == currMenuOption)
        {
            SetTextRGBA(a0, 255, 128, 255, 255);
            SetTextString(a0, menuOptions[i]);
            SetTextRGBA(a0, 128, 255, 128, 255);
        }
        else
            SetTextString(a0, menuOptions[i]);
    }
    if (delay) return;
    switch (input->pad)
    {
    case BUTTON_DPAD_U:
        delay = DELAY_AMOUNT;
        if (currMenuOption > 1)
            currMenuOption--;
        break;
    case BUTTON_DPAD_D:
        delay = DELAY_AMOUNT;
        if (currMenuOption+1 < MENULEN)
            currMenuOption++;
        break;
    case BUTTON_DPAD_R:
        delay = DELAY_AMOUNT;
        currMenu = currMenuOption;
        break;
    }
}

/* Actor spawner menu*/

char * actor_types[12] = {
    "Unknown",	"Prop (1)",	"Player",	"Bomb",
    "NPC",	"Enemy",	"Prop (2)",	"Item/Action",
    "Misc",	"Boss",		"Door",	"Chests"
};

u16 currActNum=0;
u16 currActVar=0;
u8 currActMenuItem=0;
u16 currActObj=0;
Coord currActPos;
rotation currActRot;
u8 asinit = 0;
u8 asgetinfo = 0;
char * actmissing = "NULL";
actor_spec_t * currActInfo;

#define ASNUM	0
#define ASX	1
#define ASY	2
#define ASZ	3
#define ASXR	4
#define ASYR	5
#define ASZR	6
#define ASVAR	7
#define ASSPAWN	8
#define ASRETURN	9

void updateActInfo(void * a0)
{
    u32 * actorStart = (void*)(ACT_TABLE + (currActNum <<5));
    u32 * actorVStart = (void*)(ACT_TABLE + 0x8+(currActNum <<5));
    u32 * actorVInfoOff = (void*)(ACT_TABLE + 0x14+(currActNum <<5));
    
    if (!*actorStart) /* Null actor */
    {
        currActInfo->actor_number=currActNum;
        currActInfo->actor_type=0;
        currActInfo->actor_status=0;
        currActInfo->object_number=0;
        return;
    }
    
    /* Copy 20 bytes actor header from ROM */
    dma_write_ram(&(currActInfo->actor_number), (void*)*actorStart + (*actorVInfoOff - *actorVStart), 0x20);
}

void spawnActorMenu(void * a0)
{
    u32 * ActName = (u32*)(ACT_TABLE + 0x18 + (currActNum << 5));
    if (!*ActName)
        ActName = (u32)actmissing;
    
    if (!asinit)
    {
        currActPos.x = 0.0;
        currActPos.y = 0.0;
        currActPos.z = 0.0;
        currActRot.x = 0.0;
        currActRot.y = 0.0;
        currActRot.z = 0.0;
        asinit = 1;
    }
    if (!asgetinfo)
        updateActInfo(a0);
    
    SetTextString(a0, "Object: %04X Type: %s\n",currActInfo->object_number, actor_types[currActInfo->actor_type]);
    if (currActMenuItem == ASNUM) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Actor: %04X : %s\n", currActNum, *ActName);
    SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Pos:\n" );
    if (currActMenuItem == ASX) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " X: %4.2f\n", currActPos.x);
    if (currActMenuItem == ASY) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " Y: %4.2f\n", currActPos.y);
    if (currActMenuItem == ASZ) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " Z: %4.2f\n", currActPos.z);
    SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "Rot:\n" );
    if (currActMenuItem == ASXR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " X: %i\n", currActRot.x/360);
    if (currActMenuItem == ASYR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " Y: %i\n", currActRot.y/360);
    if (currActMenuItem == ASZR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, " Z: %i\n", currActRot.z/360);
    if (currActMenuItem == ASVAR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Var: %04X\n", currActVar);
    if (currActMenuItem == ASSPAWN) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Spawn! ->\n");
    if (currActMenuItem == ASRETURN) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "<- Return\n");
    
    if (delay)
        return;
    switch (input->pad)
    {
    case BUTTON_DPAD_D:
        delay = DELAY_AMOUNT;
        if (currActMenuItem < 9)
            currActMenuItem++;
        break;
    case BUTTON_DPAD_U:
        delay = DELAY_AMOUNT;
        if (currActMenuItem > 0)
            currActMenuItem--;
        break;
    case BUTTON_DPAD_L:
        delay = DELAY_AMOUNT;
        switch (currActMenuItem)
        {
        case ASNUM:
            if (currActNum - inc >= 0)
               currActNum-=inc;
            asgetinfo = 0;
            break;
        case ASX:
            if (currActPos.x - (f32)inc > -10000.0)
               currActPos.x -=(f32)inc;
            break;
        case ASY:
            if (currActPos.y - (f32)inc > -10000.0)
               currActPos.y-=(f32)inc;
            break;
        case ASZ:
            if (currActPos.z - (f32)inc > -10000.0)
               currActPos.z-=(f32)inc;
            break;
        case ASXR:
            currActRot.x -=inc;
            break;
        case ASYR:
            currActRot.y-=inc;
            break;
        case ASZR:
            currActRot.z-=inc;
            break;
        case ASVAR:
            if (currActVar - inc > 0)
               currActVar-=inc;
            break;
        case ASRETURN:
            currMenu=0;
            break;
        }
        break;
    case BUTTON_DPAD_R:
        delay = DELAY_AMOUNT;
        switch (currActMenuItem)
        {
        case ASNUM:
            if (currActNum + inc < OOT_TOTALACTORS)
               currActNum+=inc;
            asgetinfo = 0;
            break;
        case ASX:
            if (currActPos.x + (f32)inc < 10000.0)
               currActPos.x +=(f32)inc;
            break;
        case ASY:
            if (currActPos.y + (f32)inc < 10000.0)
               currActPos.y+=(f32)inc;
            break;
        case ASZ:
            if (currActPos.z + (f32)inc < 10000.0)
               currActPos.z+=(f32)inc;
            break;
        case ASXR:
            currActRot.x += inc;
            break;
        case ASYR:
            currActRot.y += inc;
            break;
        case ASZR:
            currActRot.z += inc;
            break;
        case ASVAR:
            if (currActVar + inc < 0x10000)
               currActVar+=inc;
            break;
        case ASSPAWN:
            ActorSpawn(AS_A0, AS_A1, currActNum, currActPos.x, currActPos.y, currActPos.z, currActRot.x, currActRot.y, currActRot.z, currActVar);
            break;
        }
        break;
    }
}
/* thnks Jani
80222B20 0001 - Darker and cloudy weather
80222B22 0001 - Lightning
80222B2B XX00 - Sand transition
 01-all sand
 02-Sand blows over the screen(lightly)
 04-Removes sand from screen
 05-Sand blows over the screen(heavily)
80222B32 XX00 - rain, 01 is light rain FF is heavy rain
80222B32 0001 - Heaviest rain
80222A62 XXYY - Enviroment "presets" and colorings
80222B34 0000 ?snow
*/
#define LIGHTNING_ON	1
#define LIGHTNING_OFF	0
#define LIGHTNING	0x80222B23
#define SANDSTORM	0x80222B2A
#define RAIN_MAX	0x80222B32
#define RAIN_CURR	0x80222B33
#define CURRENVSET	0x80222A63

#define ESLIGHTNING	0
#define ESSANDSTORM	1
#define ESRAINMAX	2
#define ESRAINCURR	3
#define ESCURRENV	4
#define ESRETURN	5

char * sand[4] = {"None", "Full", "Blows over screen", "Blows over screen"};
u8 currEnvSetting=0;

void envSettings(void * a0)
{
    u8 *lightning	= (void*)LIGHTNING;
    u8 *sandstorm	= (void*)SANDSTORM;
    u8 *maxRain		= (void*)RAIN_MAX;
    u8 *currRain	= (void*)RAIN_CURR;
    u8 *currEnv		= (void*)CURRENVSET;
    if (*lightning) *lightning = 1;
    *sandstorm = *sandstorm & 3;
    if (currEnvSetting == ESLIGHTNING) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Lightning: %s\n", offon[*lightning]);
    if (currEnvSetting == ESSANDSTORM) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Sandstorm: %s\n", sand[*sandstorm&3]);
    if (currEnvSetting == ESRAINMAX) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Rain max: %i\n", *maxRain);
    if (currEnvSetting == ESRAINCURR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Rain current: %i\n", *currRain);
    if (currEnvSetting == ESCURRENV) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "Current Environment: %i\n", *currEnv);
    if (currEnvSetting == ESRETURN) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128,255);
    SetTextString(a0, "<- Return\n");
    if (delay)
        return;
    switch (input->pad)
    {
    case BUTTON_DPAD_D:
        delay = DELAY_AMOUNT;
        if (currEnvSetting < ESRETURN)
            currEnvSetting++;
        break;
    case BUTTON_DPAD_U:
        delay = DELAY_AMOUNT;
        if (currEnvSetting > 0)
            currEnvSetting--;
        break;
    case BUTTON_DPAD_L:
        delay = DELAY_AMOUNT;
        switch (currEnvSetting)
        {
        case ESLIGHTNING:
            *lightning = (~*lightning)&1;
            break;
        case ESSANDSTORM:
            if (*sandstorm)
                (*sandstorm)--;
            break;
        case ESRAINMAX:
            if (*maxRain-inc >= 0)
                *maxRain-=inc;
            break;
        case ESRAINCURR:
            if (*currRain-inc >= 0)
                *currRain-=inc;
            break;
        case ESCURRENV:
            if (*currEnv-inc >= 0)
                *currEnv-=inc;
            break;
        case ESRETURN:
            currMenu=0;
            break;
        }
        break;
    case BUTTON_DPAD_R:
        delay = DELAY_AMOUNT;
        switch (currEnvSetting)
        {
        case ESLIGHTNING:
            *lightning = (~*lightning)&1;
            break;
        case ESSANDSTORM:
            if (*sandstorm < 3)
                (*sandstorm)++;
            break;
        case ESRAINMAX:
            if (*maxRain+inc <= 255)
                *maxRain+=inc;
            break;
        case ESRAINCURR:
            if (*currRain+inc <= 255)
                *currRain+=inc;
            break;
        case ESCURRENV:
            if (*currEnv+inc <= 255)
                *currEnv+=inc;
            break;
        }
        break;
    }
}
/* Memory Editor */
/* datatypes */
#define BYTE	0
#define HWORD	1
#define WORD	2
#define FLOAT	3

#define MEADDR	0
#define MEVAR	1
#define METYP	2
#define MERET	3

s32 CurrAddr=0; /* Signed so I can pick up less than 0 */
u8 datatype=0;
u8 meMenuItem=0;
u32 incSizBytes=1;
u32 addrinc;

//static char * typeStrFmt[4]	= { "%02X", "%04X", "%08X", "%4.4f" };
char * typeStr[4] = { "Byte", "Halfword", "Word", "Float32" };
const u32 typeAndVals[4]	= { 0xFFFFFFFF,	0xFFFFFFFE, 0xFFFFFFFC, 0xFFFFFFFC };

void meme(void * a0)
{
    u8 (*memoryu8)[0x7F0000]	= (void*)0x80000000;
    u16 (*memoryu16)[0x3F8000]	= (void*)0x80000000;
    u32 (*memoryu32)[0x1FC000]	= (void*)0x80000000;
    f32 (*memoryf32)[0x1FC000]	= (void*)0x80000000;
    
    incSizBytes	= (0x01020404 >> ((3 - datatype) << 3)) & 0xFF;
    
    CurrAddr &= typeAndVals[datatype];
    addrinc = inc << (incSizBytes>>1);
    if (meMenuItem==MEADDR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "Addr: 8%07X\n", CurrAddr);
    if (meMenuItem==MEVAR) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "Val: ");
    switch (datatype)
    {
    case BYTE:
        SetTextString(a0, "%02X", (*memoryu8)[CurrAddr]);
        break;
    case HWORD:
        SetTextString(a0, "%04X", (*memoryu16)[CurrAddr>>1]);
        break;
    case WORD:
        SetTextString(a0, "%08X", (*memoryu32)[CurrAddr>>2]);
        break;
    case FLOAT:
        SetTextString(a0, "%.4f", (*memoryf32)[CurrAddr>>2]);
        break;
    }
    if (meMenuItem==METYP) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "\nType: %s\n", typeStr[datatype]);
    if (meMenuItem==MERET) SetTextRGBA(a0, 255, 128, 255, 255);
    else SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "<- Return");
    if (delay)
        return;
    switch(input->pad)
    {
    case BUTTON_DPAD_D:
        delay = DELAY_AMOUNT;
        if (meMenuItem < MERET)
            meMenuItem++;
        break;
    case BUTTON_DPAD_U:
        delay = DELAY_AMOUNT;
        if (meMenuItem > 0)
            meMenuItem--;
        break;
    case BUTTON_DPAD_L:
        delay = DELAY_AMOUNT;
        switch (meMenuItem)
        {
        case MEADDR:
            if (CurrAddr - addrinc >= 0)
                CurrAddr -= addrinc;
            break;
        case MEVAR:
            switch (datatype)
            {
            case BYTE:
                (*memoryu8)[CurrAddr] -= inc;
                break;
            case HWORD:
                (*memoryu16)[CurrAddr>>1] -= inc;
                break;
            case WORD:
                (*memoryu32)[CurrAddr>>2] -= inc;
                break;
            case FLOAT:
                (*memoryf32)[CurrAddr>>2] -= (10/(f32)inc);
                break;
            }
            break;
        case METYP:
            if (datatype) datatype--;
            break;
        case MERET:
            currMenu=0;
            break;
        }
        break;
    case BUTTON_DPAD_R:
        delay = DELAY_AMOUNT;
        switch (meMenuItem)
        {
        case MEADDR:
            if (CurrAddr + addrinc < 0x600000)
                CurrAddr += addrinc;
            break;
        case MEVAR:
            switch (datatype)
            {
            case BYTE:
                (*memoryu8)[CurrAddr] += inc;
                break;
            case HWORD:
                (*memoryu16)[CurrAddr>>1] += inc;
                break;
            case WORD:
                (*memoryu32)[CurrAddr>>2] += inc;
                break;
            case FLOAT:
                (*memoryf32)[CurrAddr>>2] += (10/(f32)inc);
                break;
            }
            break;
        case METYP:
            if (datatype<3) datatype++;
            break;
        }
        break;
    }
}
void incomplete(void * a0)
{
    SetTextString(a0, "Incomplete\n");
    SetTextRGBA(a0,255,128,255,255);
    SetTextString(a0, "<- Return");
    if (delay)
        return;
    else if ((input->pad == BUTTON_DPAD_R) || (input->pad == BUTTON_DPAD_L))
    {
        currMenu=0;
    }
}

u8 currActType = 0;
u8 actTypeActCurr[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
u8 amMenuItem = 0;

#define AMACTTYPE	0
#define AMTYPEACT	1
#define AMVAR		2
#define AMPOSX		3
#define AMPOSY		4
#define AMPOSZ		5
#define AMROTX		6
#define AMROTY		7
#define AMROTZ		8
#define AMSCALEX	9
#define AMSCALEY	10
#define AMSCALEZ	11
#define AMRETURN	12

#define TOTALACTTYPES	12
int i;

void actMod(void * a0)
{
    RamActor * actor = NULL;
    RAMActorListPtr (*RAMActorList)[12] = (void*)0x80213C50;	/* 80212020 + 001C30 */
    
    if (!((*RAMActorList)[currActType].noActors))
    {
        SetTextString(a0, "No actors of actor type %i\n", currActType);
        if ((amMenuItem > AMACTTYPE) || (amMenuItem < AMRETURN))
            amMenuItem = AMACTTYPE;
    }
    
    else
    {
        actor = (RamActor*)(*RAMActorList)[currActType].firstActor;
        if (actTypeActCurr[currActType])
        {
            for (i=0;i<actTypeActCurr[currActType];i++) // Step through linked list
            {
                if ( !(*actor).actor_next )
                {
                    actTypeActCurr[currActType]--;
                    break;
                }
                actor = (RamActor*)((*actor).actor_next);
            }
        }
    }
    if ((actor == NULL) && (amMenuItem > AMACTTYPE) && (amMenuItem < AMRETURN))
        amMenuItem = AMACTTYPE;
    
    SetTextString(a0, "Actor type ");
    (amMenuItem == AMACTTYPE) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, "%i", currActType);
    SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextString(a0, " / %s\n", actor_types[currActType]);    

    if (actor != NULL)
    {
        u32 * ActName = (u32*)(ACT_TABLE + 0x18 + ((*actor).actorNo << 5));
        
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, "Act ");
        (amMenuItem == AMTYPEACT) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, "%i", actTypeActCurr[currActType]+1);
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, " of %i\nVariable: ", (*RAMActorList)[currActType].noActors);
                
        (amMenuItem ==  AMVAR) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 2 */
        SetTextString(a0, "%04X\n", (*actor).actorVariable);
        
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, "Position\n");
        (amMenuItem ==  AMPOSX) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 3 */
        SetTextString(a0, " X: %.2f\n", (*actor).Pos2.x.f);
        (amMenuItem ==  AMPOSY) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 4 */
        SetTextString(a0, " Y: %.2f\n", (*actor).Pos2.y.f);
        (amMenuItem ==  AMPOSZ) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 5 */
        SetTextString(a0, " Z: %.2f\n", (*actor).Pos2.z.f);
        
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, "Rotation\n");
        (amMenuItem ==  AMROTX) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 6 */
        SetTextString(a0, " X: %.1f\n", (f32)(*actor).rot_2.x / 182.041666667);
        (amMenuItem ==  AMROTY) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 7 */
        SetTextString(a0, " Y: %.1f\n", (f32)(*actor).rot_2.y / 182.041666667);
        (amMenuItem ==  AMROTZ) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 8 */
        SetTextString(a0, " Z: %.1f\n", (f32)(*actor).rot_2.z / 182.041666667);
        
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0, "Scale\n");
        (amMenuItem ==  AMSCALEX) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 9 */
        SetTextString(a0, " X: %.2f\n",(*actor).Scale.x.f);
        (amMenuItem ==  AMSCALEY) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 10 */
        SetTextString(a0, " Y: %.2f\n",(*actor).Scale.y.f);
        (amMenuItem ==  AMSCALEZ) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 11 */
        SetTextString(a0, " Z: %.2f\n",(*actor).Scale.z.f);
        
        SetTextRGBA(a0, 128, 255, 128, 255);
        SetTextString(a0,"Actor: %04X / %s\n", (*actor).actorNo, *ActName);
    }
    
    (amMenuItem ==  AMRETURN) ? SetTextRGBA(a0, 255, 128, 255, 255) : SetTextRGBA(a0, 128, 255, 128, 255);	 /* 12 */
    SetTextString(a0, "\n<- Return");

    if (delay)
        return;
    
    switch (input->pad)
    {
    case BUTTON_DPAD_D:
        delay = DELAY_AMOUNT;
        if ((actor == NULL) && (amMenuItem != AMRETURN))
            amMenuItem = AMRETURN;
        else if (amMenuItem < AMRETURN)
            amMenuItem++;
        break;
    case BUTTON_DPAD_U:
        delay = DELAY_AMOUNT;
        if ((actor == NULL) && (amMenuItem != AMACTTYPE))
            amMenuItem = AMACTTYPE;
        else if (amMenuItem > 0)
            amMenuItem--;
        break;
    case BUTTON_DPAD_L:
        delay = DELAY_AMOUNT;
        switch (amMenuItem)
        {
        case AMACTTYPE:
            if(currActType)
                currActType--;
            break;
        case AMTYPEACT:
            if(actTypeActCurr[currActType])
                actTypeActCurr[currActType]--;
            break;
        case AMVAR:
            if( ((*actor).actorVariable - inc) > -1)
                (*actor).actorVariable -= inc;
            break;
        case AMPOSX:
            (*actor).Pos2.x.f -=(f32)inc / 10.0;
            break;
        case AMPOSY:
            (*actor).Pos2.y.f -=(f32)inc / 10.0;
            break;
        case AMPOSZ:
            (*actor).Pos2.z.f -=(f32)inc / 10.0;
            break;
        case AMROTX:
            (*actor).rot_2.x -= inc;
            break;
        case AMROTY:
            (*actor).rot_2.y -= inc;
            break;
        case AMROTZ:
            (*actor).rot_2.z -= inc;
            break;
        case AMSCALEX:
            (*actor).Scale.x.f -=(f32)inc / 10.0;
            break;
        case AMSCALEY:
            (*actor).Scale.y.f -=(f32)inc / 10.0;
            break;
        case AMSCALEZ:
            (*actor).Scale.z.f -=(f32)inc / 10.0;
            break;
        case AMRETURN:
            currMenu=0;
            break;
        }
        break;
    case BUTTON_DPAD_R:
        delay = DELAY_AMOUNT;
        switch (amMenuItem)
        {
        case AMACTTYPE:
            if(currActType+1 < TOTALACTTYPES)
                currActType++;
            break;
        case AMTYPEACT:
            if((*actor).actor_next && (actTypeActCurr[currActType]+1 < (*RAMActorList)[currActType].noActors))
                actTypeActCurr[currActType]++;
            break;
        case AMVAR:
            if( ((*actor).actorVariable + inc) < 0x10000)
                (*actor).actorVariable += inc;
            break;
        case AMPOSX:
            (*actor).Pos2.x.f +=(f32)inc / 10.0;
            break;
        case AMPOSY:
            (*actor).Pos2.y.f +=(f32)inc / 10.0;
            break;
        case AMPOSZ:
            (*actor).Pos2.z.f +=(f32)inc / 10.0;
            break;
        case AMROTX:
            (*actor).rot_2.x += inc;
            break;
        case AMROTY:
            (*actor).rot_2.y += inc;
            break;
        case AMROTZ:
            (*actor).rot_2.z += inc;
            break;
        case AMSCALEX:
            (*actor).Scale.x.f +=(f32)inc / 10.0;
            break;
        case AMSCALEY:
            (*actor).Scale.y.f +=(f32)inc / 10.0;
            break;
        case AMSCALEZ:
            (*actor).Scale.z.f +=(f32)inc / 10.0;
            break;
        case AMRETURN:
            currMenu=0;
            break;
        }
        break;
    }
}
void _start(void * a0)
{
    
    SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextXY(a0, 0, 7);
    SetTextString(a0, menuOptions[currMenu]);
    switch (currMenu)
    {
    case 0:
        mainMenu(a0);
        break;
    case 1:
        spawnActorMenu(a0);
        break;
    case 2:/* Spawn object */
        incomplete(a0);
        break;
    case 3:/* Actors in ram */
        actMod(a0);
        break;
    case 4:
        envSettings(a0);
        break;
    case 5:/* Meme */
        meme(a0);
        break;
    }
    
    /* change increment value */
    if (input->pad == BUTTON_L && (!delay))
    {
        delay = DELAY_AMOUNT;
        if (inc < 0x2000)
            inc <<= 1;
        else
            inc = 1;
    }
    
    /* decrease delay */
    if (delay)
        delay--;
    
    /* print increment value */
    SetTextRGBA(a0, 128, 255, 128, 255);
    SetTextXY(a0, 0, 27);
    if (currMenu == 5) /* Execptions for memory editor */
    {
        if (meMenuItem == MEVAR)
        {
            if (datatype == FLOAT)
                SetTextString(a0, "\nInc: %.4f", (10/(f32)inc) );
            else
                goto defaultinc;
        }
        else if (meMenuItem == MEADDR)
            SetTextString(a0, "\nInc: 0x%06X", addrinc);
        else
            goto defaultinc;
    }
    else if (currMenu == 3) /* Execptions for actor menu */
    {
        if (amMenuItem > AMVAR)
        {
            if (amMenuItem == AMRETURN)
                goto defaultinc;
            else if ((amMenuItem > AMROTZ) || (amMenuItem < AMROTX))
                SetTextString(a0, "\nInc: %.2f", (f32)inc / 10.0 );
            else
            {
                if(inc < 8)
                    inc = 16;
                SetTextString(a0, "\nInc: %.1f", (f32)inc/182.041666667 );
            }
        }
        else
            goto defaultinc;
    }
    else /* Normal value */
    {
        defaultinc:
        SetTextString(a0, "\nInc: %04d (0x%X)", inc, inc);
    }
    
}






/*
*/

#include "playas.h"

#define TRUE	1
#define FALSE	0
extern void playas (void * a0) __attribute__ ((section (".main")));

u32 mesh_act=0;
u32 * ZTargetPtr = (void*)ZTARGETPOINTER;
Controller *input1 = (void*)P1_CONTROLLER;
int delay=0;
int animation_offset=0;
u32 pres_ani;
u16 pres_ani_f;
int mesh_curr = 0;
RamActor * mesh;
RamAnimation * mesh_a;
RamAnimation * link_a;
u32 last;
u32 * linkmain = (u32*)0x80701004;

int shield = FALSE;
f32 speedmod;
u32 * shield_dl;
u32 * sword_dl;
u16 * link_flags = (void*)LINK+0x158;

/*
Stalfos sword/sheild display list pointers:
80368948 - hierarchy
803687B8 - 0x06005DA0 shield	diff 0x190
8036880C - 0x06006A48 sword	diff 0x13C

stalfo (Playtendo)
(-4 = actual)				(+1 = actual)
0x448	Hit				7
0x1424	Defeat (falls backward)		32
0x197C	Jump (backward) 		9
0x1C24	Block				2
0x218C	Entrance?			9
0x26D8	Walking Forward			9
0x3170	Standing			26
0x81B8	Slowly walking Forward		12
0x8608	Hit				8
0x9A94	Defeat (Falls forward)		40
0x9E54	Falling				5
0xA328	Jump slash start		7
0xA9a0	Jump slash end			11
0xB010	Slash start			11
0xB4E8	Slash end			7
0xBE50	Slash				18
0xC43C	Landing				11
0xC8A4	Laughing?			12
0xDCF4	Looking around			52
0xE2B4	Side-stepping			14
0xE7E8	animation after entrance?	11

wolfos (Playtendo):
0x4AD4	Backflip	12
0x4CA8	Crouch		3
0x5434	Defeated?	24
0x57A4	Run Forward	8
0x98CC	Side-step	6
0x9B24	Hit?		7

dinolfos/lizalfos (SirTopHat): (offsets and #frames fixed)
0x06008138 59 Walking									WALK
0x06008C6C 21 Swing sword down while bending down a bit
0x06009530 20 Walking with shorter stride
0x0600A3D4 30 Swimg sword down after swinging behind					JUMP	ATTACK
0x0600B10C 29 Open mouth								STAND
0x0601081C 7  Bent to ground, goes up slightly
0x06010CAC 7  Wave hands outward, bob head						BLOCK
0x06011070 7  Bent over, bending down to crawling position
0x060119F4 17 Spin around
0x0601366C 56 Do 0x11070 and then get up and spin around
0x06014E60 50 Fall backwards
0x060157F8 20 First part bit of 0x14E60 but then goes back to starting position		HIT
0x06016388 29 Walk as if swimming or frolicing
*/

MPAnim actors[] = {
/*frames               anim_off block           attack          stand       walk        jump       hit*/
{0, 0, 0,  0,   0,	-1,	-1,		-1,		-1,	    -1,         -1,         -1	},
{3, 19, 27, 10, 12, 8,	0x188,	0x06001C20,	0x0600BE4C,	0x0600316C, 0x060026D4, 0x0600A99C, 0x06000444}, /* Stalfo */
{4, 30, 29, 9, 13, 8,	0x188,	0x06004CA4,	0x06004638,	0x0600A4AC, 0x060057A0, 0x06004AD0, 0x06009B20}, /* Wolfos */
{7, 30, 29, 59, 30, 20, 0x14C,	0x06010CAC,	0x0600A3D4,	0x0600B10C, 0x06008138, 0x0600A3D4, 0x060157F8}  /* Lizalfos */
};

void step_anim(RamAnimation * mesh);

void initialize()
{
    int i;
    last = 0;
    mesh_act = 0;
    delay=0;
    pres_ani=0;
    pres_ani_f=0;
    mesh_curr=0;
    *init=1;
    shield = FALSE;
}

void playas(void * a0)
{
    /* do nothing on failure to spawn actors */
    if(!(*actors_ram)[1][0])
        return;
    /* move unused actors out of the way */
    SetTextString(a0, "\r%i\n", mesh_curr);
    if(!*init)
        initialize();
    int i;
    RamActor * tmp;
    for(i=1;i<mesh_tot;i++){
        tmp = (RamActor*)(*actors_ram)[i][0];
        if(i!=mesh_curr && tmp){
            tmp->Pos2.x.u = 0;
            tmp->Pos2.y.f = -2000.0;
            tmp->Pos2.z.u = 0;
         //   tmp->ActorDraw = (*actors_ram)[i][2];
        }
    }
    mesh = (void*)(*actors_ram)[mesh_curr][0];
    
    if(mesh && mesh_curr) /* play as something else */
    {
        mesh_a = (void*)(actors[mesh_curr].anim_off + (*actors_ram)[mesh_curr][0]);
        link_a = (void*)LINK + 0x1B4;
        
        link->ActorMain = NULL;
        mesh->Scale.y.u = mesh->Scale.z.u;
        if(!(*actors_ram)[mesh_curr][1])
            mesh->ActorDraw = NULL;//mesh->ActorMain;
            mesh->ActorMain = NULL;
        else
        {
            (*actors_ram)[mesh_curr][1]--;
            return;
        }
        /* update position */
        mesh->Pos2.x.u = link->Pos2.x.u;
        mesh->Pos2.y.u = link->Pos2.y.u;
        mesh->Pos2.z.u = link->Pos2.z.u;
        mesh->rot_2.y = link->rot_2.y;
        
        /* VERY IMPORTANT! step the next frame */
        if(mesh_a->ani_off!=actors[mesh_curr].block || shield)
        {
            step_anim(mesh_a);
            shield = FALSE;
        }
        
        /* render a frame */
        renderFrame(AS_A1, mesh_a->hierarchy, mesh_a->vals_ptr, NULL, mesh, NULL);
        
        if(*link_flags == 0x0303) /* sword put away */
        {
            if(mesh->actorNo == 2)
            {
                shield_dl = (void*)mesh_a->hierarchy - 0x190;
                sword_dl = (void*)mesh_a->hierarchy - 0x13C;
                if(mesh_a->ani_off!=actors[mesh_curr].block)
                    *shield_dl = 0;
                else
                    *shield_dl = 0x06005DA0;
                *sword_dl = 0;
            }
        }
        else
        {
            if(mesh->actorNo == 2)
            {
                *shield_dl = 0x06005DA0;
                *sword_dl = 0x06006A48;
            }
        }
        
        if(last != link_a->ani_off)
        {
            speedmod = 1.0f;
            switch(link_a->ani_off)
            {
            case 0x04003140:	/* walk/run */
            case 0x04003178:
            case 0x04003190:
                //if(mesh_a->ani_off!=actors[mesh_curr].walk)
                {
                    mesh_a->current_f = 0.0f;
                    mesh_a->ani_off = actors[mesh_curr].walk;
                    mesh_a->noFrames = (f32)actors[mesh_curr].walk_f;
                    speedmod = 1.5f;
                }
                break;
            case 0x040033E8:	/* standing */
            case 0x04003240:
            case 0x04003200:
            case 0x04002AC8:	/* acutally jump slash pt 3 */
            {
            standing:
                //if(mesh_a->ani_off!=actors[mesh_curr].stand)
                {
                    mesh_a->current_f = 0.0f;
                    mesh_a->ani_off = actors[mesh_curr].stand;
                    mesh_a->noFrames = (f32)actors[mesh_curr].stand_f;
                    speedmod = 0.75f;
                }
                break;
            }
            case 0x04002968:
            case 0x04002970:
            case 0x04002A80:
            case 0x04003180:
            {
            slash:
                if(mesh_a->ani_off!=actors[mesh_curr].attack)
                {
                    mesh_a->current_f = 0.0f;
                    mesh_a->ani_off = actors[mesh_curr].attack;
                    mesh_a->noFrames = (f32)actors[mesh_curr].attack_f;
                }
                break;
            }
            case 0x04002400:	/* block */
            case 0x04002ED0:
            case 0x040025D0:
                //if(mesh_a->ani_off!=actors[mesh_curr].block)
                {
                    mesh_a->current_f = 1.0f;
                    mesh_a->ani_off = actors[mesh_curr].block;
                    mesh_a->noFrames = (f32)actors[mesh_curr].block_f;
                    shield=TRUE;
                }
                break;
            case 0x04002528:	/* hit */
                //if(mesh_a->ani_off!=actors[mesh_curr].hit)
                {
                    mesh_a->current_f = 0.0f;
                    mesh_a->ani_off = actors[mesh_curr].hit;
                    mesh_a->noFrames = (f32)actors[mesh_curr].hit_f;
                }
                break;
            case 0x04002900: /* jump pt 1 */
                if(mesh->actorNo == 2)
                {
                    //if(mesh_a->ani_off != 0x0600A324)
                    {
                        mesh_a->ani_off = 0x0600A324;
                        mesh_a->noFrames = 8.0f;
                        mesh_a->current_f = 0.0f;
                    }
                }
                else
                    goto jumpstrike;
                break;
            case 0x04002910: /* jump pt 2 */
                if(mesh->actorNo == 2)
                {
                    //if(mesh_a->ani_off != 0x0600A99C)
                    {
                        mesh_a->ani_off = 0x0600A99C;
                        mesh_a->noFrames = 12.0f;
                        mesh_a->current_f = 0.0f;
                    }
                }
                else
                {
                jumpstrike:
                    mesh_a->ani_off = actors[mesh_curr].jump;
                    mesh_a->noFrames=(f32)actors[mesh_curr].jump_f;
                    mesh_a->current_f = 0.0f;
                }
                break;
            }
        }
        //mesh_a->speed = speedmod;
        
        SetTextString(a0, "mesh=0x%08X\nmesh_a=0x%08X\nlink_a->ani_off=0x%08X\nmesh_a->ani_off=0x%08X\nmesh_a->noFrames=%.0f\nmesh_a->current_f=%.0f\nmesh_a->speed=%.3f",
                      mesh, mesh_a, link_a->ani_off, mesh_a->ani_off, mesh_a->noFrames, mesh_a->current_f, mesh_a->speed);
        
        last = link_a->ani_off;
    }
    else /* normal link */
        link->ActorMain = *linkmain;
    SetTextString(a0, "\n\nPos:\n[%.0f, %.0f, %.0f]", link->Pos2.x.f, link->Pos2.y.f, link->Pos2.z.f);
    /* delay */
    if (delay)
    {
        delay--;
        return;
    }
    /* poll for player switch */
    switch(input1->pad)
    {
    case BUTTON_DPAD_D:
        delay = 10;
        if(mesh_curr)
            mesh_curr--;
        else
            mesh_curr = mesh_tot;
        break;
    case BUTTON_DPAD_U:
        delay = 10;
        if(mesh_curr < mesh_tot-1)
            mesh_curr++;
        else
            mesh_curr = 0;
        break;
    }
}

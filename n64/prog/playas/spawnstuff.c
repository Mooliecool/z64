/*
*/
#include "playas.h"

u32 * linkmain = (u32*)0x80701004;

act_tospawn actn[] = {
{ -1,	-1,	-1 },
{0x0002, 0x0032, 0x0002}, /* stalfo */
{0x01AF, 0x0183, 0x0001}, /* wolfos */
{0x0025, 0x001B, 0x0080}  /* lizalfos */
};


void spawnstuff(int a0, int a1)
{
    int i=1;
    *init = 0;
    *linkmain = link->ActorMain;
    RamActor * tmp;
    do{
       ObjectSpawn(OS_A0, actn[i].obj);
       tmp = (void*)ActorSpawn(a0, a1, actn[i].anum, 0.0,-2000.0, 0.0, 0, 0, 0, actn[i].var);
       (*actors_ram)[i][0] = (u32)tmp;
       (*actors_ram)[i][1] = 4;
       (*actors_ram)[i][2] = tmp->ActorDraw;
    }while(++i<mesh_tot);
}


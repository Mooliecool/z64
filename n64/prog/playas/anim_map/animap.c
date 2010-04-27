#include "zelda64.h"

u32 * ZTargetPtr = (void*)ZTARGETPOINTER;
typedef struct
{ /* FIXME: really hackish */
    char buff[8];
    u32 ani_off;
    u32 two[2];
    f32 noFrames;
    f32 current_f;
    f32 speed;
} RamAnimation;
/* Quick and dirty! */
void _start(void*a0){
SetTextString(a0,"\nZ-Target: %08X\n",*ZTargetPtr);
if(*ZTargetPtr){
RamActor*ztarget_act= (void*)*ZTargetPtr;
SetTextString(a0,"Anum:%04X\n",ztarget_act->actorNo);
switch(ztarget_act->actorType)
{
case 4:;
case 6:;
RamAnimation * ramAni = (void*)(0x14C+(*ZTargetPtr));
SetTextString(a0,"NPC\nAnim: %08X\nNFrames: %04X",ramAni->ani_off,ramAni->noFrames);
break;
case 5:;
int amnttoadd=0x14C;
if ((ztarget_act->actorNo==2)||(ztarget_act->actorNo==0x197)||
    (ztarget_act->actorNo==0x1AF))
    amnttoadd=0x188;
RamAnimation * ramAni_ = (void*)(amnttoadd+(*ZTargetPtr));
SetTextString(a0,"Enemy\nAnim: %08X\nNFrames: %.0f",ramAni_->ani_off,ramAni_->noFrames);
break;
default:
SetTextString(a0,"\nUnmapped: %02X",ztarget_act->actorType);
break;
}
}
}

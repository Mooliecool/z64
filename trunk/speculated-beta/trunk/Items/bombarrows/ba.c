#include "mips.h"
#include "actor.h"

#define ITEM_PULLED	0x8022495C
#define BA_ENABLED	0x80600400
#define BA_LIST		0x80600410
#define BA_COUNT	0x80600401

typedef struct
{
    u32 arrow;
    u32 bomb;
} bombarrow;

u32 ActorSpawn(u32 const_1, u32 const_2, int actor, f32 fx, f32 fy, f32 fz, u16 rx, u16 ry, u16 rz, int variable);

void _start()
{
    u8 *enabled = (void*)BA_ENABLED;
    if (*enabled)
    {
        u8 *ba_count = (void*)BA_COUNT;
        bombarrow (*ba_list)[1] = (void*)BA_LIST;
        u32 *new_item = (void*)ITEM_PULLED;
        u8 *bombs = (void*)0x8015E6EE;
        int cnt = 0;
        
        if (*new_item && *bombs)
        {
            u16 *new_item_act_no = (void*)*new_item;
            if (*new_item_act_no == 0x16)
            {
                int spawn = 0;
                while (cnt < *ba_count)
                {
                    if ((*ba_list)[cnt].arrow == *new_item)
                    {
                        spawn = 1;
                        break;
                    }
                    cnt+=1;
                }
                if (!spawn)
                {
                    (*ba_list)[*ba_count].arrow = *new_item;
                    (*ba_list)[*ba_count].bomb = ActorSpawn(
			0x80213C44,
			0x80212020,
			0x0010,
			0,
			0,
			0,
			0,
			0,
			0,
			0
                    );
                    *ba_count += 1;
                    *bombs -= 1;
                }
            }
        }
        cnt = 0;
        int ba_count_cpy = *ba_count;
        while (cnt < ba_count_cpy)
        {
            struct z64_actor_t * Arrow;
            struct z64_actor_t * Bomb;
            Arrow = (struct z64_actor_t*)(*ba_list)[cnt].arrow;
            Bomb  = (struct z64_actor_t*)(*ba_list)[cnt].bomb;
            
            u32 *arrow_hit = (void*)((*ba_list)[cnt].arrow + 0x74);
            s16 *bomb_timer = (void*)((*ba_list)[cnt].bomb + 0x1F8);
            u16 *bomb_exploded = (void*)((*ba_list)[cnt].bomb + 0x1C);
            
            /* Explode bomb? */
            if (*arrow_hit != 0)
            {
                *bomb_timer /= 2;
            }
            
            /* Has the bomb exploded? */ 
            if ( *bomb_exploded == 1)
            {
                *ba_count--;
                (*ba_list)[cnt].arrow = 0;
                (*ba_list)[cnt].bomb = 0;
            }
            
            else
            {
                Bomb->coords_2 = Arrow->coords_2;
                Bomb->coords_2.y += 20.0;
            }
            cnt +=1;
        }
    }
}

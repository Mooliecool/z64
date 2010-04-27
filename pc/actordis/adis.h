/*
 sinep
*/

#define OOT_DEBUG_ATABLE_START	0x00B8D440

typedef char			u8;
typedef unsigned short		u16;
typedef unsigned		u32;
typedef signed char		s8;
typedef signed short		s16;
typedef signed		s32;
typedef float			f32;

typedef struct {
	unsigned text_size;
	unsigned data_size;
	unsigned rodata_size;
	unsigned bss_size;
	unsigned rel_count;
} actorHeader;

typedef struct {
	unsigned rom_start;
	unsigned rom_end;
	unsigned virtual_start;
	unsigned virtual_end;
	unsigned unknown_0;
	unsigned virtual_ainfo;
	unsigned name;
	unsigned unknown_1;
} actorCodeEntry;

typedef struct {
	unsigned short number;
	unsigned char type;
	unsigned char status;
	unsigned long unknown_0;
	unsigned short object_number;
	unsigned short unknown_1;
	unsigned long unknown_2;
	unsigned long init_func;
	unsigned long code_func;
	unsigned long draw_func;
	unsigned long unknown_func;
} actorInformation;

typedef struct {
	unsigned addr;
	char * name;
} function;




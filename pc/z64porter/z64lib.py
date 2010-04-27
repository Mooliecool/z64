
from struct import unpack

#When editing scene maximums, the two are located below:
MAX_MM_SCENE    = 105
MAX_OOT_SCENE   = 109
# Scene max values ^---HERE

MM_SCENE_FMT    = ">LLLL"
OOT_SCENE_FMT   = ">LLLLL"
MM_SCENE_TABLE  = 0xC5A250
OOT_SCENE_TABLE = 0xBA0BB0
BAD_MM_SCENES   = [ 2, 7, 8, 51, 42 ]

oot_to_mm_acts = {
#   OoT    : MM
    0x0000 : 0x0000,    #Link
    0x0008 : 0x0004,    #Flame
    0x0009 : 0x0005,    #Door
    0x0007 : 0x000F,    #Dissipating flames
    0x000A : 0x0006,    #Chest
    0x0010 : 0x0009,    #Bomb
    0x0011 : 0x000A,    #Wallmaster
    0x0012 : 0x000B,    #Dodongo
    0x0013 : 0x000C,    #Keese
    0x0014 : 0x0054,    #Epona
    0x0015 : 0x000E,    #Collectables
    0x0018 : 0x0010,    #Fairies
    0x0019 : 0x0011,    #Cucco
    0x001B : 0x0012,    #Tektite
    0x001C : 0x0013,    #???
    0x001D : 0x0014,    #Peahat
    0x001E : 0x0015,    #Butterfly
    0x0020 : 0x0016,    #Bugs
    0x0021 : 0x0017,    #Fish
    0x0023 : 0x0018,    #Room changing plane
    0x0025 : 0x0019,    #Dinolfos/Lizafos
    0x0026 : 0x001A,    #Wooden post with red cloth
    0x0029 : 0x0152,    #Zelda
    0x002D : 0x001D,    #Bubble
    0x002E : 0x001E,    #Studded lifting door
    0x0032 : 0x0020,    #Boomerang
    0x0032 : 0x0022,    #???
    0x0037 : 0x0024,    #Skulltula
    0x0039 : 0x0027,    #gameplay_keep stuffs
    0x003B : 0x0028,    #sounds
    0x0049 : 0x0162,    #Flame circle
    0x004C : 0x002F,    #Bombflowers
    0x0055 : 0x0033,    #Deku baba
    0x005D : 0x0038,    #Warp portals
    0x005E : 0x0039,    #Torch stand
    0x005F : 0x003A,    #Heart container
    0x0060 : 0x003B,    #Deku scrub
    0x0065 : 0x003F,    #Water
    0x0068 : 0x0061,    #Twisted hallway
    #NOTE: oot one requires objects 0x71, 0x72 and 0x73 to be loaded
    
    0x0069 : 0x003E,    #Bubble (bouncing skull)
    0x0077 : 0x0041,    #Tree
    0x008A : 0x0047,    #Beamos
    0x008D : 0x0172,    #Flame Wall
    0x008E : 0x004A,    #Floormaster
    0x0090 : 0x004C,    #ReDead
    0x0094 : 0x004F,    #Butterflies (again)
    0x0095 : 0x0050,    #Skullwalltula
    0x009D : 0x01E3,    #Gravestone
    0x00A1 : 0x0069,    #Ruto
    0x00B0 : 0x00A7,    #I'M ON A BOAT
    0x00B5 : 0x018E,    #Flying rubble
#Zeth's:
    0x00DD : 0x006C,    #Like Like
    0x0167 : 0x00bd,    #Kakariko Roof Guy
    0x0153 : 0x0248,    #Music Box Grinder man
    0x0162 : 0x017d,    #Runningman/postman
    0x019B : 0x00e2,    #Doggie
    0x01B9 : 0x00ef,    #Gossip Stones
    0x0178 : 0x01C7,    #Hyrule/Town guard      oot var 0000
    0x0142 : 0x01C7,    #Hyrule/Town guard      oot var 0001
    0x00B3 : 0x01C7,    #Hyrule/Town guard
    0x01CE : 0x0228,    #Zora   MM:normal       oot var 0000
    0x0186 : 0x00FA,    #Walking Gerudo Guards  oot Var < 3
    0x01AE : 0x0242,    #Gorons                 oot var < 0xD
    0x01AF : 0x00EC,    #Wolfos
    0x01C0 : 0x00F1     #Guay

}

mm_to_oot_acts = {
#   MM     : OoT
    0x0000 : 0x0000,    #Link
    0x0004 : 0x0008,    #Flame
    0x0005 : 0x0009,    #Door
    0x000F : 0x0007,    #Dissipating flames
    0x0006 : 0x000A,    #Chest
    0x0009 : 0x0010,    #Bomb
    0x000A : 0x0011,    #Wallmaster
    0x000B : 0x0012,    #Dodongo
    0x000C : 0x0013,    #Keese
    0x0054 : 0x0014,    #Epona
    0x000E : 0x0015,    #Collectables
    0x0010 : 0x0018,    #Fairies
    0x0011 : 0x0019,    #Cucco
    0x0012 : 0x001B,    #Tektite
    0x0013 : 0x001C,    #???
    0x0014 : 0x001D,    #Peahat
    0x0015 : 0x001E,    #Butterfly
    0x0016 : 0x0020,    #Bugs
    0x0017 : 0x0021,    #Fish
    0x0018 : 0x0023,    #Room changing plane
    0x0019 : 0x0025,    #Dinolfos/Lizafos
    0x001A : 0x0026,    #Wooden post with red cloth
    0x0152 : 0x0029,    #Zelda
    0x001D : 0x002D,    #Bubble
#    0x000E : 0x002E,    #Studded lifting door
    0x001E : 0x002E,    #Studded lifting door
    0x0020 : 0x0032,    #Boomerang
    0x0022 : 0x0032,    #???
    0x0024 : 0x0037,    #Skulltula
    0x0027 : 0x0039,    #gameplay_keep stuffs
    0x0028 : 0x003B,    #sounds
    0x0162 : 0x0049,    #Flame circle
    0x002F : 0x004C,    #Bombflowers
    0x0033 : 0x0055,    #Deku baba
    0x0038 : 0x005D,    #Warp portals
    0x0039 : 0x005E,    #Torch stand
    0x003A : 0x005F,    #Heart container
    0x003B : 0x0060,    #Deku scrub
    0x003F : 0x0065,    #Water
    0x0061 : 0x0068,    #Twisted hallway
    0x003E : 0x0069,    #Bubble (bouncing skull)
    0x0041 : 0x0077,    #Tree
    0x0047 : 0x008A,    #Beamos
    0x0172 : 0x008D,    #Flame Wall
    0x004A : 0x008E,    #Floormaster
    0x004C : 0x0090,    #ReDead
    0x004F : 0x0094,    #Butterflies (again)
    0x0050 : 0x0095,    #Skullwalltula
    0x01E3 : 0x009D,    #Gravestone
    0x0069 : 0x00A1,    #Ruto
    0x00A7 : 0x00B0,    #I'M ON A BOAT
    0x018E : 0x00B5,    #Flying rubble
    0x01DA : 0x0090,    #Gibodo (Use oot var -2 )
    0x0235 : 0x0090,    #Gibodos (Use oot var -2)
    0x00ED : 0x01B0,    #Stalchild
    0x02A5 : 0x01B0,    #Stalchild
    0x0191 : 0x0115,    #Skullkid
    0x00E4 : 0x019E,    #Beehive
    0x00E5 : 0x01A0,    #Crate
    0x00E9 : 0x01AC,    #Honey and darling
    0x0105 : 0x0054,    #Armos
    0x0110 : 0x008D,    #Firewall
    0x028D : 0x01A8,    #Cracked wall
    0x0298 : 0x01A9,    #cracked wall
    0x0255 : 0x00CF,    #cracked wall
    0x0258 : 0x015B,    #cracked wall
    0x00F3 : 0x01C6,    #Cow
    0x0220 : 0x00E7,    #Cremia/Child malon
    0x01A4 : 0x01C5,    #Malon/Romani           oot var FFFF
    0x021F : 0x01C5,    #Malon/Romani(guess)    oot var FFFF
    0x02AF : 0x0112,    #invisible collectibles
    0x0066 : 0x00C7,    #Withered Deku Baba
    0x011F : 0x01C7,    #Iceicles
    0x012D : 0x0055,    #Bio deku baba
    0x01BD : 0x011A,    #Deku salesman
    0x0274 : 0x011A,    #Deku salesman
    0x026E : 0x0142,    #Gaurd
    0x01EE : 0x019B,    #racing dog
    0x01F1 : 0x0021,    #Labratory fish
    0x01F3 : 0x000D,    #Poe
    0x0208 : 0x006D,    #Big poe
    0x01CA : 0x0085,    #Dampe
    0x008F : 0x0121,    #Freezard
    0x0216 : 0x001C,    #leever
#Zeth's:
    0x006C : 0x00DD,    #Like Like
    0x00bd : 0x0167,    #Kakariko Roof Guy
    0x0248 : 0x0153,    #Music Box Grinder man
    0x017d : 0x0162,    #Runningman/postman
    0x00e2 : 0x019B,    #Doggie
    0x00ef : 0x01B9,    #Gossip Stones
    0x01C7 : 0x0142,    #Hyrule/Town guard      oot var 0001
    0x00F8 : 0x01CE,    #Zora   MM:Swimming     oot var 0000
    0x0228 : 0x01CE,    #Zora   MM:normal       oot var 0000
    0x0231 : 0x01CE,    #Zora   MM:Guitarist    oot var 0000
    0x0238 : 0x01CE,    #Zora   MM:Drummer      oot var 0000
    0x0241 : 0x01CE,    #Zora   MM:Pianist      oot var 0000
    0x0252 : 0x01CE,    #Zora   MM:Singer       oot var 0000
    0x0260 : 0x01CE,    #Zora   MM:Swimming     oot var 0000
    0x00FA : 0x0186,    #Walking Gerudo Guards  oot Var < 3
    0x0242 : 0x01AE,    #Gorons                 oot var < 0xD
    0x00EC : 0x01AF,    #Wolfos
    0x00F1 : 0x01C0     #Guay
}

oot_to_mm_objs = {
#   OoT    : MM
    0x000E : 0x000C,    #Chests
    0x000B : 0x0009,    #Wallmaster
    0x000C : 0x000A,    #Dodongo
    0x000D : 0x000B,    #Keese
    0x001A : 0x007D,    #Epona
    0x0013 : 0x000F,    #Cucco
    0x0016 : 0x0012,    #Tektie
    0x0017 : 0x0013,    #???
    0x0018 : 0x0014,    #Peahat
    0x001B : 0x0017,    #Dinolfos/Lizafos
    0x0076 : 0x005F,    #Wooden post with red cloth
    0x001D : 0x014B,    #Zelda
    0x0012 : 0x000E,    #Bubble
    0x0022 : 0x00BC,    #???
    0x0024 : 0x0020,    #Skulltula
    0x0031 : 0x002A,    #bombflowers
    0x0039 : 0x0031,    #deku baba
    0x0048 : 0x003E,    #Warp portals
    0x00A4 : 0x0080,    #Torch stand
    0x00BD : 0x0096,    #Heart container
    0x004A : 0x0040,    #Deku scrub
    0x0059 : 0x017E,    #Water
    0x0070 : 0x0088,    #Twisted hallway
    0x005D : 0x0051,    #Bubble (bouncing skull)
    0x007C : 0x0061,    #Tree
    0x008B : 0x006A,    #Beamos
    0x002C : 0x0153,    #Flame Wall
    0x000B : 0x0009,    #Floormaster
    0x0098 : 0x0075,    #ReDead
    0x0024 : 0x0020,    #Skullwalltula
    0x00A2 : 0x01C2,    #Gravestone
    0x00A3 : 0x00A2,    #Ruto
    0x0069 : 0x017F,    #I'M ON A BOAT
    0x0092 : 0x018D,    #Flying rubble
    0x00D4 : 0x00AB,    #Like like
    0x00EC : 0x00C2,    #Kakariko roof guy
    0x0133 : 0x00FF,    #windmill man
    0x013C : 0x0107,    #Runningman/postman
    0x016B : 0x0132,    #Dog
    0x0188 : 0x0143,    #Gossip stones
    0x0097 : 0x01B5,    #Gaurds
    0x00FE : 0x00D0,    #Zora (Swimming and normal)
    0x0167 : 0x0130,    #Gerudo walkers
    0x00C9 : 0x00A1,    #Gorons
    0x0183 : 0x0141,    #Wolfos
    0x0008 : 0x0006     #Guay
}

mm_to_oot_objs = {
#   MM     : OoT
    0x000C : 0x000E,    #Chests
    0x0009 : 0x000B,    #Wallmaster
    0x000A : 0x000C,    #Dodongo
    0x000B : 0x000D,    #Keese
    0x007D : 0x001A,    #Epona
    0x000F : 0x0013,    #Cucco
    0x0012 : 0x0016,    #Tektie
    0x0013 : 0x0017,    #???
    0x0014 : 0x0018,    #Peahat
    0x0017 : 0x001B,    #Dinolfos/Lizafos
    0x005F : 0x0076,    #Wooden post with red cloth
    0x014B : 0x001D,    #Zelda
    0x000E : 0x0012,    #Bubble
    0x00BC : 0x0022,    #???
    0x0020 : 0x0024,    #Skulltula
    0x002A : 0x0031,    #bombflowers
    0x0031 : 0x0039,    #deku baba
    0x003E : 0x0048,    #Warp portals
    0x0080 : 0x00A4,    #Torch stand
    0x0096 : 0x00BD,    #Heart container
    0x0040 : 0x004A,    #Deku scrub
    0x017E : 0x0059,    #Water
    0x0088 : 0x0070,    #Twisted hallway
    0x0051 : 0x005D,    #Bubble (bouncing skull)
    0x0061 : 0x007C,    #Tree
    0x006A : 0x008B,    #Beamos
    0x0153 : 0x002C,    #Flame Wall
    0x0009 : 0x000B,    #Floormaster
    0x0075 : 0x0098,    #ReDead
    0x0020 : 0x0024,    #Skullwalltula
    0x01C2 : 0x00A2,    #Gravestone
    0x00A2 : 0x00A3,    #Ruto
    0x017F : 0x0069,    #I'M ON A BOAT
    0x018D : 0x0092,    #Flying rubble
    0x00AB : 0x00D4,    #Like like
    0x00C2 : 0x00EC,    #Kakariko roof guy
    0x00FF : 0x0133,    #windmill man
    0x0107 : 0x013C,    #Runningman/postman
    0x0132 : 0x016B,    #Dog
    0x0143 : 0x0188,    #Gossip stones
    0x01B5 : 0x0097,    #Gaurds
    0x00D0 : 0x00FE,    #Zora (Swimming and normal)
    0x0211 : 0x00FE,    #Zora (guitarist)
    0x0216 : 0x00FE,    #Zora (drummer)
    0x0220 : 0x00FE,    #Zora (Pianist)
    0x022B : 0x00FE,    #Zora (Singer)
    0x0130 : 0x0167,    #Gerudo walkers
    0x00A1 : 0x00C9,    #Gorons
    0x0141 : 0x0183,    #Wolfos
    0x0006 : 0x0008,    #Guay
    0x0142 : 0x0184,    #Stalchildren
    0x0192 : 0x010A,    #Skullkid
    0x01B9 : 0x0002,    #beehive
    0x0133 : 0x0170,    #Crate
    0x0140 : 0x0182,    #Honey and darling
    0x0030 : 0x0038,    #Armos
    0x0153 : 0x002C,    #Firewall
    0x0267 : 0x0074,    #Cracked wall
    0x0234 : 0x00B1,    #Cracked wall
    0x0203 : 0x002C,    #cracked wall
    0x01E0 : 0x00F1,    #cracked wall
    0x0146 : 0x018B,    #Cow
    0x00A7 : 0x00E0,    #Cremia
    0x00B7 : 0x00D0,    #Malon/Romani
    0x0031 : 0x0039,    #Withered Deku Baba
    0x0157 : 0x006B,    #Icicles
    0x015E : 0x0039,    #Bio deku baba
    0x01E5 : 0x0168,    #Deku salesman
    0x01B6 : 0x0097,    #Gaurd
    0x01C3 : 0x0009,    #Poe
    0x01F1 : 0x006D,    #Big poe
    0x01AF : 0x0089,    #Dampe
    0x00E4 : 0x0114,    #Freezard
    0x0201 : 0x0017     #leever
}

ActorTypes = {
    0 : "type 0",
    1 : "1 (Prop)",
    2 : "2 (Link?)",
    3 : "3 (Bomb)",
    4 : "4 (NPC)",
    5 : "5 (Enemy)",
    6 : "6 (Prop)",
    7 : "7 (Item/Action)",
    8 : "8 (Miscellaneous)",
    9 : "9 (Boss)",
    10 : "type 10",
    11 : "11 (Door?)",
    12 : "type 12",
    13 : "type 13",
    14 : "type 14",
    15 : "type 15" }

def mkMapActor(endianess, num = 0, x = 0, y = 0, z = 0, xr = 0, yr = 0, zr = 0, var = 0):
    return pack("%sHhhhhhhH"% (endianess, num, x, y, x, xr, yr, zr, v))
def GenActorEntryFmt(endianess):
    """Returns FMT strings to be used with struct to unpack actor file pointers"""
    return "%sLLLLxxxxLLxxxx"	% endianess
def GenSceneEntryFmt(endianess):
    """Returns FMT strings to be used with struct to unpack scene file pointers"""
    return "%sLLLLL"		% endianess
def GenObjectEntryFmt(endianess):
    """Returns FMT strings to be used with struct to unpack object file pointers"""
    return "%sLL"		% endianess
def GenFileEntryFmt(endianess):
    """Returns FMT strings to be used with struct to unpack file pointers"""
    return "%sLLLL"		% endianess
def GenActorHeaderFmt(endianess):
    """Returns FMT strings to be used with struct to unpack actor headers"""
    return "%sLLLLL"		% endianess
def GenActorInfoFmt(endianess):
    """Returns FMT strings to be used with struct to unpack actor info (actor number, type, object)"""
    return "%sHBxxxxxH"		% endianess
def GenActorFileFmt(endianess,textlen,datalen,rodatalen,bsslen, no_rels):
    """Returns a FMT string to be used with struct to unpack a actor's parts"""
    return "%s%is%is%is%is%is"	% (endianess, textlen, datalen, rodatalen,
                                   bsslen, no_rels*4)

def FindFileTable(RomFile,endianess):
    """Returns filetable offset if found, else None"""
    FileTableOffset = None
    CurrentOffset = 0
    BuildInfo = None
    RomFile.seek(0)
    for i in range(0,0x20000,16):
        DD=RomFile.read(16)
        if len(DD.split("@srd")) == 2:
            CurrentOffset = RomFile.tell()
            RomFile.seek(RomFile.tell() - 16)
            BuildInfo = CleanBuildInfo(RomFile.read(0x30))
            break
    for i in range(0,0x80,16):
        CurrentOffset+=16
        RomFile.seek(CurrentOffset)
        DoubleWord = unpack( "%sQ" % endianess, RomFile.read(8) )[0]
        if DoubleWord == 0x0000000000001060:
            FileTableOffset = CurrentOffset
            break
    return FileTableOffset, BuildInfo

def CleanBuildInfo(RawBuildInfo):
    """Cleans raw build info (including the 0s)"""
    CleanedBuildInfo = ''
    for char in RawBuildInfo:
        if char == '\x00':
            CleanedBuildInfo += ' '
        else: CleanedBuildInfo += char
    del RawBuildInfo
    while CleanedBuildInfo[-1] == ' ':
        CleanedBuildInfo = CleanedBuildInfo[:-1]
    return CleanedBuildInfo

def FindNameTable(RomFile,endianess):
    """Returns nametable offset if found, else None"""
    ret=None
    for i in range(0x1060,0x10000,16):
        RomFile.seek(i)
        if unpack("%sQQ"%endianess,RomFile.read(16))== (0x6D616B65726F6D00, 0x626F6F7400000000):
            ret=i
            break
    return ret

def FindCode(RomFile,endianess):
    """Returns code's offsets if found, else None"""
    ret=None
    FileTableOff=FindFileTable(RomFile,endianess)[0]
    for i in range(0, 0x300, 16):
        RomFile.seek(FileTableOff+i)
        vst,ve,pst,pe=unpack(">LLLL",RomFile.read(16))
        if pe==0:
            RomFile.seek(ve-16)
            if unpack("%sQQ"%endianess,RomFile.read(16))==(0x6A6E8276E707B8E3,0x7D8A471D6A6E18F9):
                ret=(vst,ve)
                break
    return ret

def ScanForHierarchies(File, endianess, FileBank):
    """Finds hierarchies within a zelda 64 resource file"""
    hierarchies = []
    OldPos = File.tell()
    File.seek(0,2)
    FileEnd = File.tell()
    j = -1
    for i in range(0, FileEnd, 4):
        File.seek(i)
        CurrentWord = unpack("%sL" % endianess,
                             File.read(4))[0]
        if (CurrentWord >> 24 == FileBank and
            CurrentWord&3 == 0 and
            CurrentWord&0xFFFFFF < FileEnd):
            NoPts = unpack("%sB" % endianess,
                           File.read(1))[0]
            if NoPts < 255:
                for j in range((CurrentWord&0xFFFFFF),
                               (CurrentWord&0xFFFFFF)+ NoPts * 4, 4):
                    File.seek(j)
                    _CurrentWord = unpack("%sL" % endianess,
                                          File.read(4))[0]

                    if (_CurrentWord >> 24 != FileBank):
                        break
                    if (_CurrentWord&3 != 0):
                        break
                    if (_CurrentWord&0xFFFFFF > FileEnd):
                        break

                if j == (CurrentWord&0xFFFFFF)+ NoPts * 4 - 4:
                    hierarchies.append(i)

    File.seek(OldPos)
    return hierarchies

def ScanForAnimations(File, endianess, FileBank):
    """Finds  animations within a zelda 64 resource file"""
    return []

def FindEndOfFiles(File,SkipScene=0):
    """Finds the end offset within a ROM that is safe to write to"""
    End = 0
    FPos = FindFileTable( File ,">" )[0]+4
    Entry = -1
    while (Entry != 0):
        File.seek(FPos)
        Entry = unpack( ">L", File.read(4) )[0]
        if (Entry > End):
            End = Entry
        FPos+=16
    codeOff = FindCode( File,">" )[0]
    for i in range( codeOff + 0xF9440, codeOff + 0xFB5E0, 0x20 ):
        File.seek(i+4)
        Entry = unpack( ">L", File.read(4) )[0]
        if (Entry > End):
            End = Entry
    for i in range( codeOff + 0x10A6D0, codeOff + 0x10B360, 0x8 ):
        File.seek(i+4)
        Entry = unpack( ">L", File.read(4) )[0]
        if (Entry > End):
            End = Entry
    c=0
    for i in range( codeOff + 0x10CBB0, codeOff + 0x10CBB0 + (MAX_OOT_SCENE+1) * 0x14, 0x14 ):
        File.seek(i)
        Entry = unpack( ">LL", File.read(8) )
        if(c==SkipScene and End <= Entry[0]):
            End = Entry[0]
        else:
            if (Entry[1] > End):
                End = Entry[1]
            File.seek(Entry[0])
            command = -1
            while (command != 4):
                command,ent,off = unpack(">BBxxL",File.read(8))
            if ( command == 0x14 ):
                break
            off=(off & 0xFFFFFF) + Entry[0]
            File.seek(off)
            for i in range(ent):
                st,en = unpack(">LL", File.read(8) )
                if (en > End):
                    End = en
        c+=1
    return End

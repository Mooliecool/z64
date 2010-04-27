
from struct import unpack

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
supercoolvalue=0
def FindEndOfFiles(File,skipscene=-1):
    """Finds the end offset within a ROM that is safe to write to"""
    global supercoolvalue
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
    j=0
    for i in range( codeOff + 0x10CBB0, codeOff + 0x10CBB0 + 110 * 0x14, 0x14 ):
        if j!=skipscene:
            File.seek(i)
            Entry = unpack( ">LL", File.read(8) )
            if (Entry[1] > End):
                End = Entry[1]
            #Check maps of scenes
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
        else: #Nasty dirty hack! this is a way to get the offset that the scene pointer is at, to be used by area.py
            supercoolvalue=i
        j+=1
    return End

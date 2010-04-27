#!/usr/bin/python

from struct import pack, unpack
from sys import argv as argv__
from os import path, system
from z64lib import *


def fix_doors(inFile, outFile, OutFileOff, inFileOff, NoDoors, DestGame="OoT", loud = False):
    oldPos = inFile.tell()
    outFile.seek(OutFileOff)
    door = []
    fixed = 0
    
    for i in range(inFileOff, (NoDoors * 16) + inFileOff, 16):
        inFile.seek(i)
        door_ = unpack(">BBBBHhhhhH",inFile.read(16))
        door[:] = door_[:]
        if DestGame == "OoT":
            door[9] = int(door[9]*1.42222222222)
            try:
                door[4] = mm_to_oot_acts[door[4]]
                if (door[4] == 0x23):
                    door[-1] = 0x13F
                elif(door[4] == 0x2E):
                    print "%x"% door[-1]
                    door[-1]=0x32d
                fixed+=1
            except:
                if loud:
                    print "Unmapped door: %04X" % ( door[4] )
                door[4] = 0x0009
                door[-1] = 0
        elif DestGame == "MM":
            door[9] = int(door[9]*0.703125)
            try:
                door[4] = oot_to_mm_acts[door[4]]
                fixed+=1
            except:
                door[4] = 0x0005
        else:
            return -1
        outFile.write(pack(">BBBBHhhhhH", door[0], door[1], door[2],
                               door[3], door[4], door[5], door[6], door[7],
                               door[8], door[9] ))
    if loud:
        if NoDoors:
            print "Matched %.2f%% of doors with OoT parallels." % ( 100 * ( fixed / float( NoDoors ) ) )
        else:
            print "No objects to match!"
    inFile.seek(oldPos)
    return 0

def fix_actors(inFile, outFile, OutFileOff, inFileOff, NoActors, DestGame="OoT", loud = False):
    oldPos = inFile.tell()
    outFile.seek(OutFileOff)
    actor = []
    fixed = 0
    
    for i in range(inFileOff, (NoActors * 16) + inFileOff, 16):
        inFile.seek(i)
        actor_ = unpack(">HhhhhhhH",inFile.read(16))
        actor[:] = actor_[:]
        actor[0] = actor[0] & 0xFFF
        if DestGame == "OoT":
            for i in range(4,7,1):
                actor[i] *= 1.42222222222
            try:
                actor[0] = mm_to_oot_acts[actor[0]]
                #Set variables for certain actors:
                if actor[0]   == 0x0142:
                    actor[7] = 1
                elif actor[0] == 0x01CE:
                    actor[7] = 0
                elif actor[0] == 0x0162:    #Running man
                    actor[7] = 1
                elif actor_[0] == 0x235 or actor_[0] == 0x1DA:
                    actor[7] = 0xFFFE
                elif actor[0] == 0x1B0:
                    actor[7] = 0
                elif actor[0] == 0x1C6:
                    actor[7] = 0
                elif actor[0] == 0x1C5:
                    actor[7] = 0xFFFF
                elif actor[0] == 0x05D:
                    actor[7] = actor[7] & 7
                elif actor[0] == 0:
                    actor[7] = 0xFFF
                fixed+=1
            except:
                actor[0] = 0x003b
                actor[7] = 0#actor[7]&0xF
        elif DestGame == "MM":
            for i in range(3,6,1):
                actor[i] *= 0.703125
            try:
                actor[0] = oot_to_mm_acts[actor[0]]
                fixed+=1
            except:
                actor[0] = 0x0004
        else:
            return -1
        outFile.write(pack(">HhhhhhhH",actor[0], actor[1], actor[2],
                               actor[3], actor[4], actor[5], actor[6], actor[7]))
    if loud:
        if NoActors:
            print "Matched %.2f%% of actors with OoT parallels." % ( 100 * ( fixed / float( NoActors ) ) )
        else:
            print "No actors to match!"
    inFile.seek(oldPos)
    return 0

def fix_objects(inFile, outFile, OutFileOff, inFileOff, NoObjects, DestGame="OoT", loud = False):
    oldPos = inFile.tell()
    outFile.seek(OutFileOff)
    inFile.seek(inFileOff)
    fixed = 0
    objs = []
    objs_ = unpack( ">" + "H" * NoObjects, inFile.read( NoObjects * 2 ) )
    objs[:] = objs_[:]
    wobjs = []
    
    for i in range(NoObjects):
        obj = objs[i]
        if DestGame == "OoT":
            try:
                wobjs.append( mm_to_oot_objs[obj] )
                fixed+=1
            except:
                pass
        elif DestGame == "MM":
            try:
                wobjs.append( mm_to_oot_objs[obj] )
                fixed+=1
            except:
                pass
    for i in range( fixed ):
        outFile.write(pack( ">H", wobjs[i] ))
    for i in range( NoObjects - fixed ):
        outFile.write(pack( ">H", 0 ))
    if loud:
        if NoObjects:
            print "Matched %.2f%% of objects with OoT parallels." % ( 100 * ( fixed / float( NoObjects ) ) )
        else:
            print "No objects to match!"
    inFile.seek(oldPos)
    return fixed
    
    #

def fix_map(inFile, outFile, outFileOff, inFileOff, loud = False, DestGame="OoT", IsScene = False, safe = False, Music = False ):
    oldPos = inFile.tell()
    outFile.seek(outFileOff)
    fpos = inFileOff
    
    while (1):
        inFile.seek(fpos)
        ID = unpack(">B",inFile.read(1))[0]
        if (ID == 0xB):
            if safe:
                outFile.seek(outFileOff+(fpos-inFileOff)+1)
                outFile.write(pack(">B",0))
            else:
                NoObjects, LocalOff = unpack(">BxxL",inFile.read(7))
                LocalOff = LocalOff & 0xFFFFFF
                NoObjects = fix_objects(inFile, outFile, outFileOff+LocalOff, inFileOff+LocalOff, NoObjects, DestGame, loud)
                outFile.seek(outFileOff+(fpos-inFileOff)+1)
                if NoObjects > 0xF:
                    outFile.write(pack(">B",0xF))
                else:
                    outFile.write(pack(">B",NoObjects))
        elif (ID == 1 or ID == 0):
            if safe and ID == 1:
                outFile.seek(outFileOff+(fpos-inFileOff)+1)
                outFile.write(pack(">B",0))
            else:
                NoActors, LocalOff = unpack(">BxxL",inFile.read(7))
                LocalOff = LocalOff & 0xFFFFFF
                fix_actors(inFile, outFile, outFileOff+LocalOff, inFileOff+LocalOff, NoActors, DestGame, loud)
        elif (ID == 0xE):
            if safe:
                outFile.seek(outFileOff+(fpos-inFileOff)+1)
                outFile.write(pack(">B",0))
            else:
                NoDoors, LocalOff = unpack(">BxxL",inFile.read(7))
                LocalOff = LocalOff & 0xFFFFFF
                fix_doors(inFile, outFile, outFileOff+LocalOff, inFileOff+LocalOff, NoDoors, DestGame, loud)
        elif (IsScene == True and ID == 0x4):
            IsScene = unpack(">BxxL",inFile.read(7))
            IsScene = [IsScene[0],IsScene[1]]
            IsScene[1] = IsScene[1] & 0xFFFFFF
        elif (ID == 0x18 or ID == 0x12): #bai bai multiple object sets :)
            outFile.seek(outFileOff+(fpos-inFileOff))
            outFile.write(pack(">Q",0x0100000000000000))
        elif (ID == 0x11):
            enabled = unpack("xxxxxB",inFile.read(6))[0]&1
            outFile.seek(outFileOff+(fpos-inFileOff))
            if not enabled:
                outFile.write(pack(">Q",0x1100000001000000))
            else:
                outFile.write(pack(">Q",0x1100000000000100))
        elif ( ID == 3 ):
            offset = unpack(">xxxL",inFile.read(7))[0]
            outFile.seek( outFileOff + ( offset & 0xFFFFFF ) + 0x20)
            outFile.write( pack( ">L", 0 ) )
        elif ( ID == 0x15 and Music ):
            outFile.seek(outFileOff+(fpos-inFileOff)+7)
            outFile.write( pack( ">B", Music ) )
        elif (ID == 0x14):
            break
        fpos += 8
    if(not IsScene):
        try:
            while(1):
                infile.seek(fpos)
                if(infile.read(4) == "\xDE\x00\x00\x00" and unpack(">B", infile.read(1))[0] != 3 ):
                    infile.seek(fpos)
                    infile.write("\x00removed")
                fpos+=8
        except:
            pass
    inFile.seek(oldPos)
    if (IsScene != False):
        return IsScene
    else:
        return 0
    
def _int(string):
    if len(string)>2 and string[:2]=='0x':
        return int(string,16)
    else:
        return int(string)


    
def checkArgs(argv_):
    if ( len(argv_) < 5 ):
        return [-1, argv_]
    
    if ( (not path.exists(argv_[1])) | (not path.exists(argv_[2])) ):
        return [-2, argv_]
    
    try:
        argv_[3] = _int(argv_[3])
        argv_[4] = _int(argv_[4])
    except ValueError:
        return [-3, argv_]
    
    if ( ( argv_[3] > MAX_OOT_SCENE ) | ( argv_[4] > MAX_MM_SCENE ) ):
        return [-4, argv_]
    
    ret = 1
    
    if ( len(argv_) > 5 ):
        cnt = 5
        while ( cnt < len(argv_) ):
            if ( argv_[cnt] == "-q" ):
                ret = ret & 0xFFFFFFFFFFFE
            if ( argv_[cnt] == "-s" ):   #safe mode
                ret = ret | 0x100000000
            elif ( argv_[cnt] == "-o" ):
                cnt += 1
                ret = ret | ( _int(argv_[cnt]) & 0xFFFFFFF0 )
            elif ( argv_[cnt] == "-m" ):
                cnt += 1
                ret = ret | ( _int(argv_[cnt]) << 33 )
            cnt += 1
    return [ret, argv_]

def main(argv_):
    ret     = checkArgs(argv_)
    status  = ret[ 0 ]
    argv    = ret[ 1 ]
    if ( status < 0 ):
        usage = "python %s <rom to port to> <rom port from> <scene to port over> <scene to port> [options]" % ( argv[0] )
        if ( status == -1 ):
            raise IOError( "Not enough arguments. Usage:\n%s" % usage)
        elif ( status == -2 ):
            raise IOError( "Cannot open specified ROMs. Usage:\n%s" % usage)
        elif ( status == -3 ):
            print ValueError( "Cannot convert scene numbers to integers. Usage:\n%s" % usage)
        elif ( status == -4 ):
            raise OverflowError( "Scene numbers specified are too great.\nIf your ROM can handle bigger scene numbers, edit this script on lines 9 and 10. Usage:\n%s" % usage)
    try:
        BAD_MM_SCENES.index( argv[4] )
        raise ValueError("Chosen scene is a invalid Majora's Mask scene")
    except:
        pass
    inFile  = open( argv[2], "rb" )
    outFile = open( argv[1], "r+b" )
    inFile.seek(MM_SCENE_TABLE + argv[4] * 16)
    OldSceneOffs = unpack( ">LL", inFile.read(8) )
    safe = bool( status & 0x100000000 )
    Music = status >> 33
    if ( not bool( Music ) ):
        Music = False
    if ( status > 15 ):
        SceneStart = status & 0xFFFFFFF0
    else:
        SceneStart = FindEndOfFiles(outFile, argv[3])
    if ( status & 1 ):
        print ("Porting scene %i from ROM %s to scene %i of ROM %s at offset 0x%08X"
               % ( argv[4], argv[2], argv[3], argv[1], SceneStart ))
    SceneEnd = SceneStart + ( OldSceneOffs[1] - OldSceneOffs[0] )
    outFile.seek(SceneStart)
    inFile.seek(OldSceneOffs[0])
    outFile.write( inFile.read( OldSceneOffs[1] - OldSceneOffs[0] ) )
    if ( status & 1):
        print "Fixing scene file"
    Map_Infos = fix_map(inFile, outFile, SceneStart, OldSceneOffs[0], status & 1 ,"OoT", True , safe, Music)
    if ( status & 1):
        print ("Old offsets: %08X - %08X\nNew offsets: %08X - %08X"
               % ( OldSceneOffs[0], OldSceneOffs[1], SceneStart, SceneEnd ) )
    MapStart = (SceneEnd & 0xFFFFFFF0) + 0x10
    count = 0
    for ptr_offset in range(Map_Infos[1], Map_Infos[1] + Map_Infos[0] * 8, 8):
        if ( status & 1 ):
            print "Fixing map %i" % ( count )
        inFile.seek(OldSceneOffs[0]+ptr_offset)
        oldMapOffs = unpack( ">LL", inFile.read(8) )
        outFile.seek(MapStart)
        inFile.seek(oldMapOffs[0])
        outFile.write( inFile.read( oldMapOffs[1] - oldMapOffs[0] ) )
        MapEnd = outFile.tell()
        fix_map( inFile, outFile, MapStart, oldMapOffs[0], status & 1, "OoT", False, safe )
        outFile.seek( SceneStart + ptr_offset )
        outFile.write( pack( ">LL" , MapStart, MapEnd ) )
        if ( status & 1 ):
            print ( "Old offsets: %08X - %08X\nNew offsets: %08X - %08X"
                   % ( oldMapOffs[0], oldMapOffs[1], MapStart, MapEnd ) )
        MapStart = MapEnd
        count += 1
        
    outFile.seek( OOT_SCENE_TABLE + argv[3] * 0x14 )
    outFile.write( pack( OOT_SCENE_FMT, SceneStart, SceneEnd, 0, 0, 0 ) )
    if ( status & 1 ):
        print "Done. Using space %08X - %08X for maps and scenes." % ( SceneStart, MapEnd )
    inFile.close()
    outFile.close()
    return SceneStart, MapEnd

if __name__ == "__main__":
    main(argv__)

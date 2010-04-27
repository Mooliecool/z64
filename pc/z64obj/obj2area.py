#!/usr/bin/python

import sys
if (sys.hexversion & 0xFFFF0000 > 0x02060000): print "WARNING: Unsupported python version, program may malfunction"
from struct import calcsize, pack
from os import path, system, remove
import wotf
import z64lib

usewine = not sys.platform in ["win32", "win64", "cygwin"]
trisOffset,vertsOffset, waterOffset=0,0,0
ZELDATIME=-1
NOTIME=-1
SafeObjects = [0x32,0xE,0xA,0x2C,0x9,0x7,0x36,0xB,0,0,0,0,0,0,0,0,0]

def _int(string_int):
    if (string_int[:2] == '0x'):
        return int(string_int, 16)	#Hexadecimal (0x)
    elif (string_int[:1] == "$"):
        return int(string_int[1:], 16)	#Also Hexadecimal ($)
    elif (string_int[:1] == 'o'):
        return int(string_int, 8)	#Octal (o)
    else:
        return int(string_int, 10)	#Decimal (default)

def align(data, bytes = 4):
    return "%s%s"%(data, "\x00" * (bytes - (len(data)&(bytes-1))) ) if len(data)&(bytes-1) else data

def ObjToCollision(Object, bank=2, offset=0, nowaterboxes=0, scale=1,ReverseFaces=False):
    global trisOffset,vertsOffset, waterOffset
    maxs = [-9000, -9000, -9000]
    mins = [9000, 9000, 9000]
    #I know this code is ugly. gtfo
    wotf.resetvars()
    bank=bank<<24
    f=open(Object,"r")
    wotf.reverse_faces=ReverseFaces
    wotf.fdir=path.split(Object)[0]
    if wotf.fdir: wotf.fdir+=wotf.slash
    object_data = wotf.fix_obj(f.read())
    wotf.parse_obj(object_data)
    f.close()
    i=0
    vert_count,tri_count=0,0
    collision_types,collision_tris,verts,water="","","",""
    for entry in wotf.mtls:
        collision_types+="\x00\x00\x00\x00\x00\x00\x00\x00"
        for triangle in entry[7]:
            collision_tris+=pack(">HHHHQ",i,triangle[0][0],triangle[0][1],triangle[0][2],0)
            tri_count+=1
        i+=1
    range3=range(3)
    for vert in wotf.verts:
        verts+=pack(">hhh",int(vert[0]*scale),int(vert[1]*scale),int(vert[2]*scale))
        for i in range3:
            if int(vert[i]*scale) > maxs[i]:
                maxs[i]=int(vert[i]*scale)+1
            elif int(vert[i]*scale) < mins[i]:
                mins[i]=int(vert[i]*scale)-1
        vert_count+=1
    verts=align(verts,8)
    for i in range(0,nowaterboxes*400,400):
        water+=pack(">hhhhhxxxxBB",0+i,0,0+i,400+i,400+i,1,0)
    typesOffset	= offset	|bank
    trisOffset	= typesOffset+len(collision_types)	|bank
    vertsOffset	= trisOffset+len(collision_tris)	|bank
    waterOffset	= vertsOffset+len(verts)	|bank
    headerOffset	= waterOffset+len(water)	|bank
    camDataOffset	= 0
    data=collision_types+collision_tris+verts+water
    del collision_types,collision_tris,verts,water
    data+=pack(">hhhhhhHxxLHxxLLLHxxL",mins[0],mins[1],mins[2],maxs[0],maxs[1],maxs[2],
    	vert_count, vertsOffset, tri_count, trisOffset, typesOffset, camDataOffset,nowaterboxes, waterOffset if nowaterboxes else 0)
    return data, headerOffset
    
def BuildScene(NoLinks, NoDoors, MapOffsetsList, CollisionObject, SkyBoxEnabled=True,NoWaterBoxes=0,CollisionScale=1,Music=2,ReverseFaces=False):
    MapList, DoorList, EntranceIndex, LinkList = "", "", "", ""
    header = ">LxxxBBBxxLBBxxLQLLLLQBBxxLLBxxxQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ"
    bank=2
    
    for MapOffsets in MapOffsetsList:
        MapList	+=pack(">LL",MapOffsets[0],MapOffsets[1])
    for i in range(NoDoors):
        DoorList	+=pack(">BbBbHhhhHH", 0, -1, 0, -1, 0x0023, 0, 0, -100, 0, 0x013F)
    for i in range(NoLinks):
        LinkList	+=pack(">HhhhHHHH", 0, 0, 0, 0, 0, 0, 0, 0x0FFF)
        EntranceIndex	+=pack(">Bx",i)
    EntranceIndex	= align(EntranceIndex,16)
    
    #Calculate offsets
    MapListOffset	= calcsize(header)|(bank<<24)
    DoorListOffset	= MapListOffset+len(MapList)
    LinkListOffset	= DoorListOffset+len(DoorList)
    EntranceIndexOffset	= LinkListOffset+len(LinkList)
    #Convert Collision
    Collision, CollisionHeaderOffset = ObjToCollision(CollisionObject, bank, EntranceIndexOffset+len(EntranceIndex),NoWaterBoxes,CollisionScale,ReverseFaces)
    
    data = pack(header, 
	0x15020000,	Music,
	0x04,	len(MapOffsetsList),	MapListOffset,
	0x0E,	NoDoors,	DoorListOffset,
	0x1900000000000003,
	0x03000000,	CollisionHeaderOffset,
	0x06000000,	EntranceIndexOffset,
	0x0701000000000002,
	0x00,	NoLinks,	LinkListOffset,
	0x11000000,	int(bool(SkyBoxEnabled)),		0x1300000002000068,
	0x0F0C000002000088, 	0x1400000000000000, 	0x0000000000000000, 	0x0000000000000000,
	0x0000000000000000, 	0x0000000000000000, 	0x0000000000000000, 	0x462D39494949B49A,
	0x8AB7B7B714143C8C, 	0x786E07E13200695A, 	0x5A494949FFFFF0B7, 	0xB7B732325A646478,
	0x07E43200785A0049, 	0x4949FA8732B7B7B7, 	0x1E1E3C78463207E3, 	0x3200284664494949,
	0x141423B7B7B73232, 	0x6400001E07E03200, 	0x3C2846494949501E, 	0x3CB7B7B750329646,
	0x2B2DFFD232004B5A, 	0x6449494937FFF0B7, 	0xB7B70A96BE145A6E, 	0xFFD232003C285049,
	0x49493C4B96B7B7B7, 	0x3C3796321E1EFFD2, 	0x3200002850494949, 	0x14324BB7B7B73264,
	0x96000A14FFD23200, 	0x605C5C000000A086, 	0x760000001E0A0A64, 	0x5A5007DA32005F50,
	0x5000000091918200, 	0x00002828505F5F55, 	0x07DA320050464600, 	0x0000964623000000,	# :(
	0x0A0A19230A0A07DC, 	0x320046465A000000, 	0x00000F0000001E1E, 	0x5000000A07DC3200,
	0x0D5C021EFA740B93, 	0x021EFA650B66020F, 	0xFDD808E6021EFE06, 	0x08E6012BFE06056C,
	0x00EDFE830572008A, 	0xFE8302920084000D, 	0xFFFB00840007FFFD, 	0x0021000DFB8C001B,
	0x000DFAFB00210090, 	0xFAF50021019DFAF7, 	0xFFC601B1FAF1FFCC, 	0x0524FA67FFC60709,
	0xF853FFC607BBF674, 	0xFFCC06B406E400F3, 	0xFE3F06DC00E2FB2B, 	0x055200F3FB2B0552,
	0x0078FB2B04E00078, 	0xFB2B04D70078FEF3, 	0x04D00078FBAFFF97, 	0x0088FBB9FF970078,
	0xFD1AFF8E001DFD1A, 	0xFF97002600130000, 	0x0000000000000000	)
    data+= MapList
    del MapList
    data+= DoorList
    del DoorList
    data+= LinkList
    del LinkList
    data+= EntranceIndex
    del EntranceIndex
    data+= Collision
    del Collision
    return align(data,16)

def BuildSceneFile(OutName, NoLinks, NoDoors, MapOffsetsList, CollisionObject, SkyBoxEnabled=True,NoWaterBoxes=0,CollisionScale=1,Music=2,ReverseFaces=False):
    global trisOffset,vertsOffset, waterOffset
    scene=open(OutName, "wb")
    #Build scene
    scene.write(BuildScene(NoLinks, NoDoors, MapOffsetsList, CollisionObject, SkyBoxEnabled,NoWaterBoxes,CollisionScale,Music,ReverseFaces))
    scene.close()
    #Fix collision
    cini=open("collision_fixer.ini","w")
    cini.write("""'6-byte\n%X\n%X\n'16-byte\n%X\n%X"""%(vertsOffset&0xFFFFFF,waterOffset&0xFFFFFF,trisOffset&0xFFFFFF,vertsOffset&0xFFFFFF))
    cini.close()
    rini=open("romname.ini","w")
    rini.write("'Rom you want to edit\n%s\n'Unmodified rom needed for collision fixer\n%s"%(OutName,OutName))
    rini.close()
    
    try:
        open("collision_fixer.exe","r").close()
        if usewine:
            try:
                system("wine collision_fixer.exe")
            except:
                print "ERROR - Collision cannot be fixed unless Wine is installed,\nas collision_fixer has not been compiled for *nix platforms\nGood luck fixing collision by hand!!!"
        else:
            system("collision_fixer.exe")
    except:
        print "ERROR - collision_fixer.exe is not present, collision cannot\nbe fixed. Put it in the same directory as this script"
    remove("collision_fixer.ini")
    remove("romname.ini")


def BuildMap(MeshObject, MeshScale=1,NoActors=0, NoObjects=0, StartTime=ZELDATIME, TimeSpeed=NOTIME,SkyBoxDisabled=False,ReverseFaces=False):

    header=">QQLBBxxLhbxLLBBxxLBBxxLQ"
    meshHeader=">xBxxLLLxxxxBxxxxxxxxxxx"
    bank=3
    
    ObjectList, ActorList="", ""
    for i in range(NoObjects&0xF):
        ObjectList+=pack(">H",SafeObjects[i])
    ObjectList=align(ObjectList,8)
    for i in range(NoActors&0x7F):
        ActorList+=pack(">HhhhHHHH", 8, 0, 0, i*30, 0, 0, 0, i&7)
    #Calculate offsets
    ObjectOffset=calcsize(header)+(bank<<24)
    ActorOffset=ObjectOffset+len(ObjectList)
    MeshHeaderOffset=ActorOffset+len(ActorList)
    MeshOffset=MeshHeaderOffset+calcsize(meshHeader)
    
    #Convert Mesh
    wotf.resetvars()
    f=open(MeshObject,"r")
    wotf.reverse_faces=ReverseFaces
    wotf.fdir=path.split(MeshObject)[0]
    if wotf.fdir: wotf.fdir+=wotf.slash
    object_data = wotf.fix_obj(f.read())
    f.close()
    wotf.parse_obj(object_data)
    del object_data
    wotf.vtx_scale = [MeshScale,MeshScale,MeshScale]
    MeshData, DisplayListOffset, TexInfo = wotf.ParsedMeshToOoT(wotf.mtls,wotf.verts,wotf.texcoords,wotf.normals, bank, MeshOffset, bank, MeshOffset)
    del TexInfo
    
    data=pack(header,
	0x1600000000000000,
	0x0801000000000000,
	0x12000000, int(SkyBoxDisabled), int(SkyBoxDisabled),
	0x10000000, StartTime, TimeSpeed,
	0x0A000000, MeshHeaderOffset,
	0x0B, NoObjects, ObjectOffset,
	0x01, NoActors, ActorOffset,
	0x1400000000000000)
    data+=ObjectList
    del ObjectList
    data+=ActorList
    del ActorList
    data+=pack(meshHeader, 1, MeshHeaderOffset+0xC, MeshHeaderOffset+0x14, DisplayListOffset, 1)
    data+=MeshData
    del MeshData
    return align(data,16)

def BuildMapFile(OutName, MeshObject, MeshScale=1, NoActors=0, NoObjects=0, StartTime=ZELDATIME, TimeSpeed=NOTIME,SkyBoxDisabled=False,ReverseFaces=False):
    Map=open(OutName,"wb")
    Map.write(BuildMap(MeshObject, MeshScale, NoActors, NoObjects, StartTime, TimeSpeed,SkyBoxDisabled,ReverseFaces))
    Map.close()

def BuildArea():
    #TODO: less ghetto version that uses command line args. At least this version will make SirTopHat happy. And 1000 other noobs.
    Rom		= open(raw_input("Zelda 64 Debug ROM: "),"r+b")
    SceneObj	= raw_input("Wavefront obj file for collision: ")
    try:open(SceneObj).close()
    except:raise IOError("Cannot open specified file")
    CollisionScale=float(raw_input("Scale: "))
    ReverseFaces= {"y":True, "n":False}[raw_input("Reverse faces? (y/n): ").lower()[0]]
    SceneNumber	= _int(raw_input("Area number (0-109): "))
    if (SceneNumber > 109): raise ValueError("Scene number too high; aborting area build")
    Music	= _int(raw_input("Music value: "))&255
    NoLinks	= _int(raw_input("Number of start positions: "))&0x3F
    NoDoors	= _int(raw_input("Number of tansition actors: "))&0x7F
    NoWaters	= _int(raw_input("Number of water boxes: "))&0x7F
    Offset	= z64lib.FindEndOfFiles(Rom, SceneNumber)
    _Offset	= Offset
    CompiledData=""
    MapOffsets=[]
    
    if (raw_input("Use different obj for mesh, or multiple mesh map?(y/n): ").lower()[0]=="y"):
        maps=[]
        i=0
        print "Enter nothing for `Obj:` when done"
        while 1:
            print "Map %i"%i
            mapObj=raw_input(" Obj: ")
            if not mapObj: break
            try: open(mapObj).close()
            except: raise IOError("Cannot open specified file")
            if mapObj:
                maps.append([mapObj,
                	float(raw_input(" Scale: ")),
                	_int(raw_input(" # Actors: ")),
                	_int(raw_input(" # Objects: "))&15,
                	{"y":True, "n":False}[raw_input(" Indoor? (y/n): ").lower()[0]],
                	{"y":True, "n":False}[raw_input(" Reverse faces? (y/n): ").lower()[0]] ])
            i+=1
    else: maps=[[SceneObj, CollisionScale, _int(raw_input(" # Actors: ")), _int(raw_input(" # Objects: "))&15,
    		{"y":True, "n":False}[raw_input(" Indoor? (y/n): ").lower()[0]], ReverseFaces ]]
    
    #Compile map(s)
    for Map in maps:
        CompiledMap = BuildMap(Map[0], Map[1], Map[2], Map[3], ZELDATIME, NOTIME, Map[4], Map[5])
        MapOffsets.append([Offset,Offset+len(CompiledMap)])
        Offset=Offset+len(CompiledMap)
        CompiledData+=CompiledMap
        del CompiledMap
    
    #Compile scene
    CompiledScene = BuildScene(NoLinks, NoDoors, MapOffsets, SceneObj, True, NoWaters, CollisionScale, Music,ReverseFaces)
    SceneOffsets = [Offset,Offset+len(CompiledScene)]
    CompiledData+=CompiledScene
    del CompiledScene
    
    #Write crap to ROM
    Rom.seek(_Offset)
    Rom.write(CompiledData)
    del CompiledData
    Rom.seek(z64lib.supercoolvalue)
    Rom.write(pack(">LLxxxxxxxxxxxx", SceneOffsets[0], SceneOffsets[1]))

    #Fix collision
    cini=open("collision_fixer.ini","w")
    cini.write("""'6-byte\n%X\n%X\n'16-byte\n%X\n%X"""%((vertsOffset&0xFFFFFF)+SceneOffsets[0], (waterOffset&0xFFFFFF)+SceneOffsets[0],
    							(trisOffset&0xFFFFFF)+SceneOffsets[0], (vertsOffset&0xFFFFFF)+SceneOffsets[0]))
    cini.close()
    rini=open("romname.ini","w")
    rini.write("'Rom you want to edit\n%s\n'Unmodified rom needed for collision fixer\n%s"%(Rom.name,Rom.name))
    rini.close()
    Rom.close()
    
    try:
        open("collision_fixer.exe","r").close()
        if usewine:
            try:
                system("wine collision_fixer.exe")
            except:
                print "ERROR - Collision cannot be fixed unless Wine is installed,\nas collision_fixer has not been compiled for *nix platforms\nGood luck fixing collision by hand!!!"
        else:
            system("collision_fixer.exe")
    except:
        print "ERROR - collision_fixer.exe is not present, collision cannot  be fixed. Put it in the same directory as this script"
    remove("collision_fixer.ini")
    remove("romname.ini")
    #Done!

if __name__ == "__main__": BuildArea()

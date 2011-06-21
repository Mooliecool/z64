from visual import *
from sys import argv
from struct import pack, unpack
import time
def _int(string):
    if len(string)>2 and string[:2]=='0x':
        return int(string,16)
    else:
        return int(string)
    
def main():
    #Setup file
    try:
        SETUP=open("zappy_settings","r")
    except:
        SETUP=open("zappy_settings","w")
        SETUP.write('''SCENE_TABLE	0xBA0BB0	#MM scene table:0xC5A280 (Decompressed US ROM)\nBG_R	0x33	#RGB BG colors are 0-255, hex or decimal\nBG_G	0x7F\nBG_A	0xB1\nFPS	50\nCAMSPEED	17\nGAME_VER	OOT	#Either OOT or MM''')
        SETUP.close()
        SETUP=open("zappy_settings","r")
    #Default variables (in the case that they are not defined in the setup file)
    SETUP_INFO = [0xBA0BB0,0x33,0x7F,0xB1,50,17,"OOT"]
    #Parse setup file
    for line in SETUP:
        if line[-1]=='\n':
            line=line[:-1]  #Remove newline
        try:
            line=line[:line.index("#")] #Strip comments
        except:
            pass
        if len(line)>14 and line[0:11]=="SCENE_TABLE":
            SETUP_INFO[0] = _int(line.split('\t')[1])
        elif len(line)>5 and line[:2]=="BR":
            if line[3]=="R":
                SETUP_INFO[1] = _int(line.split('\t')[1])
            elif line[3]=="G":
                SETUP_INFO[2] = _int(line.split('\t')[1])
            elif line[3]=="B":
                SETUP_INFO[3] = _int(line.split('\t')[1])
        elif len(line)>4 and line[:3]=="FPS":
            SETUP_INFO[4] = _int(line.split('\t')[1])
        elif len(line)>9 and line[:8]=='CAMSPEED':
            SETUP_INFO[5] = _int(line.split('\t')[1])
        elif len(line)>9 and line[:8]=='GAME_VER':
            SETUP_INFO[6] = line.split('\t')[1].upper()
    SETUP.close()
    
    #User defined variables
    ROM = open(argv[1],"r+b")
    SCENE = int(argv[2])
    MAP = int(argv[3])
    if len(argv) == 5:
        SET = int(argv[4])
    else:
        SET = 0
    
    #Other variables
    SCENE_ACTORS = []
    MAP_ACTORS = []
    LINK_ACTORS = []
    OBJECTS = []
    WATER_BOXES = []

    OLD_SCENE_ACTORS = []
    OLD_MAP_ACTORS = []
    OLD_LINK_ACTORS = []
    OLD_WATER_BOXES = []
    OLD_OBJECTS = []

    CURRENT_SCENE_ACTOR = 0
    CURRENT_LINK_ACTOR = 0
    CURRENT_MAP_ACTOR = 0
    CURRENT_OBJECT = 0
    CURRENT_WATER_BOX = 0

    CURRENT_ACTOR_SET = 1
    CURRENT_INCREMENT = 1
    
    #Camera stuff
    INITIAL_CAM_POS_SET = 1
    ROT_DIVISOR = 10430.378349109
    CAMSPEED = SETUP_INFO[5]
    
    #Rendering vars
    VERTS = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    VERT_COLORS = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

    #Get scene offsets
    if SETUP_INFO[6] == "OOT":
        ROM.seek(SETUP_INFO[0]+SCENE*0x14)
    else:
        ROM.seek(SETUP_INFO[0]+SCENE*0x10)
    SCENE_OFFS = unpack(">LL",ROM.read(8))

    #Main window
    scene = display(width=800, height=600, visible = 0, background=(SETUP_INFO[1]/255., SETUP_INFO[2]/255., SETUP_INFO[3]/255.), userzoom = 0)
    
    #FPS limitation
    rate(SETUP_INFO[4])

    #Get scene header offsets
    if SET > 0:
        ROM.seek(SCENE_OFFS[0])
        if unpack(">B", ROM.read(1))[0]==0x18:
            ROM.seek(SCENE_OFFS[0]+4)
            ROM.seek(SCENE_OFFS[0]+(unpack(">L",ROM.read(4))[0]&0xFFFFFF)+(8*(SET-1)))
            SCENE_ST_OFF=unpack(">L",ROM.read(4))[0]&0xFFFFFF
        else:
            SCENE_ST_OFF=0
    else:
        SCENE_ST_OFF=0
        
    #Get map offsets, set up scene actors, waterboxes
    SCENE_POS=SCENE_OFFS[0]+SCENE_ST_OFF
    COMMAND = (-1,-1)
    while COMMAND[0]!=0x14:
        ROM.seek(SCENE_POS)
        COMMAND = unpack(">BBHL",ROM.read(8))
        if COMMAND[0] == 4:
            ROM.seek(SCENE_OFFS[0]+(COMMAND[3]&0xFFFFFF)+MAP*8)
            MAP_OFFS = unpack(">LL",ROM.read(8))
        elif COMMAND[0] == 0xE:
            SCENE_ACTOR_OFF = SCENE_OFFS[0]+(COMMAND[3]&0xFFFFFF)
            ROM.seek(SCENE_ACTOR_OFF)
            for i in range(COMMAND[1]):
                SCENE_ACTOR = []
                SCENE_ACTOR_TURPLE = unpack(">BBBBHhhhhH",ROM.read(16))
                SCENE_ACTOR[0:10] = SCENE_ACTOR_TURPLE[0:10]
                SCENE_ACTOR.append(frame(pos=(0,0,0)))
                curve(frame=SCENE_ACTOR[-1], pos=[(30,0,0),(-30,0,0)],color=(0,0,1))
                curve(frame=SCENE_ACTOR[-1], pos=[(0,30,0),(0,-30,0)],color=(0,1,0))
                curve(frame=SCENE_ACTOR[-1], pos=[(0,0,30),(0,0,-30)],color=(1,0,0))
                box(frame=SCENE_ACTOR[-1],pos=(0,0,0),length=10,height=10,width=10,color=(.64,.26,.26))
                SCENE_ACTOR[-1].pos=SCENE_ACTOR_TURPLE[5:8]
                SCENE_ACTOR[-1].rotate(angle=SCENE_ACTOR_TURPLE[-2]/ROT_DIVISOR,axis=(0,1,0))
                SCENE_ACTORS.append(SCENE_ACTOR)
                del SCENE_ACTOR_TURPLE
                del SCENE_ACTOR
        elif COMMAND[0] == 0:
            LINK_ACTOR_OFF = SCENE_OFFS[0]+(COMMAND[3]&0xFFFFFF)
            ROM.seek(LINK_ACTOR_OFF)
            for i in range(COMMAND[1]):
                LINK_ACTOR = []
                LINK_ACTOR_TURPLE = unpack(">HhhhhhhH",ROM.read(16))
                LINK_ACTOR[0:9] = LINK_ACTOR_TURPLE[0:9]
                LINK_ACTOR.append(frame(pos=(0,0,0)))
                curve(frame=LINK_ACTOR[-1], pos=[(30,0,0),(-30,0,0)],color=(0,0,1))
                curve(frame=LINK_ACTOR[-1], pos=[(0,30,0),(0,-30,0)],color=(0,1,0))
                curve(frame=LINK_ACTOR[-1], pos=[(0,0,30),(0,0,-30)],color=(1,0,0))
                box(frame=LINK_ACTOR[-1],pos=(0,0,0),length=10,height=10,width=10,color=(0,1,0))
                LINK_ACTOR[-1].pos=LINK_ACTOR_TURPLE[1:4]
                LINK_ACTOR[-1].rotate(angle=LINK_ACTOR_TURPLE[-4]/ROT_DIVISOR,axis=(1,0,0))
                LINK_ACTOR[-1].rotate(angle=LINK_ACTOR_TURPLE[-3]/ROT_DIVISOR,axis=(0,1,0))
                LINK_ACTOR[-1].rotate(angle=LINK_ACTOR_TURPLE[-2]/ROT_DIVISOR,axis=(0,0,1))
                LINK_ACTORS.append(LINK_ACTOR)
                del LINK_ACTOR_TURPLE
                del LINK_ACTOR
        elif COMMAND[0] == 3:
            WATER_OFF = SCENE_OFFS[0]+(COMMAND[3]&0xFFFFFF)+0x24
            ROM.seek(WATER_OFF)
            NO_WATERBOXES = unpack(">H",ROM.read(2))[0]
            ROM.seek(WATER_OFF+4)
            WATER_BOX_OFF = SCENE_OFFS[0]+(unpack(">L",ROM.read(4))[0]&0xFFFFFF)
            ROM.seek(WATER_BOX_OFF)
            for i in range(NO_WATERBOXES):
                WATER_BOX=[]
                WATER_BOX_TURPLE = unpack(">hhhHHHL",ROM.read(16))
                WATER_BOX[0:7] = WATER_BOX_TURPLE[0:7]
                WATER_BOX.append(box(pos=(WATER_BOX[0]+(WATER_BOX[3]/2.),WATER_BOX[1]-16384,WATER_BOX[2]+(WATER_BOX[4]/2.)),
                                     length=WATER_BOX[3],height=32768,width=WATER_BOX[4],color = (.1,.1,.9)))
                WATER_BOXES.append(WATER_BOX)
                del WATER_BOX
                del WATER_BOX_TURPLE
        SCENE_POS+=8

    #Get scene header offsets
    if SET > 0:
        ROM.seek(MAP_OFFS[0])
        if unpack(">B", ROM.read(1))[0]==0x18:
            ROM.seek(MAP_OFFS[0]+4)
            ROM.seek(MAP_OFFS[0]+(unpack(">L",ROM.read(4))[0]&0xFFFFFF)+(8*(SET-1)))
            MAP_ST_OFF=unpack(">L",ROM.read(4))[0]&0xFFFFFF
        else:
            MAP_ST_OFF=0
    else:
        MAP_ST_OFF=0
        
    #Set up map actors and objects
    MAP_POS=MAP_OFFS[0]+MAP_ST_OFF
    COMMAND = (-1,-1)
    while COMMAND[0]!=0x14:
        ROM.seek(MAP_POS)
        COMMAND = unpack(">BBHL",ROM.read(8))
        if COMMAND[0] == 0xB:
            OBJECT_OFF = MAP_OFFS[0]+(COMMAND[3]&0xFFFFFF)
            ROM.seek(OBJECT_OFF)
            OBJECTS_TURPLE = unpack(">%s"%('H'*COMMAND[1]), ROM.read(COMMAND[1]<<1))
            OBJECTS[0:COMMAND[1]+1] = OBJECTS_TURPLE[0:COMMAND[1]+1]
        elif COMMAND[0] == 1:
            MAP_ACTOR_OFF = MAP_OFFS[0]+(COMMAND[3]&0xFFFFFF)
            ROM.seek(MAP_ACTOR_OFF)
            for i in range(COMMAND[1]):
                MAP_ACTOR = []
                MAP_ACTOR_TURPLE = unpack(">HhhhhhhH",ROM.read(16))
                MAP_ACTOR[0:9] = MAP_ACTOR_TURPLE[0:9]
                MAP_ACTOR.append(frame(pos=(0,0,0)))
                curve(frame=MAP_ACTOR[-1], pos=[(30,0,0),(-30,0,0)],color=(0,0,1))
                curve(frame=MAP_ACTOR[-1], pos=[(0,30,0),(0,-30,0)],color=(0,1,0))
                curve(frame=MAP_ACTOR[-1], pos=[(0,0,30),(0,0,-30)],color=(1,0,0))
                box(frame=MAP_ACTOR[-1],pos=(0,0,0),length=10,height=10,width=10,color=(0,0,1))
                MAP_ACTOR[-1].pos=MAP_ACTOR_TURPLE[1:4]
                MAP_ACTOR[-1].rotate(angle=MAP_ACTOR_TURPLE[-4]/ROT_DIVISOR,axis=(1,0,0))
                MAP_ACTOR[-1].rotate(angle=MAP_ACTOR_TURPLE[-3]/ROT_DIVISOR,axis=(0,1,0))
                MAP_ACTOR[-1].rotate(angle=MAP_ACTOR_TURPLE[-2]/ROT_DIVISOR,axis=(0,0,1))
                MAP_ACTORS.append(MAP_ACTOR)
                del MAP_ACTOR_TURPLE
                del MAP_ACTOR
        MAP_POS+=8

    #Render Map
    MAP_POS = MAP_OFFS[0]
    COMMAND = (-1,-1)
    DISPLAYLIST = 0
    while MAP_POS < MAP_OFFS[1]:
        ROM.seek(MAP_POS)
        COMMAND = unpack(">BBHL",ROM.read(8))
        if DISPLAYLIST:
            try:
                if COMMAND[0] == 1:
                    ROM.seek(MAP_OFFS[0]+(COMMAND[3]&0xFFFFFF))
                    for i in range(32):
                        ROM.seek(MAP_OFFS[0]+(COMMAND[3]&0xFFFFFF)+i*16)
                        VERTS[i]=unpack(">hhh",ROM.read(6))
                        ROM.seek(MAP_OFFS[0]+(COMMAND[3]&0xFFFFFF)+(i*16)+12)
                        VERT_COLORS[i]=unpack(">BBB",ROM.read(3))
                        VERT_COLORS[i]=(VERT_COLORS[i][0]/127.,VERT_COLORS[i][1]/127.,VERT_COLORS[i][2]/127.)
                if COMMAND[0] == 6:
                    faces(pos=[VERTS[COMMAND[1]>>1],VERTS[COMMAND[2]>>9],VERTS[(COMMAND[2]&0xFF)>>1]],color=(VERT_COLORS[COMMAND[1]>>1],VERT_COLORS[COMMAND[2]>>9],VERT_COLORS[(COMMAND[2]&0xFF)>>1]))
                    faces(pos=[VERTS[COMMAND[3]>>17],VERTS[(COMMAND[3]>>9)&0xFF],VERTS[(COMMAND[3]&0xFF)>>1]],color=(VERT_COLORS[COMMAND[3]>>17],VERT_COLORS[(COMMAND[3]>>9)&0xFF],VERT_COLORS[(COMMAND[3]&0xFF)>>1]))
                    curve(pos=[VERTS[COMMAND[1]>>1],VERTS[COMMAND[2]>>9],VERTS[(COMMAND[2]&0xFF)>>1]],color=(1,0,0))
                    curve(pos=[VERTS[COMMAND[3]>>17],VERTS[(COMMAND[3]>>9)&0xFF],VERTS[(COMMAND[3]&0xFF)>>1]],color=(1,0,0))
                if COMMAND[0] == 5:
                    faces(pos=[VERTS[COMMAND[1]>>1],VERTS[COMMAND[2]>>9],VERTS[(COMMAND[2]&0xFF)>>1]],color=(VERT_COLORS[COMMAND[1]>>1],VERT_COLORS[COMMAND[2]>>9],VERT_COLORS[(COMMAND[2]&0xFF)>>1]))
                    curve(pos=[VERTS[COMMAND[1]>>1],VERTS[COMMAND[2]>>9],VERTS[(COMMAND[2]&0xFF)>>1]],color=(1,0,0))
                if COMMAND[0] == 0xDF:
                    DISPLAYLIST = 0
            except:
                DISPLAYLIST = 0
        elif COMMAND[0]==0xE7 and COMMAND[1:4]==(0,0,0):
            DISPLAYLIST = 1
        MAP_POS+=8
    scene.range = 3
    scene.autoscale=0   #fixes problems with newer versions of vpython
    try:
        OLD_CAM_VALS = (scene.center , scene.forward , scene.range )
    except:
        pass
    OLD_SCENE_ACTORS[:] = SCENE_ACTORS[:]
    OLD_MAP_ACTORS[:] = MAP_ACTORS[:]
    OLD_LINK_ACTORS[:] = LINK_ACTORS[:]
    OLD_OBJECTS[:] = OBJECTS[:]
    OLD_WATER_BOXES[:] = WATER_BOXES[:]
    
    CURRENTLY_SELECTED = sphere(pos=LINK_ACTORS[0][-1].pos, color = (1,1,1), radius = 15)
    TIME_GOAL = time.clock()+.25

    while 1:
        #Update highlighting sphere
        #print time.clock()
        if CURRENT_ACTOR_SET < 3:
            if CURRENT_ACTOR_SET == 0:
                CURRENTLY_SELECTED.pos = SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos
            elif CURRENT_ACTOR_SET == 1:
                CURRENTLY_SELECTED.pos = LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos
            elif CURRENT_ACTOR_SET == 2:
                CURRENTLY_SELECTED.pos = MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos
            if time.clock() >= TIME_GOAL:
                CURRENTLY_SELECTED.visible = ~CURRENTLY_SELECTED.visible&1
                TIME_GOAL = time.clock()+.25
        elif CURRENT_ACTOR_SET >2:
            CURRENTLY_SELECTED.visible = 0
        #Update title
        if CURRENT_ACTOR_SET == 0:
            NEW_TITLE = "zappy - Scene Act %i: 0x%04X Var: 0x%04X Inc: %i (0x%X)"%(
                CURRENT_SCENE_ACTOR,
                SCENE_ACTORS[CURRENT_SCENE_ACTOR][4],
                SCENE_ACTORS[CURRENT_SCENE_ACTOR][9],
                CURRENT_INCREMENT,
                CURRENT_INCREMENT)
        elif CURRENT_ACTOR_SET == 1:
            NEW_TITLE = "zappy - Link Act %i: 0x%04X Var: 0x%04X Inc: %i (0x%X)"%(
                CURRENT_LINK_ACTOR,
                LINK_ACTORS[CURRENT_LINK_ACTOR][0],
                LINK_ACTORS[CURRENT_LINK_ACTOR][7],
                CURRENT_INCREMENT,
                CURRENT_INCREMENT)
        elif CURRENT_ACTOR_SET == 2:
            NEW_TITLE = "zappy - Map Act %i: 0x%04X Var: 0x%04X Inc: %i (0x%X)"%(
                CURRENT_MAP_ACTOR,
                MAP_ACTORS[CURRENT_MAP_ACTOR][0],
                MAP_ACTORS[CURRENT_MAP_ACTOR][7],
                CURRENT_INCREMENT,
                CURRENT_INCREMENT)
        elif CURRENT_ACTOR_SET == 3:
            NEW_TITLE = "zappy - Object %i: 0x%04X Inc: %i (0x%X)"%(
                CURRENT_OBJECT,
                OBJECTS[CURRENT_OBJECT],
                CURRENT_INCREMENT,
                CURRENT_INCREMENT)
        elif CURRENT_ACTOR_SET == 4:
            NEW_TITLE = "zappy - Water Box %i Inc: %i (0x%X)"%(
                CURRENT_WATER_BOX,
                CURRENT_INCREMENT,
                CURRENT_INCREMENT)        
        if scene.title != NEW_TITLE:
            scene.visible = 0
            scene.title = NEW_TITLE
            scene.visible = 1
        #Get keypresses
        if scene.kb.keys:
            s = scene.kb.getkey()
            #Change incrementation amount
            if s == '\t':
                if CURRENT_INCREMENT == 1:
                    CURRENT_INCREMENT = 16
                elif CURRENT_INCREMENT == 16:
                    CURRENT_INCREMENT = 64
                elif CURRENT_INCREMENT == 64:
                    CURRENT_INCREMENT = 256
                elif CURRENT_INCREMENT == 256:
                    CURRENT_INCREMENT = 0x1000
                elif CURRENT_INCREMENT == 0x1000:
                    CURRENT_INCREMENT = 1

            elif s == 'f1' and len(SCENE_ACTORS)>0:
                CURRENT_ACTOR_SET = 0   #Doors
                if len(WATER_BOXES): WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
            elif s == 'f2':
                CURRENT_ACTOR_SET = 1   #Link positions
                if len(WATER_BOXES): WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
            elif s == 'f3' and len(MAP_ACTORS)>0:
                CURRENT_ACTOR_SET = 2   #Map actors
                if len(WATER_BOXES): WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
            elif s == 'f4' and len(OBJECTS)>0:
                CURRENT_ACTOR_SET = 3   #Objects
                if len(WATER_BOXES): WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
            elif s == 'f5' and len(WATER_BOXES)>0:
                CURRENT_ACTOR_SET = 4   #Waterboxes
                WATER_BOXES[CURRENT_WATER_BOX][-1].color = (1,0,0)
            
            #Increment/Decrement current actor (object) numbers
            elif s == ']':
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][4] += CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][0] += CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][0] += CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 3:
                    OBJECTS[CURRENT_OBJECT] += CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][3] += CURRENT_INCREMENT
            elif s == '[':
                if CURRENT_ACTOR_SET == 0 and SCENE_ACTORS[CURRENT_SCENE_ACTOR][4]-CURRENT_INCREMENT >= 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][4]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 1 and LINK_ACTORS[CURRENT_LINK_ACTOR][0]-CURRENT_INCREMENT >= 0:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][0]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 2 and MAP_ACTORS[CURRENT_MAP_ACTOR][0]-CURRENT_INCREMENT >= 0:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][0]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 3 and OBJECTS[CURRENT_OBJECT]-CURRENT_INCREMENT >= 0:
                    OBJECTS[CURRENT_OBJECT]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 4 and (WATER_BOXES[CURRENT_WATER_BOX][3] - CURRENT_INCREMENT) > 0:
                    WATER_BOXES[CURRENT_WATER_BOX][3] -= CURRENT_INCREMENT
            #Increment/Decrement current actor varible
            elif s == '.' or s == '>':
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][9]+=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][7]+=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][7]+=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][4] += CURRENT_INCREMENT
            elif s == ',' or s == '<':
                if CURRENT_ACTOR_SET == 0 and SCENE_ACTORS[CURRENT_SCENE_ACTOR][9]-CURRENT_INCREMENT >= 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][9]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 1 and LINK_ACTORS[CURRENT_LINK_ACTOR][7]-CURRENT_INCREMENT >= 0:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][7]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 2 and MAP_ACTORS[CURRENT_MAP_ACTOR][7]-CURRENT_INCREMENT >= 0:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][7]-=CURRENT_INCREMENT
                elif CURRENT_ACTOR_SET == 4 and (WATER_BOXES[CURRENT_WATER_BOX][4] - CURRENT_INCREMENT) > 0:
                    WATER_BOXES[CURRENT_WATER_BOX][4] -= CURRENT_INCREMENT
                    
            #Increment/Decrement current actor (object)
            elif s == '-':
                if CURRENT_ACTOR_SET == 0 and CURRENT_SCENE_ACTOR > 0:
                    CURRENT_SCENE_ACTOR -= 1
                elif CURRENT_ACTOR_SET == 1 and CURRENT_LINK_ACTOR > 0:
                    CURRENT_LINK_ACTOR -= 1
                elif CURRENT_ACTOR_SET == 2 and CURRENT_MAP_ACTOR > 0:
                    CURRENT_MAP_ACTOR -= 1
                elif CURRENT_ACTOR_SET == 3 and CURRENT_OBJECT > 0:
                    CURRENT_OBJECT-=1
                elif CURRENT_ACTOR_SET == 4 and CURRENT_WATER_BOX > 0:
                    WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
                    CURRENT_WATER_BOX-=1
                    WATER_BOXES[CURRENT_WATER_BOX][-1].color = (1,0,0)
            elif s == '=' or s == '+':
                if CURRENT_ACTOR_SET == 0 and len(SCENE_ACTORS)-1 > CURRENT_SCENE_ACTOR:
                    CURRENT_SCENE_ACTOR += 1
                elif CURRENT_ACTOR_SET == 1 and len(LINK_ACTORS)-1 > CURRENT_LINK_ACTOR:
                    CURRENT_LINK_ACTOR += 1
                elif CURRENT_ACTOR_SET == 2 and len(MAP_ACTORS)-1 > CURRENT_MAP_ACTOR:
                    CURRENT_MAP_ACTOR += 1
                elif CURRENT_ACTOR_SET == 3 and len(OBJECTS)-1 > CURRENT_OBJECT:
                    CURRENT_OBJECT+=1
                elif CURRENT_ACTOR_SET == 4 and len(WATER_BOXES)-1 > CURRENT_WATER_BOX:
                    WATER_BOXES[CURRENT_WATER_BOX][-1].color = (0,0,1)
                    CURRENT_WATER_BOX+=1
                    WATER_BOXES[CURRENT_WATER_BOX][-1].color = (1,0,0)

            #Rotate current actor
            elif s == 't': #Y Plus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] += CURRENT_INCREMENT
                    if SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] > 0x7FFF:
                        SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] -= 0x10000
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(0,1,0))
                elif CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][5] += CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][5] > 0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][5] -= 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(0,1,0))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][5] += CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][5] > 0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][5] -= 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(0,1,0))
            elif s == 'e': #Y Minus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] -= CURRENT_INCREMENT
                    if SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] < -0x7FFF:
                        SCENE_ACTORS[CURRENT_SCENE_ACTOR][8] += 0x10000
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(0,1,0))
                elif CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][5] -= CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][5] < -0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][5] += 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(0,1,0))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][5] -= CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][5] < -0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][5] += 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(0,1,0))
            elif s == 'r': #X Plus
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][4] += CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][4] > 0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][4] -= 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(1,0,0))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][4] += CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][4] > 0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][4] -= 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(1,0,0))
            elif s == 'f': #X Minus
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][4] -= CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][4] < -0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][4] += 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(1,0,0))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][4] -= CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][4] < -0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][4] += 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(1,0,0))
            elif s == 'g': #Z Plus
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][6] += CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][6] > 0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][6] -= 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(0,0,1))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][6] += CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][6] > 0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][6] -= 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=CURRENT_INCREMENT/ROT_DIVISOR,axis=(0,0,1))
            elif s == 'd': #Z Minus
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][6] -= CURRENT_INCREMENT
                    if LINK_ACTORS[CURRENT_LINK_ACTOR][6] < -0x7FFF:
                        LINK_ACTORS[CURRENT_LINK_ACTOR][6] += 0x10000
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(0,0,1))
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][6] -= CURRENT_INCREMENT
                    if MAP_ACTORS[CURRENT_MAP_ACTOR][6] < -0x7FFF:
                        MAP_ACTORS[CURRENT_MAP_ACTOR][6] += 0x10000
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].rotate(angle=-(CURRENT_INCREMENT/ROT_DIVISOR),axis=(0,0,1))
            #Move current actor
            elif s == 'i': #X plus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[0] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[0] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[0] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][0] += CURRENT_INCREMENT
            elif s == 'k': #X minus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[0] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[0] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[0] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][0] -= CURRENT_INCREMENT
            elif s == 'l': #Z plus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[2] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[2] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[2] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][2] += CURRENT_INCREMENT
            elif s == 'j': #Z minus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[2] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[2] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[2] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][2] -= CURRENT_INCREMENT
            elif s == 'y': #Y plus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[1] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[1] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[1] += CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][1] += CURRENT_INCREMENT
            elif s == 'h': #Y minus
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos[1] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos[1] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos[1] -= CURRENT_INCREMENT
                if CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][1] -= CURRENT_INCREMENT
            #Reset Positions
            elif s == 'a':
                if CURRENT_ACTOR_SET == 0:
                    SCENE_ACTORS[CURRENT_SCENE_ACTOR][-1].pos = OLD_SCENE_ACTORS[CURRENT_SCENE_ACTOR][5:8]
                elif CURRENT_ACTOR_SET == 1:
                    LINK_ACTORS[CURRENT_LINK_ACTOR][-1].pos = OLD_LINK_ACTORS[CURRENT_LINK_ACTOR][1:4]
                elif CURRENT_ACTOR_SET == 2:
                    MAP_ACTORS[CURRENT_MAP_ACTOR][-1].pos = OLD_MAP_ACTORS[CURRENT_MAP_ACTOR][1:4]
                elif CURRENT_ACTOR_SET == 4:
                    WATER_BOXES[CURRENT_WATER_BOX][-1].pos = (OLD_WATER_BOXES[CURRENT_WATER_BOX][0]+(OLD_WATER_BOXES[CURRENT_WATER_BOX][3]/2.),
                                                              OLD_WATER_BOXES[CURRENT_WATER_BOX][1]-16384,
                                                              OLD_WATER_BOXES[CURRENT_WATER_BOX][2]+(OLD_WATER_BOXES[CURRENT_WATER_BOX][4]/2.))

            #Move camera
            elif s == 'w':
                scene.center[0] += scene.forward[0]*CAMSPEED
                scene.center[1] += scene.forward[1]*CAMSPEED
                scene.center[2] += scene.forward[2]*CAMSPEED
                scene.range[0] -= scene.forward[0]*CAMSPEED
                scene.range[1] -= scene.forward[1]*CAMSPEED
                scene.range[2] -= scene.forward[2]*CAMSPEED
            elif s == 's':
                scene.center[0] -= scene.forward[0]*CAMSPEED
                scene.center[1] -= scene.forward[1]*CAMSPEED
                scene.center[2] -= scene.forward[2]*CAMSPEED
                scene.range[0] += scene.forward[0]*CAMSPEED
                scene.range[1] += scene.forward[1]*CAMSPEED
                scene.range[2] += scene.forward[2]*CAMSPEED
            #Save
            elif s == 'ctrl+s':
                try:
                    ROM.seek(SCENE_ACTOR_OFF)
                    for SCENE_ACTOR in SCENE_ACTORS:
                        ROM.write(pack(">BBBBHhhhhH",SCENE_ACTOR[0],SCENE_ACTOR[1],SCENE_ACTOR[2],SCENE_ACTOR[3],
                                       SCENE_ACTOR[4],SCENE_ACTOR[-1].pos[0],SCENE_ACTOR[-1].pos[1],SCENE_ACTOR[-1].pos[2],SCENE_ACTOR[8],SCENE_ACTOR[9]))
                except NameError:
                    pass
                if LINK_ACTOR_OFF:
                    ROM.seek(LINK_ACTOR_OFF)
                    for LINK_ACTOR in LINK_ACTORS:
                        ROM.write(pack(">HhhhhhhH",LINK_ACTOR[0],LINK_ACTOR[-1].pos[0],LINK_ACTOR[-1].pos[1],LINK_ACTOR[-1].pos[2],
                                       LINK_ACTOR[4],LINK_ACTOR[5],LINK_ACTOR[6],LINK_ACTOR[7]))
                try:
                    ROM.seek(MAP_ACTOR_OFF)
                    for MAP_ACTOR in MAP_ACTORS:
                        ROM.write(pack(">HhhhhhhH",MAP_ACTOR[0],MAP_ACTOR[-1].pos[0],MAP_ACTOR[-1].pos[1],MAP_ACTOR[-1].pos[2],
                                       MAP_ACTOR[4],MAP_ACTOR[5],MAP_ACTOR[6],MAP_ACTOR[7]))
                except NameError:
                    pass
                try:
                    ROM.seek(OBJECT_OFF)
                    for OBJECT in OBJECTS:
                        ROM.write(pack(">H",OBJECT))
                except NameError:
                    pass
                try:
                    ROM.seek(WATER_BOX_OFF)
                    for WATER_BOX in WATER_BOXES:
                        ROM.write(pack(">hhhHHHL",WATER_BOX[0],WATER_BOX[1],WATER_BOX[2],WATER_BOX[3],WATER_BOX[4],WATER_BOX[5],WATER_BOX[6]))
                except NameError:
                    pass

        if INITIAL_CAM_POS_SET:
            try:
                scene.center = OLD_CAM_VALS[0]
                scene.forward = OLD_CAM_VALS[1]
                scene.range = OLD_CAM_VALS[2]
            except:
                pass
            INITIAL_CAM_POS_SET = 0
        if CURRENT_ACTOR_SET == 4:
            WATER_BOXES[CURRENT_WATER_BOX][-1].pos = (WATER_BOXES[CURRENT_WATER_BOX][0]+(WATER_BOXES[CURRENT_WATER_BOX][3]/2.),WATER_BOXES[CURRENT_WATER_BOX][1]-16384,WATER_BOXES[CURRENT_WATER_BOX][2]+(WATER_BOXES[CURRENT_WATER_BOX][4]/2.))
            WATER_BOXES[CURRENT_WATER_BOX][-1].length=WATER_BOXES[CURRENT_WATER_BOX][3]
            WATER_BOXES[CURRENT_WATER_BOX][-1].height=32768
            WATER_BOXES[CURRENT_WATER_BOX][-1].width=WATER_BOXES[CURRENT_WATER_BOX][4]


    ROM.close()

if __name__ == '__main__':
    if (len(argv) >= 4):
        while 1:
            try:
                open(argv[1],"r").close()
            except:
                print "Invalid ROM (Cannot open \"%s\")"%(argv[1])
                break
            try:
                if (int(argv[2])>109 or int(argv[2])<0):
                    print "Too high of scene number (%i, must be 0-109)"%int(argv[2])
                    break
            except:
                print "Invalid scene number (invalid integer\"%s\")"%(argv[2])
                break
            try:
                int(argv[3])
            except:
                print "Invalid map number (invalid integer \"%s\")"%(argv[3])
                break
            if (len(argv) >= 5):
                try:
                    int(argv[4])
                except:
                    print "Invalid header number (invalid integer \"%s\")"%(argv[4])
            main()
            break

    else:
        print """Invalid arguments supplied (Not enough arguments to load ROM, scene and map)
Usage:
 python zappy.py <ROM> <SCENE NUBMER> <MAP NUMBER> <OBJECT SET (optional)>
Note: All numbers provided are decimal
Example:
 python zappy.py /home/spinout/ROMS/ZELOOTMA.Z64 82 0 0"""
        raw_input('Press any key to continue')

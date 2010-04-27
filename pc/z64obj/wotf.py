#!/usr/bin/python


##Wavefront Obj To F3DEX2/F3DZEX converter 2.0.1 (wotf)
##
##FIXME
## holy crap this works as a horrible library - but it does work, assuming you know it's backbone
## variables (fdir, slash, verts, normals, texcoords, curr_mtl, mtls, mtlns, uvscale, vtxscale, offset, bank)
##
##Change log
## 2.0.0 First released re-write
## 
## 2.0.1 Changed command line interface, added scaling of axis and texture coordinites
## 
## 2.0.2 Implemented reading of 'g' commands in parsing stage, so that collision conversion
##       can reflect group changes. Got the combiner right, texture stuff is pretty much complete.
##       also made it so faces can be reversed
##
##Author
## spinout ( http://spinout182.com spinout_182@yahoo.com )
##
##Creds
## Thanks to zeldaSymphony for giving me the algorithm to
## draw polygons of more than 3 points :)
## 
## Thanks to cooliscool for helping me sort combiner stuff out.
##
##Licence
## Redistribute, modify, refrence, rape, etc at will. I doubt you can make a profit, and if you did I could care less.
##

import sys
argv=sys.argv
from os import path, getcwd
import platform
from struct import pack,unpack
from gbi import *
try:
    import png
    png_imported = True
except ImportError:
    print "WARNING: pypng module not found; cannot convert PNG images. Get pypng at:\nhttp://code.google.com/p/pypng/downloads/list"
    png_imported = False

verts = []
normals = []
texcoords = []
curr_mtl = 0
mtls = [[(.2,.2,.2), (.8,.8,.8), (1,1,1), 1, 0, None, None, [], True, False]]
mtlns = []
uv_scale = 1024
vtx_scale = (16.0, 16.0, 16.0)
offset = 0
bank = 6
reverse_faces=False


if not sys.platform in ["win32", "win64", "cygwin"]:
    slash = "/"
else:
    slash = "\\"

#Directory that should contain MTL file, including trailing slash.
try:
    fdir = path.split(argv[-1])[0]
except:
    fdir = ""
if not(fdir):
    fdir = getcwd()
fdir+=slash

filename = "%sout.bin" % fdir

def resetvars():
    global verts, normals, texcoords, curr_mtl, mtls, mtlns, uvscale, vtxscale, offset, bank
    verts = []
    normals = []
    texcoords = []
    curr_mtl = 0
    mtls = [[(.2,.2,.2), (.8,.8,.8), (1,1,1), 1, 0, None, None, [], True, False]]
    mtlns = []
    uv_scale = 1024
    vtx_scale = (16.0, 16.0, 16.0)
    offset = 0
    bank = 6
    reverse_faces=False

#MTL commands
def mtl_ka(line): #ambient color
    ka_vals = line.split(" ")[1:]
    ka_vals = (float(ka_vals[0]),float(ka_vals[1]),float(ka_vals[2]))
    mtls[-1][0] = ka_vals

def mtl_kd(line): #diffuse color
    kd_vals = line.split(" ")[1:]
    kd_vals = (float(kd_vals[0]),float(kd_vals[1]),float(kd_vals[2]))
    mtls[-1][1] = kd_vals

def mtl_ks(line): #specular color
    ks_vals = line.split(" ")[1:]
    ks_vals = (float(ks_vals[0]),float(ks_vals[1]),float(ks_vals[2]))
    mtls[-1][2] = ks_vals

def mtl_tr(line): #transparency
    mtls[-1][3] = float(line.split(' ')[1])

def mtl_ns(line): #shinyness
    mtls[-1][4] = float(line.split(' ')[1])

def mtl_illum(line): #illumination
    mtls[-1][5]

def mtl_tex(line): #texture (map_kd/map_ks style)
    global fdir
    mtls[-1][6] = "%s%s"%(fdir, line[6:])

def mtl_tex_(line): #texture (mapkd/mapks style)
    global fdir
    mtls[-1][6] = "%s%s"%(fdir, line[7:])

def mtl_ignore(line): #Usually a comment
    return

def mtl_cull(line):
    mtls[-1][8] = False

def mtl_enableVtxColors(line):
    mtls[-1][9] = True

mtl_switch = {
    "ka"	: mtl_ka,
    "kd"	: mtl_kd,
    "ks"	: mtl_ks,
    "d"	: mtl_tr,
    "tr"	: mtl_tr,
    "ns"	: mtl_ns,
    "illum"	: mtl_illum,
    "map_ka"	: mtl_tex_,
    "map_kd"	: mtl_tex_,
    "mapka"	: mtl_tex,
    "mapkd"	: mtl_tex,
    "ni"	: mtl_ignore,	#Optical intensity, unsupported by F3DEX2
    "tf"	: mtl_ignore,	#transmission filter, unsupported by F3D((Z)EX(2))
    "ke"	: mtl_ignore,	#FIXME: could not find documentation on Ke on the web.
    #Begin custom MTL commands
    "nocull"	: mtl_cull,
    "vtxcols"	: mtl_enableVtxColors
}

#Object commands
def obj_ignore(line): #Comments, or group naming commands.
    return

def obj_faces(line): #Faces
    global curr_mtl, reverse_faces
    abc = line.split(" ")[1:]
    face_xyz = []	#XYZ
    face_tex_coords = []#Tex coords
    face_norms = []	#Normals
    for entry in abc:
        parts = entry.split("/")
        face_xyz.append( int(parts[0])-1 )	#XYZ pos
        if len(parts) > 1:	#more than just verts?
            if parts[1]:	#Sometimes texture coords are left blank, ie v//vn
                face_tex_coords.append( int(parts[1])-1 )
            else:
                face_tex_coords.append( 0 )
            if len(parts) == 3:	#Vertex normals
                face_norms.append( int(parts[2])-1 )
            else:
                face_norms.append( 0)
        else:
            face_tex_coords.append(0)
            face_norms.append(0)
    PointLen = len(abc)-2
    if not reverse_faces:
        for i in range(0,PointLen,1):
            mtls[curr_mtl][7].append(tuple([
        	 tuple([face_xyz[0], face_xyz[i+1], face_xyz[i+2]]),
        	 tuple([face_tex_coords[0], face_tex_coords[i+1], face_tex_coords[i+2]]),
        	 tuple([face_norms[0], face_norms[i+1], face_norms[i+2]]) ]))
    else:
        for i in range(0,PointLen,1):
            mtls[curr_mtl][7].append(tuple([
        	 tuple([face_xyz[0], face_xyz[i+2], face_xyz[i+1]]),
        	 tuple([face_tex_coords[0], face_tex_coords[i+2], face_tex_coords[i+1]]),
        	 tuple([face_norms[0], face_norms[i+2], face_norms[i+1]]) ]))

def obj_vertex(line): #verts
    xyz = line.split(" ")[1:]
    xyz = (float(xyz[0]), float(xyz[1]), float(xyz[2]))
    verts.append(xyz)

def obj_normals(line): #normals
    xyzn = line.split(" ")[1:]
    xyzn = (float(xyzn[0]),float(xyzn[1]),float(xyzn[2]))
    normals.append(xyzn)
    
def obj_texcoords(line): #Texutre U/V coordinates
    xyt = line.split(" ")[1:]
    xyt = (float(xyt[0]),float(xyt[1]))
    texcoords.append(xyt)

def obj_usemtl(line): #set curr_mtl
    global curr_mtl
    mtl = line[7:]
    try: #Try and find the correct MTL...
        curr_mtl = mtlns.index(line[7:])
    except ValueError:
        curr_mtl = 0

def obj_mtllib(line): #Read a MTL file
    global fdir
    global mtls
    try:
        mtlfn = line[7:]
        if mtlfn[:2] == './':
            mtlfn = mtlfn[2:]
        mtlfn = "%s%s"%(fdir, mtlfn)
        #open, read, close
        mtllibf = open(mtlfn,"r")
        mtllibd = mtllibf.read()
        mtllibf.close()
        mtllibd = fix_obj(mtllibd).split('\n') #'fixed' using same function obj files are 'fixed' with.
        mtls = []
        i = 0
        no_lines = len(mtllibd)
        while i < no_lines:
            if mtllibd[i][:6] == "newmtl":
                mtls.append([	#	Create new material with default values, in the case not all are specified.
                	(.2,.2,.2),	#Ka	ambient color
                	(.8,.8,.8),	#Kd	diffuse color
                	(1,1,1),	#Ks	specular color
                	1,	#Tr, d		transparency/alpha
                	0,	#Ns		shinyness
                	None,	#illum		illum option (1 = specular highlights disabled, 2 = enabled)
                	None,	#map_Kd, map_Ks	texture file
                	[],	#list that will contain tuples of triangle points/verts/uvs
                	True,	#Backface culling
                	False	#Disable normals
                    	])
                mtlns.append(mtllibd[i][7:])
            elif mtllibd[i]:
                command = mtllibd[i].split(" ")[0]
                if command[0] != "#":
                    try:
                        mtl_switch[command.lower()](mtllibd[i])
                    except KeyError:
                        print "Unimplimented mtl command on line %i: %s" % (i+1, command )
            i+=1
        del mtllibd
        mtl_used = True
    except IOError: #MTL not found
        print "Could not open MTL file %s" % ( mtlfn )

def obj_group(line):
    global curr_mtl
    mtls.append(mtls[curr_mtl][:])
    mtls[-1][7]=[]
    curr_mtl=len(mtls)-1

obj_switch = {
    "f"	: obj_faces,
    "v"	: obj_vertex,
    "vt"	: obj_texcoords,
    "vn"	: obj_normals,
    "#"	: obj_ignore,
    "g"	: obj_ignore,#group,
    "s"	: obj_ignore,
    "usemtl"	: obj_usemtl,
    "mtllib"	: obj_mtllib
}

def parse_obj(obj_data):
    #split obj data into a list, each entry being a line
    obj_lines = obj_data.split("\n")
    i = 0
    last=""
    for line in obj_lines:
        if line:
            command = line.split(" ")[0]
            if command[0] != '#':
                try:
                    obj_switch[command](line)
                except KeyError:
                    print "Unimplimented obj command on line %i: %s" %(i+1, command)
            #HACK: support grouping (For collision type separation)
            if last == "g":
                obj_group("")
            last=command
        i+=1
    del obj_lines

def fix_obj(string_obj):
    ret = '' #return value
    lc = '' #last char
    for c in string_obj:
    	#Change tab to space...
    	if (c == '/t'):
    	    c = ' '
        #Remove un-needed whitespace, or \r
        if not ((c == ' ' and lc == ' ') or (c == ' ' and lc == '/n') or (c == '\r') ):
            ret+=c
        lc = c
    del string_obj
    retlines = ret.split("\n")
    ret =""
    for line in retlines:
       while line and ( (line[0] == ' ') or (line[0] == '\t') ):
           line = line[1:]
       while line and ( (line[-1] == ' ') or (line[-1] == '\t') ):
           line = line[:-1]
       if line:
           ret += "%s\n" % (line)
    del retlines
    return ret

def align(data, bytes = 4):
    return "%s%s"%(data, "\x00" * (bytes - (len(data)&(bytes-1))) ) if len(data)&(bytes-1) else data

def mk_vtx(x,y,z,u,v,r,g,b,a):
    global uv_scale
    global vtx_scale
    return pack(">hhhxxhhBBBB",int(x * vtx_scale[0]), int(y * vtx_scale[1]), int(z * vtx_scale[2]),
                int(u * uv_scale), int(v * uv_scale),
                int(r * 255) & 255, int(g * 255) & 255, int(b * 255) & 255, int(a * 255) & 255 )

rgba8888_to_rgba5551 = lambda r,g,b,a: struct.pack(">H", ((r&0xF8)<<8)|((g&0xF8)<<3)|((b&0xF8)>>2)|(a>>7))

siz_conv = {
 4  : 0,
 8  : 1,
 16 : 2,
 32 : 4
}

''' |     |    siz
    |     |04|08|16|32
    |rgba |  |  | x| x
fmt |yuv  |  |  | x|  
    |ci   | x| x|  |  
    |ia   | x| x| x|  
    |i    | x| x|  |  
G_IM_FMT_RGBA
G_IM_FMT_YUV
G_IM_FMT_CI
G_IM_FMT_IA
G_IM_FMT_I
G_IM_SIZ_4b
G_IM_SIZ_8b
G_IM_SIZ_16b
G_IM_SIZ_32b'''
class ColorIndex:
    def __init__(self):
        self.colors=[]
    def append(self,r,g,b,a):
        self.colors.append(rgba(r,g,b,a))
    def index(self,r,g,b,a):
        i=0
        for color in self.colors:
            if color.r == r and color.g == g and color.b == b and color.a == a:
                return i
            i+=1
        raise ValueError("ColorIndex.index(x): x not in list")
    def toData(self,siz_bytes):
        ret = ""
        for color in self.colors:
            ret += color.toData(siz_bytes)
        return ret

class rgba:
    def __init__(self, r=0,g=0,b=0,a=255):
        self.r=r
        self.g=g
        self.b=b
        self.a=a
    def toData(self, siz_bytes):
        if siz_bytes == 2:
            return rgba8888_to_rgba5551(self.r,self.g,self.b,self.a)
        elif siz_bytes == 4:
            return struct.pack(">BBBB",self.r,self.g,self.b,self.a)
        else:
            raise ValueError("siz_bytes is not a valid value")
    def fromData(self, src):
        if len(src) == 2:
            src = struct.unpack(">H",src)
            self.r = (src >> 8)&0xF8
            self.g = (src >> 3)&0xF8
            self.b = (src << 2)&0xF8
            self.a = 255 * src&1
        elif len(src) == 4:
            src = struct.unpack(">L",src)
            self.r = (src >> 24)
            self.g = (src >> 16)&0xFF
            self.b = (src >> 8)&0xFF
            self.a = src&0xFF
        else:
            raise ValueError("src pixel is unknown size")
        return None

def getNearest(r,g,b,ColorIndex):
    closest = 0
    diffs = [255,255,255]
    for i in range(255):
        rdif = abs(ColorIndex.colors[i].r - r)
        gdif = abs(ColorIndex.colors[i].g - g)
        bdif = abs(ColorIndex.colors[i].b - b)
        if rdif < diffs[0]:
            if gdif < diffs[1]:
                if bdif < diffs[2]:
                    diffs = [rdif, gdif, bdif]
                    closest = i
        if gdif < diffs[1]:
            if bdif < diffs[2]:
                if rdif < diffs[0]:
                    diffs = [rdif, gdif, bdif]
                    closest = i
        if bdif < diffs[2]:
            if rdif < diffs[0]:
                if gdif < diffs[1]:
                    diffs = [rdif, gdif, bdif]
                    closest = i
    return closest
        
def png_to_tex(fname, Textures):
    pngi = png.Reader(fname)
    png_data = pngi.read()
    pngBPC = png_data[3]['bitdepth']
    pngAlpha = png_data[3]['alpha']
    pngGreyScale = png_data[3]['greyscale']
    TexX = png_data[0]
    TexY = png_data[1]
    if TexX not in [1,2,4,8,16,32,64,128,256]:
        raise ValueError("%s: Non base-2 width texture",fname)
    if TexY not in [1,2,4,8,16,32,64,128,256]:
        raise ValueError("%s: Non base-2 height texture",fname)
    TexBPP = pngBPC #This will change if RGBA, else it will remain the same.
    TexOff = len(Textures)
    PalOff = None
    png_lines = list(png_data[2])[::-1]
    
    CI = ColorIndex()
    CIT = []
    CIBPP = None
    
    if (pngGreyScale and pngAlpha):
        TexFormat = G_IM_FMT_IA
        TexBPP = TexBPP + pngBPC #Alpha channel
        if (pngBPC == 8):
            for line in png_lines:
                for i in range(0,len(line),2):
                    Textures += struct.pack(">BB",line[i],line[i+1])
        elif (pngBPC == 4):
            for line in png_lines:
                for i in range(0,len(line),2):
                    Textures += struct.pack(">B",(line[i]<<4)|line[i+1])
        else:
            TypeError("PNG Conversion faliure. Tell spinout this:\n%s"%(png_data))
        
    elif (pngGreyScale and not pngAlpha):
        TexFormat = G_IM_FMT_I
        if (pngBPC == 8):
            for line in png_lines:
                for pixel in line:
                    Textures += struct.pack(">B",pixel)
        elif (pngBPC == 4):
            for line in png_lines:
                for i in range(0,len(line),2):
                    Textures += struct.pack(">B",(line[i]<<4)|line[i+1])
        else:
            TypeError("PNG Conversion faliure. Tell spinout this:\n%s"%(png_data))
         
    elif (not pngGreyScale) and (pngBPC == 8):
        TexFormat = G_IM_FMT_RGBA
        
        TexBPP = 16
        CI_ = []
        
        if (pngAlpha):
            for line in png_lines:
                for i in range(0, len(line), 4):
                    '''
                    try:
                        CIT.append(CI.index(line[i]&0xF8, line[i+1]&0xF8, line[i+2]&0xF8, int(bool(line[i+3])) * 255))
                    except:
                        if len(CI.colors) < 255:
                            CIT.append(len(CI.colors))
                            CI.append(line[i]&0xF8, line[i+1]&0xF8, line[i+2]&0xF8, int(bool(line[i+3])) * 255)
                        else:
                            CIT.append(getNearest(line[i]&0xF8, line[i+1]&0xF8, line[i+2]&0xF8, CI))
            if len(CI.colors) <= 16:
                CIBPP = G_IM_SIZ_4b
                for i in range(0,len(CIT),2):
                    print CIT[i], CIT[i+1]
                    Textures+=struct.pack(">B",(CIT[i]<<4)|CIT[i+1])
            else:
                CIBPP = G_IM_SIZ_8b
                for color in CIT:
                    Textures+=struct.pack(">B", color)
            print len(CI.colors)
            Textures = align(Textures, 4)
            PalOff = len(Textures)
            Textures += CI.toData(2)
                    '''
                    #Textures += rgba8888_to_rgba5551(line[i],line[i+1],line[i+2],line[i+3])
                    color = rgba8888_to_rgba5551(line[i],line[i+1],line[i+2],line[i+3])
                    print line[i],line[i+1],line[i+2],line[i+3], "->" "%04X"%(struct.unpack(">H",color))
                    try:
                        CIT.append(CI_.index(color))
                    except:
                        if len(CI_) < 255:
                            CIT.append(len(CI_))
                            CI_.append(color)
                        else:
                            CIT.append(255)
            for inde in CIT:
                Textures+=struct.pack(">B",inde)
            PalOff = len(Textures)
            for color in CI_:
                Textures+=color
            CIBPP = G_IM_SIZ_8b
        else: #FIXME
            for line in png_lines:
                for i in range(0, len(line), 3):
                    Textures += rgba8888_to_rgba5551(line[i],line[i+1],line[i+2],255)
    else:
        TypeError("PNG Conversion faliure. Tell spinout this:\n%s"%(png_data))
        
    
    return ([TexOff, TexFormat, siz_conv[TexBPP], TexX, TexY, PalOff, CIBPP],Textures)

def bmp_to_tex(fname, Textures):
    bmp_f = open(fname,"rb")
    tex_data = bmp_f.read()
    bmp_f.close()
    #GHETTO WARNING!!1
    bmp_dat_off = unpack("<L",tex_data[0xA:0xE])[0]
    if (unpack("<L",tex_data[0x1E:0x22])[0]):
        raise TypeError("Texture is not BI_RGB format")
    TexX, TexY = unpack("<LL",tex_data[0x12:0x1A])
    if TexX not in [1,2,4,8,16,32,64,128,256]:
        raise ValueError("Non base-2 width texture")
    if TexY not in [1,2,4,8,16,32,64,128,256]:
        raise ValueError("Non base-2 height texture")
    bmpBPP = unpack("<H",tex_data[0x1C:0x1E])[0]
    bmpPAL = unpack("<L",tex_data[0x2E:0x32])[0]
    
    TexOff = len(Textures)
    
    if not bmpPAL:
        if (bmpBPP == 24):
            TexFormat = G_IM_FMT_RGBA
            TexBPP = G_IM_SIZ_16b
            for y in range(TexY):
                line = ""
                for x in range(TexX):
                    line += rgba8888_to_rgba5551(unpack(">B",tex_data[bmp_dat_off+2])[0]/2,unpack(">B",tex_data[bmp_dat_off+1])[0]/2,unpack(">B",tex_data[bmp_dat_off])[0]/2,255)
                    bmp_dat_off+=3
                Textures += line
        elif (bmpBPP == 32):
            TexFormat = G_IM_FMT_RGBA
            TexBPP = G_IM_SIZ_16b
            for y in range(TexY):
                line = ""
                for x in range(TexX):
                    line += rgba8888_to_rgba5551(unpack(">B",tex_data[bmp_dat_off+2])[0], unpack(">B",tex_data[bmp_dat_off+1])[0], unpack(">B",tex_data[bmp_dat_off])[0], unpack(">B",tex_data[bmp_dat_off+3])[0])
                    bmp_dat_off+=3
                Textures += line
        else:
            raise TypeError("TODO: add support for %i bit BMP textures" % bmpBPP)
    else:
        raise TypeError("TODO: add support for paletted BMP textures")
    return ([TexOff, TexFormat, TexBPP, TexX, TexY, None, None],Textures)

conv_tex = {
#These functions MUST: (see functions)
# return ([TexOff, TexFormat, TexBPP, TexX, TexY],Textures)
 "png" : png_to_tex,
 "bmp" : bmp_to_tex
}

mask = { # Basically a logrithm table for all whole values of log2(x)
 1  : 0,
 2  : 1,
 4  : 2,
 8  : 3,
 16 : 4,
 32 : 5,
 64 : 6,
 128: 7,
 256: 8
}


def ParsedMeshToOoT(ParsedMesh, PointList, UVList, NormList, bank=6, BaseOff=0, TexBank=6, TexBaseOff=0, TextureNameToInfo={}):

    
    global png_imported
    if(BaseOff < TexBaseOff) and (TexBank == bank):
        raise ValueError("If TexBaseOff is less than BaseOff, TexBank must be a different value than bank")
    #Parsed mesh is a list (or tuple) with each entry being a list/tuple containing:
    #INDEX	CONTENTS
    #0		ambient color
    #1		diffuse color
    #2		specular color
    #3		transparency/alpha
    #4		shinyness
    #5		illumination option
    #6		texture filename (inc. path)
    #7		lists containing lists of triangle data:
    #		INDEX	CONTENTS
    #		0	[xyz1,xyz2,xyz3] Values are indexes within PointList to get xyz coordinates of triangle
    #		1	[uv,  uv,  uv  ] Values are indexes within UVList to get u/v coordinites of triangles' points
    #		2	[n,   n,   n   ] Values are indexes within NormList to get normals of triangles' points
    #8		cull True/False
    #9		Enable vertex colors if True, Disable normals if False
    pl = PointList
    uv = UVList
    nl = NormList
    if not nl:
        nl = [(0,0,0)]
    if not uv:
        uv = [(0,0)]
    i = 0
    DisplayList = ""
    VertList = ""
    Textures = ""
    
   
    #First pass: Remove any mesh definitions that do not have triangles, convert textures for the remaining mesh definitions
    i = 0
    toPop=[]
    for Entry in ParsedMesh:
        if len(Entry[7]): #There are triangles
            while (Entry[6]): #I use while so I can use break. This should change.
                if Entry[6] in TextureNameToInfo: break #Is the texture already converted?
                try:
                    img = open(Entry[6], "rb")
                except:
                    print "WARNING: Specified texture (%s) for mesh %i not found!" %(Entry[6], i)
                    ParsedMesh[i][6] = None
                    break
                tex_data = img.read(4)
                img.close()
                if (tex_data[1:4] == "PNG"):
                    tex_type = "png"
                elif (tex_data[:2] == "BM"):
                    tex_type = "bmp"
                else:
                    extension = Entry[6][-3:].lower()
                    if (extension == "png" or extension == "bmp"):
                        tex_type = extension
                    else:
                        print "WARNING: Specified texture for mesh %i is unknown format (%s)!" % (i, extension)
                        ParsedMesh[i][6] = None
                        break;
                print "Converting texture %i: %s"%(i,Entry[6])
               # try:
                TextureNameToInfo[Entry[6]], Textures = conv_tex[tex_type](Entry[6],Textures)
                TextureNameToInfo[Entry[6]][0] += TexBaseOff | (TexBank<<24)
                #except:
                 #   print "Error converting texture"
                 #   ParsedMesh[i][6] = None
                break;
        else:	#No triangles, we might as well not bother doing anything else, so remove the entry.
            toPop.append(i)
        i+=1
    toPop.reverse()
    for index in toPop:
        ParsedMesh.pop(index)
    #INFO: VertListOff is the offset of the vertex list relative to BaseOff.
    
    #Texture bank is the same as vertex+display list bank
    if (TexBank == bank):
        VertListOff = len(Textures)
    
    #Texture bank is a alternative bank
    else:
        VertListOff = 0
    
    i = 0
    #Second pass: Convert triangles
    for Entry in ParsedMesh:
        ##DESCRIBE GEOMETRY##
        DisplayList += pack(">Q",PipeSync)
        if Entry[6]:
            TexOff, TexFormat, TexBPP, TexX, TexY, TexPal, CIBPP = TextureNameToInfo[Entry[6]]
            TexXR, TexYR = 1.0, 1.0
            #TexYR = float(TexX)/32 	#Used for texture coord x scaling
            #TexXR = float(TexY)/32	#Used for texture coord y scaling
            DisplayList += pack(">Q", SetTextureLOD(G_TL_TILE))
            DisplayList += pack(">Q", Texture(-1, -1, 0, 0, G_ON))# s = -1(0xFFFF); t = -1(0xFFFF); level=0, tile=0, on=1
            if TexPal == None:
                DisplayList += LoadTextureBlock(TexOff, TexFormat, TexBPP, TexX, TexY, 0, G_TX_NOMIRROR, G_TX_NOMIRROR, mask[TexX], mask[TexY], G_TX_NOLOD, G_TX_NOLOD)
#                DisplayList += LoadTextureBlock(TexOff, TexFormat, TexBPP, TexX, TexY, 0, G_TX_NOMIRROR|G_TX_CLAMP, G_TX_NOMIRROR|G_TX_CLAMP, mask[TexX], mask[TexY], G_TX_NOLOD, G_TX_NOLOD)
            else:
                DisplayList += pack(">Q", SetTextureLUT(G_TT_RGBA16))
                if CIBPP == G_IM_SIZ_4b:
                    DisplayList += LoadTLUT_pal16(0,TexPal)
                else:
                    DisplayList += LoadTLUT_pal256(TexPal)
                DisplayList += LoadTextureBlock(TexOff, G_IM_FMT_CI, CIBPP, 
                    TexX,TexY, 0, G_TX_NOMIRROR, G_TX_NOMIRROR,
                    mask[TexX],mask[TexY], G_TX_NOLOD, G_TX_NOLOD)

            
        else:
            TexXR, TexYR = 1.0, 1.0
            DisplayList+= pack(">Q", Texture(-1, -1, 0, 0, G_OFF))
        #omg thanks cooliscool i owe you one
        DisplayList += pack(">Q", SetCombine(0x127E03, 0xFFFCF3F8))#DF8) )
        DisplayList += pack(">Q", SetRenderMode(0x1C, 0xC811307C))#G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_INTER))
        #GeometryMode stuff
        geoModeFlags = G_ZBUFFER|G_FOG|G_LIGHTING
        if Entry[8]: #Enable backface culling if true (Default: True)
            geoModeFlags = geoModeFlags|G_CULL_BACK
        if Entry[9]: #Enable vertex colors if true (Default: False)
            geoModeFlags = geoModeFlags|G_SHADE
        DisplayList += pack(">Q", GeometryMode(G_LIGHTING|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR, 0))
        DisplayList += pack(">Q", GeometryMode(0, geoModeFlags))
        #End GeometryMode
        
        #FIXME: make it work
        #DisplayList+=pack(">Q", SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF))#int(Entry[0][0]*255), int(Entry[0][1]*255), int(Entry[0][2]*255), int(Entry[3]*255)))
        DisplayList+=pack(">Q", SetPrimColor(0,0, int(Entry[1][0]*255)&255, int(Entry[1][1]*255)&255, int(Entry[1][2]*255)&255, int(Entry[3]*255)&255)) # 0xFF, 0xFF, 0xFF, 0xFF))#
        #DisplayList+=pack(">Q", SetBlendColor(int(Entry[1][0]*255), int(Entry[1][1]*255), int(Entry[1][2]*255), int(Entry[3]*255)))
        
        ##ASSEMBLE GEOMETRY##
        #Number of triganles
        NoTris = len(Entry[7])
        
        TmpVertList = []#List containing verts in current vertex buffer, for easy indexing
        asm_tris = ''	#Assembled triangles using verts in TmpVertList
        comm = True	#Toggle for 06/00 commands.
        
        for c in range(0,NoTris,1):
            Tri = Entry[7][c]	#Current triangle
            tri_points = []	#List that will be appended to for adding to asm_tris later
            for j in range(0,3,1):
                vn = Tri[0][j]	#Vertex number
                uvn = Tri[1][j]	#Texuture coords number
                nn = Tri[2][j]	#Normals number
                vtx = mk_vtx(pl[vn][0],pl[vn][1],pl[vn][2],uv[uvn][0]*TexXR,uv[uvn][1]*TexYR,nl[nn][0],nl[nn][1],nl[nn][2],Entry[3])
                try:
                    vtx_no = TmpVertList.index(vtx) #Does this vert already exist?
                except ValueError:	#Nope
                    if len(TmpVertList) <= 29+j:#Is there space for it?
                        TmpVertList.append(vtx)	#Yep
                    else:	#This should never happen.
                        print "Uhoh, I somehow overloaded the vertex buffer. This won't be pretty..."
                    vtx_no = len(TmpVertList)-1 #Vertex number
                tri_points.append(vtx_no<<1) #Append Vertex number to the triangle points list
            
            #Assemble tri_points, and append to assembled triangle list.
            asm_tris += pack(">BBBB", comm*6, tri_points[0], tri_points[1], tri_points[2])
            
            #Toggle comm. It's placement here (before the check to assemble triangles, after assembling triangle) is vital
            comm= not comm
            
            if len(TmpVertList) > 29 or c == NoTris-1: #Vertex buffer full? On last triangle? if so, append mesh and verts to master DL.
                if len(asm_tris)&4: #Do we need to use a 05 for the last triangle instead of a 06?
                    asm_tris = "%s\x05%s\x00\x00\x00\x00"%(asm_tris[:-4],asm_tris[-3:])
                vtx_off = BaseOff + len(VertList) + VertListOff #Calculate vertex offset
                for vert in TmpVertList:
                    VertList+=vert	#Write vertex to master vert string
                #assemble vertex load command, and append to display list
                DisplayList+=pack(">LL",(0x1<<24)|(len(TmpVertList)<<12)|len(TmpVertList*2),(bank<<24)|vtx_off)
                DisplayList+=asm_tris #Append triangles to display list
                #Reset some variables.
                asm_tris = ""
                TmpVertList = []
                comm = True
        i+=1
    DisplayList += pack(">Q", EndDisplayList)
    if (TexBank == bank):
        return (Textures + VertList + DisplayList, BaseOff + len(VertList) + VertListOff, TextureNameToInfo )
    else:
        return (VertList + DisplayList, BaseOff + len(VertList), TextureNameToInfo )






def _int(string_int):
    if (string_int[:2] == '0x') or (string_int[0] == "$"):
        return int(string_int, 16)	#Hexadecimal
    elif (string_int[0] == 'o'):
        return int(string_int, 8)	#Octal
    else:
        return int(string_int, 10)	#Decimal

def help(option_no):
    print """Usage: python %s [options] [object file]
Options:
 -s [float]		: set vertex scale
 -sa [float] [float] [float]	: set x/y/z vertex scale
 -st [float]		: set uv scale
 -b [hex/oct/dec val]	: set bank to use for output file
 -a [hex/oct/dec val]	: set start offset to start writing data to
 -o [string]		: output filename [will be in same dir as obj
			  file if not specified, if that is not specified,
			  program's directory will be used.]
 -h, --help		: This message. All other options are forgotten""" % argv[0]
    return len(argv) # Will break the loop 

def set_vert_scale(option_no):
    global vtx_scale
    option_no += 1
    vtx_scale = float(argv[option_no])
    vtx_scale = tuple([vtx_scale, vtx_scale, vtx_scale])
    return option_no

def set_uv_scale(option_no):
    global uv_scale
    option_no += 1
    uv_scale = float(argv[option_no])
    return option_no

def set_offset(option_no):
    global offset
    option_no += 1
    offset = _int(argv[option_no])
    return option_no

def set_bank(option_no):
    global bank
    option_no += 1
    bank = _int(argv[option_no])
    return option_no

def set_axis_scale(option_no):
    global vtx_scale
    option_no += 3
    vtx_scale = tuple( [float(argv[option_no - 2]), float(argv[option_no - 1]), float(argv[option_no])] )
    print vtx_scale
    return option_no

def set_fn(option_no):
    global filename
    option_no += 1
    filename = argv[option_no]
    try:
        path.split(filename)[0]
    except:
        filename = "%s%s"%(fdir,filename)
    return option_no
    
option_switch = {
    "-s" : set_vert_scale,
    "-sa" : set_axis_scale,
    "-st" : set_uv_scale,
    "-b" : set_bank,
    "-a" : set_offset,
    "-o" : set_fn,
    "-h" : help,
    "--help" : help
}

def main():
    global mtls
    global verts
    global normals
    global texcoords
    global offset
    global bank	#This is going to have to be fixed for scene+map support
    global filename
    
    #Read arguments
    option_no	= 1
    arg_len	= len(argv) - 1
    obj_file	= None
    error_msg	= "Type 'python %s --help' for more information"%(argv[0])
    while option_no <= arg_len:
        option = argv[option_no]
        if (option_no == arg_len) and ((option != "--help") and (option != "-h")): #open file
            try:
                obj_file = open(option, "r")
            except:
                print "File IO Error\n%s" % error_msg
                return -1
        if (obj_file == None):
            try:
                option_no = option_switch[option](option_no)
            except KeyError:
                print "Unknown argument (%s) supplied\n%s" % (option, error_msg)
                return -1
            except IndexError:
                print "Not enough arguments supplied\n%s" % (error_msg)
                return -1
        option_no += 1
    
    if (option_no > arg_len+1):
        return 0
    
    if (arg_len < 1 ) or (obj_file == None):
        print "No input file specified.\n%s" % error_msg
        return -1
    
    #Read & close file
    obj_data = obj_file.read()
    obj_file.close()
    
    #"fix" file...
    obj_data = fix_obj(obj_data)
    
    #read file...
    parse_obj(obj_data)
    
    del obj_data
    
    if not normals:
        normals = [(0,0,0)]
    if not texcoords:
        texcoords = [(0,0)]
    
    #convert parsed data...
    Data = ParsedMeshToOoT(mtls,verts,texcoords,normals, bank, offset, bank, offset)
    f=open(filename, "wb")
    f.write("\x00" * offset)    
    f.write(Data[0])
    f.close()
    raw_input("Display list offset: %08X"%Data[1])
    
if __name__ == "__main__":main()

2010-01-18 01:40:53

okay so you want to put .obj in ocarina of time eh

well i have the thing for you
it's called obj2area.py, and it uses all the other stuff in this folder except this file
and it's pretty easy to use
ex:

  $ python obj2area.py
  Zelda 64 Debug ROM: ../ZELOOTMA.z64
  Wavefront obj file for collision: ../dj/well/well_scene.obj
  Area number (0-109): 109
  Scale: 20
  Music value: 0x4C
  Number of start positions: 0x2
  Number of tansition actors: 1
  Number of water boxes: 2
  Use different obj for mesh, or multiple mesh map?(y/n): y
  Enter nothing for `Obj:` when done
  Map 0
   Obj: ../dj/well/well_room_0.obj
   Scale: 20
   # Actors: 10
   # Objects: 5
   Indoor? (y/n): n
  Map 1
   Obj: ../dj/well/well_room_1.obj
   Scale: 20
   # Actors: 10
   # Objects: 5
   Indoor? (y/n): y
  Map 2
   Obj: 
  Converting texture 0: ../dj/well/grass1.bmp
  Converting texture 1: ../dj/well/bark.bmp
  Converting texture 3: ../dj/well/tile.bmp
  Converting texture 4: ../dj/well/bricks.bmp
  Converting texture 0: ../dj/well/bricks1.bmp
  Converting texture 1: ../dj/well/bricksb.bmp

just give it what it asks for. when it asks for a integer, (ie any number except the scale values, which are assumed to be base-10 floats)
you can specify hex by prefixing with "$" or "0x", octal with "o" (but what idiot uses octal), and otherwise it assumes decimal
i've added some small things since the initial release, primarily the ability to reverse faces

nifty huh

also this is release 0.0.1 so beware

oh one last thing: wotf can be used to build independent display lists:
Usage: python wotf.py [options] [object file]
Options:
 -s [float]			: set vertex scale
 -sa [float] [float] [float]	: set x/y/z vertex scale
 -st [float]			: set uv scale
 -b [hex/oct/dec val]		: set bank to use for output file
 -a [hex/oct/dec val]		: set start offset to start writing data to
 -o [string]			: output filename [will be in same dir as obj
				  file if not specified, if that is not specified,
				  program's directory will be used.]
 -h, --help			: help message


enjoy

spinout_182@yahoo.com

eof

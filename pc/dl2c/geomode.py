
for x in ("""G_ZBUFFER		0x00000001
G_SHADE			0x00000004	/* enable Gouraud interp */
G_TEXTURE_ENABLE	0x00000000	/* Ignored               */
G_SHADING_SMOOTH	0x00200000	/* flat or smooth shaded */
G_CULL_FRONT		0x00000200
G_CULL_BACK		0x00000400
G_CULL_BOTH		0x00000600	/* To make code cleaner */
G_FOG			0x00010000
G_LIGHTING		0x00020000
G_TEXTURE_GEN		0x00040000
G_TEXTURE_GEN_LINEAR	0x00080000
G_LOD			0x00100000	""".split("\n")):
    x = x.split("\t")[0]
    print "				if (w1 & %s) sprintf(flags, \"%%s|%s\", flags);"% (x, x);



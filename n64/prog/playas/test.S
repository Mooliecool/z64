	.file	1 "spawnstuff.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 1
	.text
	.align	2
	.globl	spawnstuff
	.set	nomips16
	.ent	spawnstuff
	.type	spawnstuff, @function
spawnstuff:
	.frame	$sp,72,$31		# vars= 0, regs= 7/0, args= 40, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-72
	lui	$2,%hi(link)
	sw	$16,44($sp)
	lw	$2,%lo(link)($2)
	lui	$16,%hi(actors)
	addiu	$16,$16,%lo(actors)
	lw	$3,308($2)
	sw	$20,60($sp)
	sw	$17,48($sp)
	li	$20,-2145255424			# 0xffffffff80220000
	move	$17,$5
	lhu	$5,68($16)
	sw	$18,52($sp)
	lui	$2,%hi(linkmain)
	move	$18,$4
	ori	$4,$20,0x37c4
	sw	$31,68($sp)
	sw	$3,%lo(linkmain)($2)
	sw	$21,64($sp)
	jal	ObjectSpawn
	sw	$19,56($sp)

	lui	$19,%hi(actors_ram)
	lhu	$6,36($16)
	lhu	$2,70($16)
	move	$7,$0
	lw	$21,%lo(actors_ram)($19)
	move	$4,$18
	move	$5,$17
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$2,36($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lhu	$5,104($16)
	sw	$2,4($21)
	jal	ObjectSpawn
	ori	$4,$20,0x37c4

	lhu	$6,72($16)
	lhu	$2,106($16)
	move	$7,$0
	lw	$21,%lo(actors_ram)($19)
	move	$4,$18
	move	$5,$17
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$2,36($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lhu	$5,140($16)
	sw	$2,8($21)
	jal	ObjectSpawn
	ori	$4,$20,0x37c4

	lhu	$6,108($16)
	lhu	$2,142($16)
	move	$7,$0
	lw	$21,%lo(actors_ram)($19)
	move	$4,$18
	move	$5,$17
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$2,36($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lhu	$5,176($16)
	sw	$2,12($21)
	jal	ObjectSpawn
	ori	$4,$20,0x37c4

	lhu	$6,144($16)
	lhu	$2,178($16)
	move	$7,$0
	lw	$21,%lo(actors_ram)($19)
	move	$4,$18
	move	$5,$17
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$2,36($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lhu	$5,212($16)
	sw	$2,16($21)
	jal	ObjectSpawn
	ori	$4,$20,0x37c4

	lhu	$6,180($16)
	lhu	$2,214($16)
	move	$7,$0
	lw	$21,%lo(actors_ram)($19)
	move	$4,$18
	move	$5,$17
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$2,36($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lhu	$5,248($16)
	sw	$2,20($21)
	jal	ObjectSpawn
	ori	$4,$20,0x37c4

	lhu	$2,250($16)
	lhu	$6,216($16)
	move	$7,$0
	move	$4,$18
	move	$5,$17
	lw	$16,%lo(actors_ram)($19)
	sw	$2,36($sp)
	sw	$7,16($sp)
	sw	$7,20($sp)
	sw	$0,24($sp)
	sw	$0,28($sp)
	jal	ActorSpawn
	sw	$0,32($sp)

	lw	$31,68($sp)
	sw	$2,24($16)
	lw	$21,64($sp)
	lw	$20,60($sp)
	lw	$19,56($sp)
	lw	$18,52($sp)
	lw	$17,48($sp)
	lw	$16,44($sp)
	j	$31
	addiu	$sp,$sp,72

	.set	macro
	.set	reorder
	.end	spawnstuff
	.size	spawnstuff, .-spawnstuff
	.globl	link
	.data
	.align	2
	.type	link, @object
	.size	link, 4
link:
	.word	-2145237584
	.globl	mesh_act
	.section	.bss,"aw",@nobits
	.align	2
	.type	mesh_act, @object
	.size	mesh_act, 4
mesh_act:
	.space	4
	.globl	ZTargetPtr
	.data
	.align	2
	.type	ZTargetPtr, @object
	.size	ZTargetPtr, 4
ZTargetPtr:
	.word	-2145305364
	.globl	input1
	.align	2
	.type	input1, @object
	.size	input1, 4
input1:
	.word	-2146014480
	.globl	delay
	.section	.bss
	.align	2
	.type	delay, @object
	.size	delay, 4
delay:
	.space	4
	.globl	animation_offset
	.align	2
	.type	animation_offset, @object
	.size	animation_offset, 4
animation_offset:
	.space	4
	.globl	actors
	.rdata
	.align	2
	.type	actors, @object
	.size	actors, 252
actors:
	.half	-1
	.byte	-1
	.byte	-1
	.half	-1
	.half	-1
	.half	-1
	.half	-1
	.half	-1
	.space	2
	.word	-1
	.word	-1
	.word	-1
	.word	-1
	.half	-1
	.half	-1
	.half	2
	.byte	14
	.byte	0
	.half	16672
	.half	16640
	.half	16720
	.half	16672
	.half	392
	.space	2
	.word	100670496
	.word	100709604
	.word	100696500
	.word	100721328
	.half	50
	.half	2
	.half	13
	.byte	15
	.byte	0
	.half	16768
	.half	16640
	.half	16768
	.half	16768
	.half	332
	.space	2
	.word	100665952
	.word	100663720
	.word	100665952
	.word	100665952
	.half	9
	.half	0
	.half	27
	.byte	15
	.byte	0
	.half	16688
	.half	16752
	.half	16688
	.half	16800
	.half	332
	.space	2
	.word	100666480
	.word	100664568
	.word	100666480
	.word	100668132
	.half	22
	.half	0
	.half	431
	.byte	15
	.byte	0
	.half	16512
	.half	16880
	.half	16872
	.half	16656
	.half	392
	.space	2
	.word	100682916
	.word	100681272
	.word	100705452
	.word	100685728
	.half	387
	.half	1
	.half	432
	.byte	15
	.byte	0
	.half	16768
	.half	16688
	.half	16768
	.half	16768
	.half	332
	.space	2
	.word	100681696
	.word	100664416
	.word	100681696
	.word	100681696
	.half	388
	.half	0
	.half	40
	.byte	15
	.byte	0
	.half	16768
	.half	16640
	.half	16768
	.half	16672
	.half	332
	.space	2
	.word	100668744
	.word	100664644
	.word	100668744
	.word	100679032
	.half	30
	.half	0
	.globl	mesh_tot
	.align	2
	.type	mesh_tot, @object
	.size	mesh_tot, 4
mesh_tot:
	.word	7
	.globl	mesh_curr
	.section	.bss
	.align	2
	.type	mesh_curr, @object
	.size	mesh_curr, 4
mesh_curr:
	.space	4
	.globl	actors_ram
	.data
	.align	2
	.type	actors_ram, @object
	.size	actors_ram, 4
actors_ram:
	.word	-2140143616

	.comm	attack_action,4,4

	.comm	pres_ani,4,4

	.comm	pres_ani_f,2,2

	.comm	mesh,4,4

	.comm	mesh_a,4,4

	.comm	link_a,4,4

	.comm	last,4,4

	.comm	linkmain,4,4
	.ident	"GCC: (GNU) 4.4.0"

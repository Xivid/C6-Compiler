	push	1
//variable i not defined, saved at fp[0]
L000:
	push	fp[0]
	push	8
	comple
	j0	L001
	push	0
//variable k not defined, saved at fp[1]
	push	1
//variable j not defined, saved at fp[2]
L003:
	push	fp[2]
	push	fp[0]
	comple
	j0	L004
	push	fp[1]
	push	10
	mul
	push	1
	add
	pop	fp[1]
	push	fp[2]
	push	2
	div
	push	2
	mul
	push	fp[2]
	sub
	push	0
	compeq
	j0	L006
	jmp	L005
L006:
	push	fp[1]
	puti
L005:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L003
L004:
L002:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L000
L001:
	push	fp[0]
	push	2
	div
	pop	fp[0]
L007:
	push	88
	puti
	push	fp[0]
	push	1
	sub
	pop	fp[0]
	push	fp[0]
	push	1
	compeq
	j0	L009
	jmp	L008
L009:
	push	1
	j0	L008
	jmp	L007
L008:

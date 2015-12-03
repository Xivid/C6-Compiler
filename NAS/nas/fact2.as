	geti
//variable x from input, saved at fp[0]
	push	fp[0]
	push	0
	compeq
	j0	L000
	push	1
	puti
	jmp	L001
L000:
	push	fp[0]
	push	1
	compge
	push	fp[0]
	push	12
	comple
	and
	j0	L002
	push	1
//variable f not defined, saved at fp[1]
L003:
	push	fp[0]
	push	1
	compgt
	j0	L004
	push	fp[1]
	push	fp[0]
	mul
	pop	fp[1]
	push	fp[0]
	push	1
	sub
	pop	fp[0]
	jmp	L003
L004:
	push	fp[1]
	puti
L002:
L001:

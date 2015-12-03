	push	0
//variable i not defined, saved at fp[0]
L000:
	push	fp[0]
	push	10
	complt
	j0	L001
	push	fp[0]
	push	5
	compeq
	j0	L003
	jmp	L002
L003:
	push	fp[0]
	puti
L002:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L000
L001:

nas: lex.yy.c nas.tab.c
	gcc -o nas lex.yy.c nas.tab.c

lex.yy.c:
	flex nas.l
	
nas.tab.c:
	bison -d nas.y
all: c6c nas

c6c: lex.yy.c y.tab.c c6c.c sym.h sym.c
	gcc -o c6c lex.yy.c y.tab.c c6c.c

lex.yy.c: c6.l
	flex c6.l

y.tab.c: c6.y
	bison -y -d c6.y

clean:
	$(RM) *.yy.c *.tab.c *.tab.h nas.c nas c6c *.as

nas: nas.c nas.tab.c
	gcc -o nas nas.c nas.tab.c

nas.c: nas.l
	flex -o nas.c nas.l

nas.tab.c: nas.y
	bison -d nas.y

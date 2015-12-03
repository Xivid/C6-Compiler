c5i: lex.yy.c y.tab.c c5i.c
	gcc -o c5i lex.yy.c y.tab.c c5i.c

c5c: lex.yy.c y.tab.c c5c.c
	gcc -o c5c lex.yy.c y.tab.c c5c.c

lex.yy.c: c5.l
	flex c5.l

y.tab.c: c5.y
	bison -y -d c5.y

clean:
	$(RM) lex.yy.c y.tab.c y.tab.h c5i c5c


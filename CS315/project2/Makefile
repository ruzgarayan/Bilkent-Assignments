project2: y.tab.c lex.yy.c
	gcc -o project2 y.tab.c
y.tab.c: project2.y lex.yy.c
	yacc project2.y
lex.yy.c: project2.l
	lex project2.l
clean:
	rm -f lex.yy.c project2 y.tab.c
scanner:main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o translate.o symbol_table.o handler.o
	gcc main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o translate.o symbol_table.o handler.o -g -lfl -o scanner
main.o:main.c 
	gcc -c main.c -g -o main.o
syntax_tree.o:syntax_tree.c
	gcc -c syntax_tree.c -g -o syntax_tree.o
syntax.tab.o:syntax.tab.c
	gcc -c syntax.tab.c -g -o syntax.tab.o
lex.yy.o:lex.yy.c syntax.tab.h
	gcc -c lex.yy.c -g -o lex.yy.o
error_handler.o:error_handler.c
	gcc -c error_handler.c -g -o error_handler.o
handler.o:handler.c
	gcc -c handler.c -g -o handler.o
symbol_table.o:symbol_table.c
	gcc -c symbol_table.c -g -o symbol_table.o
translate.o:translate.c
	gcc -c translate.c -g -o translate.o
lex.yy.c:lexical.l syntax.tab.h
	flex lexical.l
syntax.tab.c syntax.tab.h:syntax.y
	bison -d syntax.y
.PHONY:clean
clean:
	rm -rf *.o log.txt lex.yy.c syntax.tab.c syntax.tab.h 
.PHONY:test
test:
	rm -f log.txt
	echo "test1:" >> log.txt
	./scanner ./test/1.c >> log.txt
	echo "\ntest2:" >> log.txt
	./scanner ./test/2.c >> log.txt
	echo "\ntest3:" >> log.txt
	./scanner ./test/3.c >> log.txt
	echo "\ntest4:" >> log.txt
	./scanner ./test/4.c >> log.txt

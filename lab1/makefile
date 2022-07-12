scanner:main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o
	gcc main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o -lfl -o scanner
main.o:main.c 
	gcc -c main.c -o main.o
syntax_tree.o:syntax_tree.c
	gcc -c syntax_tree.c -o syntax_tree.o
syntax.tab.o:syntax.tab.c
	gcc -c syntax.tab.c -o syntax.tab.o
lex.yy.o:lex.yy.c syntax.tab.h
	gcc -c lex.yy.c -o lex.yy.o
error_handler.o:error_handler.c
	gcc -c error_handler.c -o error_handler.o
lex.yy.c:lexical.l syntax.tab.h
	flex lexical.l
syntax.tab.c syntax.tab.h:syntax.y
	bison -Wcounterexamples -d syntax.y
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
	echo "\ntest5:" >> log.txt
	./scanner ./test/5.c >> log.txt
	echo "\ntest6:" >> log.txt
	./scanner ./test/6.c >> log.txt
	echo "\ntest7:" >> log.txt
	./scanner ./test/7.c >> log.txt
	echo "\ntest8:" >> log.txt
	./scanner ./test/8.c >> log.txt
	echo "\ntest9:" >> log.txt
	./scanner ./test/9.c >> log.txt
	echo "\ntest10:" >> log.txt
	./scanner ./test/10.c >> log.txt
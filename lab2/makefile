scanner:main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o symbol_table.o handler.o
	gcc main.o lex.yy.o syntax.tab.o error_handler.o syntax_tree.o symbol_table.o handler.o -g -lfl -o scanner
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
	echo "\ntest11:" >> log.txt
	./scanner ./test/11.c >> log.txt
	echo "\ntest12:" >> log.txt
	./scanner ./test/12.c >> log.txt
	echo "\ntest13:" >> log.txt
	./scanner ./test/13.c >> log.txt
	echo "\ntest14:" >> log.txt
	./scanner ./test/14.c >> log.txt
	echo "\ntest15:" >> log.txt
	./scanner ./test/15.c >> log.txt
	echo "\ntest16:" >> log.txt
	./scanner ./test/16.c >> log.txt
	echo "\ntest17:" >> log.txt
	./scanner ./test/17.c >> log.txt
	echo "\ntest18:" >> log.txt
	./scanner ./test/18.c >> log.txt
	echo "\ntest19:" >> log.txt
	./scanner ./test/19.c >> log.txt
	echo "\ntest20:" >> log.txt
	./scanner ./test/20.c >> log.txt
	echo "\ntest21:" >> log.txt
	./scanner ./test/21.c >> log.txt
	echo "\ntest22:" >> log.txt
	./scanner ./test/22.c >> log.txt
	echo "\ntest23:" >> log.txt
	./scanner ./test/23.c >> log.txt

bison: ./bison.y
	bison bison.y -o bison.c --defines=bison.h

flex: ./flex.l
	flex -o flex.c flex.l

compile: bison flex ./main.c ./listmap.c
	gcc main.c flex.c bison.c listmap.c -o ../out/main

run: compile
	../out/main

test: compile
	cat ../out/test.lang | ../out/main > ../out/test.s
	cat ../out/test.s


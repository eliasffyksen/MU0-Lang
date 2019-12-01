
C_FILES=$(wildcard src/*.c)
H_FILES=$(wildcard src/*.h)

clean:
	rm -f out/*

bison: src/bison.y
	bison src/bison.y -o out/bison.c --defines=out/bison.h

flex: src/flex.l
	flex -o out/flex.c src/flex.l

compile: clean bison flex ${C_FILES} ${H_FILES}
	cp src/*.c src/*.h out/
	gcc out/*.c -o out/main

run: compile
	out/main



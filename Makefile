
C_FILES=$(wildcard src/*.c)
H_FILES=$(wildcard src/*.h)

compile: clean bison flex ${C_FILES} ${H_FILES}
	cp src/*.c src/*.h out/
	gcc out/*.c -o out/main

bison: src/bison.y
	bison src/bison.y -o out/bison.c --defines=out/bison.h

flex: src/flex.l
	flex -o out/flex.c src/flex.l

run: compile
	out/main

demo: compile
	cat demo.mu0 | ./out/main > out/demo.s; cat out/demo.s

clean:
	rm -f out/*

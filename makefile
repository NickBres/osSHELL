all: cmp copy encode decode stshell

stshell: stshell.c
	gcc -o stshell stshell.c

cmp: cmp.c
	gcc -o cmp cmp.c

copy: copy.c
	gcc -o copy copy.c

codecA.a: codecA.c codecA.h
	gcc -c codecA.c 
	ar rcs codecA.a codecA.o

codecB.a: codecB.c codecB.h
	gcc -c codecB.c 
	ar rcs codecB.a codecB.o

encode: encode.c codecA.a codecB.a
	gcc -o encode encode.c codecA.a codecB.a

decode: decode.c codecA.a codecB.a
	gcc -o decode decode.c codecA.a codecB.a

clean:
	rm -f *.o *.a cmp copy encode decode stshell
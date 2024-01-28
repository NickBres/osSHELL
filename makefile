all: codecA codecB cmp copy encode decode stshell
	

stshell: stshell.c
	gcc -o shell stshell.c

cmp: cmp.c
	gcc -o cmp cmp.c

copy: copy.c
	gcc -o copy copy.c

codecA: codecA.c codecA.h
	gcc -c -fPIC codecA.c -o codecA.o
	gcc -shared -o libcodecA.so codecA.o

codecB: codecB.c codecB.h
	gcc -c -fPIC codecB.c -o codecB.o
	gcc -shared -o libcodecB.so codecB.o

encode: encode.c
	gcc -o encode encode.c -L. -lcodecA -lcodecB

decode: decode.c
	gcc -o decode decode.c -L. -lcodecA -lcodecB


clean:
	rm -f *.o *.a *.so cmp copy encode decode shell

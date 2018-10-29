build: main.o mirror.o overlay.o
	gcc -g main.o mirror.o overlay.o mirror.h main.h overlay.h -o ppm-tools -lm
main.o: main.c
	gcc -g -Wall -c main.c -o main.o -lm
mirror.o: mirror.c
	gcc -g -Wall -c mirror.c -o mirror.o -lm
overlay.o: overlay.c
	gcc -g -Wall -c overlay.c -o overlay.o
clean:
	rm *.o

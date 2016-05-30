EXE=barber

all:
	gcc -o $(EXE) b.c -lpthread


clean:
	rm -f $(EXE)

CFLAGS = -std=c++11

all: default

default: Main

Main: Main.o
		g++ Main.o -o Main -lsfml-graphics -lsfml-window -lsfml-system

Main.o: Main.C
		g++ $(CFLAGS) -c Main.C

clean:
	  rm -f *.o Main

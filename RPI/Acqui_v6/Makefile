# Les parametres de compilation et edition des liens
# pour compiler un programme sous GNU/Linux+Xenomai

CFLAGS=-Wall $(shell xeno-config --skin=native --cflags)
LDFLAGS= -lm $(shell xeno-config --skin=native --ldflags)

#example_02: example_02.o
exercice2: exercice2.o

clean:
	rm -f *.o *.xg example_02 exercice2




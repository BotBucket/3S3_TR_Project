# Les parametres de compilation et edition des liens
# pour compiler un programme sous GNU/Linux+Xenomai

CFLAGS=-Wall $(shell xeno-config --skin=native --cflags)
LDFLAGS= -lm $(shell xeno-config --skin=native --ldflags)

#Acq_Ex: Acq_Ex.o
server_RPI: server_RPI.o

clean:
	rm -f *.o *.xg Acq_Ex server_RPI




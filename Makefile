CC=gcc
CFLAGS=-I.
LIBS=-lpthread
DEPS = module.h pedrolio.h
OBJ = main.o pedrolio.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

control: $(OBJ) mod_control/mod_control.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

grupo11: $(OBJ) mod_grupo11/grupo11_control.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: control grupo11

PHONY: clean

clean:
	rm -f *.o */*.o control

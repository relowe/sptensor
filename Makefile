CFLAGS=-I. -g
ALL=sptensortest
LDFLAGS=-lm
CC=gcc

all: $(ALL)
sptensor.o: sptensor.h sptensor.c
sptensorio.o: sptensorio.h sptensorio.c
vector.o: vector.h vector.c
view.o: view.h view.c
multiply.o: multiply.h multiply.c
sptensortest: sptensortest.o sptensor.o sptensorio.o vector.o view.o multiply.o
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@
clean:
	rm -f *.o $(ALL)

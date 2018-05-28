CFLAGS=-I. -g
ALL=sptensortest
LDFLAGS=-lm

all: $(ALL)
sptensor.o: sptensor.h sptensor.c
sptensorio.o: sptensorio.h sptensorio.c
vector.o: vector.h vector.c
view.o: view.h view.c
sptensortest: sptensortest.o sptensor.o sptensorio.o vector.o view.o
clean:
	rm -f *.o $(ALL)

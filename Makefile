CFLAGS=-I. -g
ALL=sptensortest

all: $(ALL)
sptensor.o: sptensor.h sptensor.c
sptensorio.o: sptensorio.h sptensorio.c
vector.o: vector.h vector.c
sptensortest: sptensortest.o sptensor.o sptensorio.o vector.o
clean:
	rm -f *.o $(ALL)

CFLAGS=-I. -g
ALL=sptensortest

all: $(ALL)
sptensor.o: sptensor.h sptensor.c
sptensorio.o: sptensorio.h sptensorio.c
sptensortest: sptensortest.o sptensor.o sptensorio.o
clean:
	rm -f *.o $(ALL)

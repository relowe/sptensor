CFLAGS=-I. -g -L.
ALL=sptensortest libsptensor.so libsptensor.a multiplytest
LDFLAGS=-lsptensor -lm
CC=gcc
SPTENSOR_LIB=sptensor.o sptensorio.o vector.o view.o multiply.o tensor_math.o

all: $(ALL)
libsptensor.so: $(SPTENSOR_LIB)
	gcc -o $@ $(CFLAGS) -fPIC $^ -shared
libsptensor.a: $(SPTENSOR_LIB)
	ar crf libsptensor.a $^
sptensor.o: sptensor.h sptensor.c
	gcc -c sptensor.c $(CFLAGS) -fPIC
sptensorio.o: sptensorio.h sptensorio.c
	gcc -c sptensorio.c $(CFLAGS) -fPIC
vector.o: vector.h vector.c
	gcc -c vector.c $(CFLAGS) -fPIC
view.o: view.h view.c
	gcc -c view.c $(CFLAGS) -fPIC
multiply.o: multiply.h multiply.c
	gcc -c multiply.c $(CFLAGS) -fPIC
tensor_math.o: tensor_math.h tensor_math.c
	gcc -c tensor_math.c $(CFLAGS) -fPIC
sptensortest: sptensortest.o libsptensor.a
	gcc $(CFLAGS) sptensortest.o $(LDFLAGS) -o $@ -static
multiplytest: multiplytest.o libsptensor.a
	gcc $(CFLAGS) multiplytest.o $(LDFLAGS) -o $@ -static
clean:
	rm -f *.o $(ALL)

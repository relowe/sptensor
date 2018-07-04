CFLAGS=-I./include -g -L./build/lib -std=c99
ALL=test/sptensortest build/lib/libsptensor.so build/lib/libsptensor.a test/multiplytest test/mathtest test/ccdtest build/bin/ntfd build/bin/factors build/bin/distance build/bin/tnsprint
LDFLAGS=-lsptensor -lm
CC=gcc
SPTENSOR_LIB=build/obj/sptensor.o build/obj/sptensorio.o build/obj/vector.o build/obj/view.o build/obj/multiply.o build/obj/tensor_math.o build/obj/ccd.o build/obj/binsearch.o

all: dirs $(ALL)
dirs: build/lib build/bin build/obj
build/lib:
	mkdir -p $@
build/bin:
	mkdir -p $@
build/obj:
	mkdir -p $@
build/lib/libsptensor.so: $(SPTENSOR_LIB)
	gcc -o $@ $(CFLAGS) -fPIC $^ -shared
build/lib/libsptensor.a: $(SPTENSOR_LIB)
	ar crf build/lib/libsptensor.a $^
build/obj/sptensor.o: include/sptensor/sptensor.h lib/storage.c
	gcc -o $@ -c lib/storage.c $(CFLAGS) -fPIC
build/obj/sptensorio.o: include/sptensor/sptensorio.h lib/sptensorio.c
	gcc -o $@ -c lib/sptensorio.c $(CFLAGS) -fPIC
build/obj/vector.o: include/sptensor/vector.h lib/vector.c
	gcc -o $@ -c lib/vector.c $(CFLAGS) -fPIC
build/obj/view.o: include/sptensor/view.h lib/view.c
	gcc -o $@ -c lib/view.c $(CFLAGS) -fPIC
build/obj/multiply.o: include/sptensor/multiply.h lib/multiply.c
	gcc -o $@ -c lib/multiply.c $(CFLAGS) -fPIC
build/obj/tensor_math.o: include/sptensor/tensor_math.h lib/tensor_math.c
	gcc -o $@ -c lib/tensor_math.c $(CFLAGS) -fPIC
build/obj/ccd.o: include/sptensor/ccd.h lib/ccd.c
	gcc -o $@ -c lib/ccd.c $(CFLAGS) -fPIC
build/obj/binsearch.o: include/sptensor/binsearch.h lib/binsearch.c
	gcc -o $@ -c lib/binsearch.c $(CFLAGS) -fPIC

#tool programs
build/bin/tnsprint: tool/tnsprint.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
build/bin/distance: tool/distance.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
build/bin/factors: tool/factors.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
build/bin/ntfd: tool/ntfd.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static

#test programs
test/ccdtest: test/ccdtest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
test/sptensortest: test/sptensortest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
test/multiplytest: test/multiplytest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
test/mathtest: test/mathtest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@ -static
clean:
	rm -rf *.o build $(ALL)

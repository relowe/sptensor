CFLAGS=-I./include -g -L./build/lib -std=c99 -g
ALL=test/sptensortest build/lib/libsptensor.so build/lib/libsptensor.a test/multiplytest test/mathtest test/ccdtest build/bin/sptensor
LDFLAGS=-lsptensor -lm
CC=gcc
SPTENSOR_LIB=build/obj/storage.o build/obj/sptensorio.o build/obj/vector.o build/obj/view.o build/obj/multiply.o build/obj/tensor_math.o build/obj/ccd.o build/obj/binsearch.o

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
build/obj/storage.o: include/sptensor/sptensor.h lib/storage.c
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

#tool program
build/obj/cmdargs.o: tool/cmdargs.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/cmdargs.c $(CFLAGS)
build/obj/distance.o: tool/distance.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/distance.c $(CFLAGS)
build/obj/factors.o: tool/factors.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/factors.c $(CFLAGS)
build/obj/ntfd.o: tool/ntfd.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/ntfd.c $(CFLAGS)
build/obj/series.o: tool/series.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/series.c $(CFLAGS)
build/obj/single.o: tool/single.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/single.c $(CFLAGS)
build/obj/sptensor.o: tool/sptensor.c tool/cmdargs.h tool/commands.h
	gcc -o $@ -c tool/sptensor.c $(CFLAGS)
build/bin/sptensor: build/obj/cmdargs.o build/obj/distance.o build/obj/factors.o build/obj/ntfd.o build/obj/series.o build/obj/single.o build/obj/sptensor.o build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@

#test programs
test/ccdtest: test/ccdtest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@
test/sptensortest: test/sptensortest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@
test/multiplytest: test/multiplytest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@
test/mathtest: test/mathtest.c build/lib/libsptensor.a
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@
clean:
	rm -rf *.o build $(ALL)

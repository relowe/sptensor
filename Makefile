CFLAGS=-I./include -g -L./build/lib -ansi -g
LDFLAGS=-lsptensor -lm
CC=gcc

SPTENSOR_LIB_SRCFILES := lib/index.c \
                         lib/vector.c

SPTENSOR_LIB_OBJFILES := $(patsubst lib/%.c, build/obj/%.o, $(SPTENSOR_LIB_SRCFILES))

SPTENSOR_LIB_TARGETS := build/lib/libsptensor.so \
                        build/lib/libsptensor.a

SPTENSOR_TEST_TARGETS := build/test/indextest


all: dirs $(SPTENSOR_LIB_TARGETS) $(SPTENSOR_TEST_TARGETS)
dirs: build/lib/ build/bin/ build/obj/ build/test/
build/%/:
	mkdir -p $@

#Library Builds
build/lib/libsptensor.so: $(SPTENSOR_LIB_OBJFILES)
	gcc -o $@ $(CFLAGS) -fPIC $^ -shared
build/lib/libsptensor.a: $(SPTENSOR_LIB_OBJFILES)
	ar crf build/lib/libsptensor.a $^

#Object Builds
build/obj/%.o: lib/%.c
	gcc -o $@ -c $< $(CFLAGS) -fPIC

#Test Builds
build/test/%: test/%.c
	gcc -o $@ $< -static $(CFLAGS) $(LDFLAGS) 



.PHONY: clean test-env
clean:
	rm -rf *.o build $(SPTENSOR_LIB_TARGETS)
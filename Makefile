CFLAGS=-I./include -g -L./build/lib -ansi -g
LDFLAGS=-lsptensor -lm -lgmp
CC=gcc

SPTENSOR_LIB_SRCFILES := lib/index.c \
                         lib/vector.c \
                         lib/index_iterator.c\
						 lib/coo.c\
						 lib/sptensor.c\
						 lib/inzt.c

SPTENSOR_LIB_OBJFILES := $(patsubst lib/%.c, build/obj/%.o, $(SPTENSOR_LIB_SRCFILES))

SPTENSOR_LIB_TARGETS := build/lib/libsptensor.so \
                        build/lib/libsptensor.a

SPTENSOR_TEST_SRCFILES := test/indextest.c\
	                      test/coo-test.c\
						  test/inzt-test.c\
						  test/morton-test.c\
						  test/skbttest.c

SPTENSOR_TEST_TARGETS := $(patsubst test/%.c,  build/test/%, $(SPTENSOR_TEST_SRCFILES))


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
build/test/%: test/%.c $(SPTENSOR_LIB_OBJFILES)
	gcc -o $@ $< -static $(CFLAGS) $(LDFLAGS) 



.PHONY: clean test-env
clean:
	rm -rf *.o build $(SPTENSOR_LIB_TARGETS)

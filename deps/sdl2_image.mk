UNAME := $(shell uname)
SOURCES := $(wildcard IMG*.c)
INCLUDES := -I../../deps/include/SDL2 -I.

ifeq ($(UNAME), Linux)
	CC := gcc
	CC_FLAGS := ${INCLUDES} -DLOAD_PNG -DLOAD_JPG
endif

ifeq ($(UNAME), Darwin)
	CC := clang
	SOURCES := ${SOURCES} IMG_ImageIO.m
	CC_FLAGS := ${INCLUDES} -D__APPLE__
endif

OBJECTS := $(patsubst %.m,build/%_m.o,$(patsubst %.c,build/%_c.o,$(SOURCES)))

build/%_m.o: %.m
	@echo $@
	@mkdir -p build
	@${CC} -c $< -o $@ ${CC_FLAGS}

build/%_c.o: %.c
	@echo $@
	@mkdir -p build
	@${CC} -c $< -o $@ ${CC_FLAGS}

release: ${OBJECTS}

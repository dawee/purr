UNAME := $(shell uname)
SOURCES := $(wildcard *.c)
INCLUDES := -I../../deps/include/SDL2 -I.

ifeq ($(UNAME), Linux)
	CC := gcc
	CC_FLAGS := ${INCLUDES}
endif

ifeq ($(UNAME), Darwin)
	CC := clang
	CC_FLAGS := ${INCLUDES}
endif

build/%_c.o: %.c build
	${CC} -c $< -o $@ ${CC_FLAGS}

build:
	@mkdir -p build

release: ${OBJECTS}

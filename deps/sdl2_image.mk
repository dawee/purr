UNAME := $(shell uname)
SOURCES := $(wildcard *.c)

ifeq ($(UNAME), Darwin)
	SOURCES := ${SOURCES} $(wildcard *.m)
endif

OBJECTS := $(patsubst %.m,build/%_m.o,$(patsubst %.c,build/%_c.o,$(SOURCES)))

build/%_m.o: %.m build
	@echo $< $@

build/%_c.o: %.c build
	@echo $< $@

build:
	@mkdir -p build

release: ${OBJECTS}

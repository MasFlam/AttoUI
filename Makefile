# This makefile probably does not work with anything except GCC and maybe Clang.
# It uses the -MMD flag so that the compiler generates make dependency files automagically.

LIBS := wayland-client harfbuzz fontconfig freetype2
CC = cc
CFLAGS := $(foreach lib,$(LIBS),$(shell pkg-config --cflags $(lib))) -Iinclude -std=c11 -g -O2 -D_GNU_SOURCE # TODO remove when memfd_create() is no longer used
LDFLAGS := $(foreach lib,$(LIBS),$(shell pkg-config --libs $(lib)))

EXAMPLES := $(wildcard example/*.c)
EXAMPLE_EXECS := $(patsubst %.c,target/%,$(EXAMPLES))
EXAMPLE_DEPS := $(patsubst %.c,target/%.d,$(EXAMPLES))
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,target/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
PIC_OBJS := $(patsubst src/%.c,target/pic-%.o,$(SRCS))
PIC_DEPS := $(PIC_OBJS:.o=.d)

.PHONY: clean static shared examples all

static: target target/libattoui.a
shared: target target/libattoui.so
examples: target/example $(EXAMPLE_EXECS)
all: static shared examples

target:
	mkdir $@

target/example: target
	mkdir $@

target/libattoui.a: $(OBJS)
	ar rcs $@ $^

target/%.o: src/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

target/libattoui.so: $(PIC_OBJS)
	$(CC) $^ -shared $(LDFLAGS) -o $@

target/pic-%.o: src/%.c
	$(CC) $(CFLAGS) -fPIC -MMD -c $< -o $@

-include $(DEPS)
-include $(PIC_DEPS)

target/example/%: example/%.c target/libattoui.a
	$(CC) $(CFLAGS) -MMD $< target/libattoui.a $(LDFLAGS) -o $@

-include $(EXAMPLE_DEPS)

clean:
	rm -rf target

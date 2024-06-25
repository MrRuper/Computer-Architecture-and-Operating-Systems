CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

.PHONY: all clean test libnand.so

# This will generate libnand.so and nand_example.c (or other tests).
all: libnand.so test

# Target for library compilation.
libnand.so: nand.o memory_tests.o
	$(CC) $(LDFLAGS) -o $@ $^

# The target for tests.
test: nand_example.o libnand.so
	$(CC) -o $@ $^ -L. -lnand

# Pattern for compiling .o from .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o libnand.so test

# Add .h dependency.
nand.o: nand.h
memory_tests.o: memory_tests.h
nand_example.o: nand.h

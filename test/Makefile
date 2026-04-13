CC      = gcc
CFLAGS  = -Wall -Wextra -g -DDEBUG -I../src

SRC     = ../src/udb.c
TESTS   = $(patsubst %.c,%,$(wildcard test_*.c))

.PHONY: all clean test

all: $(TESTS)

%: %.c $(SRC)
	$(CC) $(CFLAGS) -o $@ $^


test: $(TESTS)
	@echo ""
	@for t in $(TESTS); do \
		./$$t; \
	done


clean:
	rm -f $(TESTS)

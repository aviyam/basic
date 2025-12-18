# Makefile for BASIC Interpreter

# Detect OS/Architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Default Compiler and Flags
CC = cc
CFLAGS = -O
LDFLAGS = 

# Check for Windows (MinGW/Cygwin)
ifeq ($(UNAME_S),CYGWIN_NT)
    CC = gcc
    CFLAGS += -Wall -pedantic -std=c89 -D_POSIX_C_SOURCE=200809L -D_WIN32
    LDFLAGS += -lws2_32 # Link with Winsock for select()
else ifeq ($(UNAME_S),MINGW32_NT)
    CC = gcc
    CFLAGS += -Wall -pedantic -std=c89 -D_POSIX_C_SOURCE=200809L -D_WIN32
    LDFLAGS += -lws2_32
else
    # MacOS / Linux (POSIX)
    CFLAGS += -Wall -pedantic -std=c89 -D_POSIX_C_SOURCE=200809L
endif

TARGET = ./bin/basic
SRCS = ./src/main.c ./src/token.c ./src/eval.c ./src/exec.c ./src/var.c
# SRCS = main.c token.c eval.c exec.c var.c
OBJS = $(SRCS:.c=.o)

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) -lm
	rm -f $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

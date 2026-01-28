# Makefile for BASIC Interpreter

# Detect OS/Architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Default Compiler and Flags
CC = cc
CFLAGS = -Os
LDFLAGS = 

# Check for Windows (MinGW/Cygwin/MSYS)
ifneq (,$(findstring CYGWIN,$(UNAME_S))$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
    CC = gcc
    CFLAGS += -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -D_WIN32
    LDFLAGS += -lws2_32
    EXTENSION = .exe
else
    # MacOS / Linux (POSIX)
    CFLAGS += -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L
    EXTENSION =
endif

# Get the current Git tag (if any)
GIT_TAG := $(shell git describe --tags --abbrev=0 2>/dev/null)
# Get the short commit hash (5 characters)
GIT_HASH := $(shell git rev-parse --short=5 HEAD 2>/dev/null)

# Construct the version string based on whether a tag exists.
# If a tag exists, append the short commit hash.
# If no tag exists, default to "UNKNOWN_VERSION".
ifeq ($(GIT_TAG),)
    # No tag found, use the default "UNKNOWN_VERSION"
    VERSION := UNKNOWN_VERSION
else
    # Tag found, append the short commit hash if available
    VERSION := $(GIT_TAG)$(if $(GIT_HASH),.$(GIT_HASH))
endif

CFLAGS += -DBASIC_VERSION="\"$(VERSION)\""

TARGET = ./bin/basic$(EXTENSION)
SRCS = ./src/main.c ./src/token.c ./src/eval.c ./src/exec.c ./src/var.c

OBJS = $(SRCS:.c=.o)

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p ./bin
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) -lm
	strip $(TARGET)
	@./tools/set_icon.sh $(TARGET) || echo "Warning: Failed to set icon"
	rm -f $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Update README version to match git tag
update-version:
	@./tools/update_version.sh

clean:
	rm -f $(TARGET) $(OBJS)

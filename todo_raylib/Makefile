# 1. Detect Operating System
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = MAC
    endif
endif

# 2. General Variables
TARGET = main
SRC = $(wildcard *.c)                 # NEW: compile every .c in the folder
CC = gcc
CFLAGS = -O3 -march=native -flto -s -Wall -std=gnu99 -O2 -I.     # NEW: gnu99 (gives M_PI) + -I. for raygui.h

# 3. OS-Specific Configurations
ifeq ($(PLATFORM),MAC)
    HOMEBREW_PREFIX = $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
    CFLAGS += -I$(HOMEBREW_PREFIX)/include
    LDFLAGS = -L$(HOMEBREW_PREFIX)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    RM = rm -f
    EXEC = ./bin/$(TARGET)

else ifeq ($(PLATFORM),LINUX)
    CFLAGS += -I/usr/local/include
    LDFLAGS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RM = rm -f
    EXEC = ./bin/$(TARGET)

else ifeq ($(PLATFORM),WINDOWS)
    CC = gcc
    TARGET := $(TARGET).exe
    CFLAGS += -IC:/msys64/ucrt64/include
    LDFLAGS = -LC:/msys64/ucrt64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    RM = del /Q
    EXEC = ./bin/$(TARGET)
endif
# 4. Build Rules
.PHONY: all run clean clangd                          # NEW

all: $(TARGET) compile_flags.txt                      # NEW: also writes clangd file

$(TARGET): $(SRC) raylib.h raygui.h                            # NEW: needs raygui.h
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)
	mv $(TARGET) bin/

#download raylib if its misisng 
raylib.h:
	curl -fsSL -o raylib.h https://raw.githubusercontent.com/raysan5/raylib/master/src/raylib.h
	# NEW: download raygui once if it's missing
raygui.h:
	curl -fsSL -o raygui.h https://raw.githubusercontent.com/raysan5/raygui/5.0/src/raygui.h

# NEW: tells clangd (nvim) where the headers are
compile_flags.txt: Makefile
	@printf '%s\n' $(CFLAGS) > $@

clangd: compile_flags.txt                             # NEW

run: $(TARGET)
	$(EXEC)

clean:
	$(RM) $(TARGET)

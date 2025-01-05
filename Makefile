# Define the compiler and the flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include/json-c

# Define the target executable
TARGET = romm

# Define the source files
SRCS = $(wildcard src/*.c)

# Define the object files
OBJS = $(SRCS:.c=.o)

# Define the JSON-C library
LIB_JSON_C = ./lib/libjson-c.a  # Path to the static JSON-C library

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_JSON_C)

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)

# Define the compiler and the flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include  # Include all headers in ./include

# Define the target executable
TARGET = romm

# Define the source files
SRCS = $(wildcard src/*.c)

# Define the object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -L./lib -ljson-c

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)

# Define the compiler and the flags
CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -Wextra -O2 -DSDL=1 -I./include -I$(PREFIX)/include
LDFLAGS=-L./lib -L$(PREFIX)/lib -ljson-c -lSDL -lSDL_ttf
LDLIBS=-DSDL=1 -lSDL -lpthread -lSDL_ttf

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
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^ $(LDFLAGS)

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)

INSTALL_DIR = /usr/local/bin

# Add an 'install' rule
install: $(TARGET)
	install -m 755 $(TARGET) $(INSTALL_DIR)/$(TARGET)

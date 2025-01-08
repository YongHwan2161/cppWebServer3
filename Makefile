GCC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = cgdb
SRCS = $(wildcard *.c src/*.c src/*/*.c)
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h src/*.h src/*/*.h)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(GCC) $(OBJS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(GCC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	rm -f $(OBJS)
	./$(TARGET)


clean:
	rm -f $(OBJS) $(TARGET) 
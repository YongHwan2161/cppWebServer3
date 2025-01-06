GCC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = cgdb
SRCS = CGDB.c src/free_space.c src/init.c
OBJS = $(SRCS:.c=.o)
HEADERS = CGDB.h src/free_space.h src/init.h

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
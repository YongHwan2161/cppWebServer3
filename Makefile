GCC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = cgdb
SRCS = CGDB.c src/free_space.c src/init.c src/database.c src/axis.c src/channel.c src/tests/axis_tests.c src/cli/command_handler.c
OBJS = $(SRCS:.c=.o)
HEADERS = CGDB.h src/free_space.h src/init.h src/database.h src/axis.h src/channel.h src/tests/axis_tests.h src/cli/command_handler.h

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
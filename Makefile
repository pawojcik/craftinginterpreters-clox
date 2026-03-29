CC = clang
CFLAGS = -Wall
TARGET = clox

all: $(TARGET)

$(TARGET): *.c
	$(CC) $(CFLAGS) *.c -o $(TARGET)

debug: all
	leaks -atExit -- ./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf clox.dSYM
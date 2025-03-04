CC = clang
CFLAGS = -Wall -Werror -O3
TARGET = csr
SRC = csr.c memoryarena.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
# Makefile for Huffman Coding Compression Tool

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = huffman
SOURCE = huffman.c

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Clean compiled files
clean:
	rm -f $(TARGET) *.o

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Test compression and decompression
test: $(TARGET)
	@echo "Testing compression..."
	./$(TARGET) -c test_file.txt test_file.huf
	@echo "Testing decompression..."
	./$(TARGET) -d test_file.huf test_file_restored.txt
	@echo "Comparing original and restored files..."
	diff test_file.txt test_file_restored.txt && echo "Files are identical - Test PASSED!" || echo "Files differ - Test FAILED!"
	@echo "Showing statistics..."
	./$(TARGET) -s test_file.txt test_file.huf

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Compile the program"
	@echo "  clean     - Remove compiled files"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  test      - Run compression/decompression test"
	@echo "  help      - Show this help"

.PHONY: all clean install uninstall test help
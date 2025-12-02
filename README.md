# Huffman Coding Compression Tool




A complete implementation of the Huffman coding algorithm in C for lossless data compression and decompression.

## Features

- **Lossless Compression**: Uses Huffman coding algorithm to compress files without data loss
- **Efficient Data Structures**: Custom implementation of min-heap for optimal tree construction
- **Bit-level Operations**: Efficient bit packing and unpacking for maximum compression
- **File Format Support**: Custom binary format with header information for reliable decompression
- **Command-line Interface**: Easy to use CLI with multiple operation modes
- **Interactive Mode**: Menu-driven interface when no arguments are provided
- **Statistics**: Detailed compression statistics including ratio and space savings
- **Error Handling**: Robust error handling for file operations and memory management
- **Memory Management**: Careful allocation and deallocation to prevent memory leaks

## Compilation

### Using Make (Recommended)
```bash
make
```

### Manual Compilation
```bash
gcc -Wall -Wextra -std=c99 -O2 -o huffman huffman.c
```

## Usage

### Command Line Options

```bash
# Compress a file
./huffman -c input.txt compressed.huf

# Decompress a file
./huffman -d compressed.huf output.txt

# Show compression statistics
./huffman -s original.txt compressed.huf

# Show help
./huffman -h
```

### Interactive Mode

Run the program without arguments to enter interactive mode:

```bash
./huffman
```

## Testing

Run the included test suite:

```bash
make test
```

This will:
1. Compress the test file
2. Decompress it back
3. Compare original and restored files
4. Show compression statistics

## Algorithm Details

### Huffman Coding Process

1. **Frequency Analysis**: Count character frequencies in input file
2. **Tree Construction**: Build Huffman tree using min-heap priority queue
3. **Code Generation**: Generate binary codes by traversing tree paths
4. **Encoding**: Replace characters with their Huffman codes
5. **Compression**: Pack bits efficiently and write to binary file

### File Format

The compressed file format includes:
- Magic number (4 bytes): File format identifier
- Original size (4 bytes): Size of original file in bytes
- Compressed size (4 bytes): Size of compressed data in bytes
- Frequency count (4 bytes): Number of unique characters
- Padding bits (1 byte): Number of padding bits in last byte
- Frequency table: Character frequencies for tree reconstruction
- Compressed data: Huffman-encoded bit stream

## Performance

- **Space Complexity**: O(n) where n is the number of unique characters
- **Time Complexity**: O(n log n) for tree construction, O(m) for encoding/decoding where m is file size
- **Compression Ratio**: Varies based on input data; typically 40-70% for text files

## Error Handling

The program handles various error conditions:
- File not found or permission errors
- Empty input files
- Invalid compressed file format
- Memory allocation failures
- Corrupted compressed data

## Memory Management

- Automatic cleanup of all allocated memory
- Proper handling of file resources
- No memory leaks under normal operation

## Limitations

- Maximum file size limited by available memory (entire file loaded into memory)
- ASCII character support (0-255)
- Single-threaded operation

## Example Output

```
Huffman Coding Compression Tool v1.0
=====================================

=== COMPRESSION STARTED ===
Input file: test_file.txt
Output file: test_file.huf
File size: 456 bytes
Huffman tree constructed successfully
Huffman codes generated successfully
=== COMPRESSION COMPLETED ===
Compression completed in 0.01 seconds

=== COMPRESSION STATISTICS ===
Original size:    456 bytes
Compressed size:  267 bytes
Compression ratio: 0.59
Space saved:      41.45%
===============================
```

## Contributing

Feel free to submit issues and enhancement requests!

## License

This project is open source and available under the MIT License.


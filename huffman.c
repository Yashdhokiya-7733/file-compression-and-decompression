#include "huffman.h"

// =============================================================================
// MEMORY MANAGEMENT FUNCTIONS
// =============================================================================

/**
 * Creates a new Huffman tree node
 * @param character The character to store (0 for internal nodes)
 * @param frequency The frequency of the character
 * @return Pointer to the created node or NULL on failure
 */
HuffmanNode* createNode(unsigned char character, unsigned int frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for Huffman node\n");
        return NULL;
    }

    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

/**
 * Recursively destroys the Huffman tree and frees memory
 * @param root Root of the tree to destroy
 */
void destroyTree(HuffmanNode* root) {
    if (root) {
        destroyTree(root->left);
        destroyTree(root->right);
        free(root);
    }
}

/**
 * Destroys the min-heap and frees associated memory
 * @param heap Pointer to the heap to destroy
 */
void destroyHeap(MinHeap* heap) {
    if (heap) {
        if (heap->nodes) {
            free(heap->nodes);
        }
        free(heap);
    }
}

// =============================================================================
// MIN-HEAP OPERATIONS
// =============================================================================

/**
 * Creates a new min-heap with specified capacity
 * @param capacity Maximum number of nodes the heap can hold
 * @return Pointer to created heap or NULL on failure
 */
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) {
        fprintf(stderr, "Error: Memory allocation failed for min-heap\n");
        return NULL;
    }

    heap->nodes = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    if (!heap->nodes) {
        fprintf(stderr, "Error: Memory allocation failed for heap nodes\n");
        free(heap);
        return NULL;
    }

    heap->size = 0;
    heap->capacity = capacity;

    return heap;
}

/**
 * Maintains min-heap property by moving element down
 * @param heap Pointer to the heap
 * @param index Index of element to heapify
 */
void minHeapify(MinHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && 
        heap->nodes[left]->frequency < heap->nodes[smallest]->frequency) {
        smallest = left;
    }

    if (right < heap->size && 
        heap->nodes[right]->frequency < heap->nodes[smallest]->frequency) {
        smallest = right;
    }

    if (smallest != index) {
        HuffmanNode* temp = heap->nodes[smallest];
        heap->nodes[smallest] = heap->nodes[index];
        heap->nodes[index] = temp;

        minHeapify(heap, smallest);
    }
}

/**
 * Extracts the minimum frequency node from the heap
 * @param heap Pointer to the heap
 * @return Pointer to the extracted node or NULL if heap is empty
 */
HuffmanNode* extractMin(MinHeap* heap) {
    if (heap->size <= 0) {
        return NULL;
    }

    HuffmanNode* root = heap->nodes[0];

    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;

    if (heap->size > 0) {
        minHeapify(heap, 0);
    }

    return root;
}

/**
 * Inserts a new node into the min-heap
 * @param heap Pointer to the heap
 * @param node Node to insert
 */
void insertMinHeap(MinHeap* heap, HuffmanNode* node) {
    if (heap->size >= heap->capacity) {
        fprintf(stderr, "Error: Heap overflow\n");
        return;
    }

    heap->size++;
    int i = heap->size - 1;

    // Move up until heap property is satisfied
    while (i && node->frequency < heap->nodes[(i - 1) / 2]->frequency) {
        heap->nodes[i] = heap->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    heap->nodes[i] = node;
}

/**
 * Checks if the heap is empty
 * @param heap Pointer to the heap
 * @return 1 if empty, 0 otherwise
 */
int isHeapEmpty(MinHeap* heap) {
    return heap->size == 0;
}

// =============================================================================
// FREQUENCY CALCULATION
// =============================================================================

/**
 * Calculates character frequencies from input file
 * @param filename Path to input file
 * @param frequencies Array to store frequencies (size ASCII_SIZE)
 * @return File size in bytes, or -1 on error
 */
int calculateFrequencies(const char* filename, unsigned int frequencies[ASCII_SIZE]) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for reading\n", filename);
        return -1;
    }

    // Initialize frequency array
    memset(frequencies, 0, ASCII_SIZE * sizeof(unsigned int));

    int ch;
    int fileSize = 0;

    // Read file character by character and count frequencies
    while ((ch = fgetc(file)) != EOF) {
        frequencies[(unsigned char)ch]++;
        fileSize++;
    }

    fclose(file);

    if (fileSize == 0) {
        fprintf(stderr, "Error: Input file is empty\n");
        return -1;
    }

    printf("File size: %d bytes\n", fileSize);
    return fileSize;
}

// =============================================================================
// HUFFMAN TREE CONSTRUCTION
// =============================================================================

/**
 * Builds the Huffman tree from character frequencies
 * @param frequencies Array of character frequencies
 * @return Root of the constructed Huffman tree or NULL on error
 */
HuffmanNode* buildHuffmanTree(unsigned int frequencies[ASCII_SIZE]) {
    // Count number of unique characters
    int uniqueChars = 0;
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (frequencies[i] > 0) {
            uniqueChars++;
        }
    }

    if (uniqueChars == 0) {
        fprintf(stderr, "Error: No characters found in input\n");
        return NULL;
    }

    // Handle single character case
    if (uniqueChars == 1) {
        for (int i = 0; i < ASCII_SIZE; i++) {
            if (frequencies[i] > 0) {
                HuffmanNode* root = createNode(0, frequencies[i]);
                root->left = createNode((unsigned char)i, frequencies[i]);
                return root;
            }
        }
    }

    // Create min-heap and populate with leaf nodes
    MinHeap* heap = createMinHeap(uniqueChars);
    if (!heap) {
        return NULL;
    }

    for (int i = 0; i < ASCII_SIZE; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode* node = createNode((unsigned char)i, frequencies[i]);
            if (!node) {
                destroyHeap(heap);
                return NULL;
            }
            insertMinHeap(heap, node);
        }
    }

    // Build Huffman tree
    while (heap->size > 1) {
        // Extract two nodes with minimum frequency
        HuffmanNode* left = extractMin(heap);
        HuffmanNode* right = extractMin(heap);

        if (!left || !right) {
            fprintf(stderr, "Error: Failed to extract nodes from heap\n");
            destroyHeap(heap);
            return NULL;
        }

        // Create new internal node
        HuffmanNode* merged = createNode(0, left->frequency + right->frequency);
        if (!merged) {
            destroyHeap(heap);
            return NULL;
        }

        merged->left = left;
        merged->right = right;

        insertMinHeap(heap, merged);
    }

    // Extract root node
    HuffmanNode* root = extractMin(heap);
    destroyHeap(heap);

    printf("Huffman tree constructed successfully\n");
    return root;
}

// =============================================================================
// CODE GENERATION
// =============================================================================

/**
 * Recursively generates Huffman codes for each character
 * @param root Current node in the tree
 * @param codes Array to store generated codes
 * @param currentCode Current code being built
 * @param depth Current depth in the tree
 */
void generateCodes(HuffmanNode* root, CodeEntry codes[ASCII_SIZE], 
                   char* currentCode, int depth) {
    if (!root) return;

    // Leaf node - store the code
    if (!root->left && !root->right) {
        currentCode[depth] = '\0';
        strcpy(codes[root->character].code, currentCode);
        codes[root->character].length = depth;
        return;
    }

    // Internal node - continue recursion
    if (root->left) {
        currentCode[depth] = '0';
        generateCodes(root->left, codes, currentCode, depth + 1);
    }

    if (root->right) {
        currentCode[depth] = '1';
        generateCodes(root->right, codes, currentCode, depth + 1);
    }
}

/**
 * Builds the complete code table from Huffman tree
 * @param root Root of the Huffman tree
 * @param codes Array to store the generated codes
 */
void buildCodeTable(HuffmanNode* root, CodeEntry codes[ASCII_SIZE]) {
    // Initialize code table
    for (int i = 0; i < ASCII_SIZE; i++) {
        codes[i].code[0] = '\0';
        codes[i].length = 0;
    }

    if (!root) return;

    // Handle single character case
    if (!root->left && !root->right) {
        codes[root->character].code[0] = '0';
        codes[root->character].code[1] = '\0';
        codes[root->character].length = 1;
        return;
    }

    char currentCode[MAX_CODE_LENGTH];
    generateCodes(root, codes, currentCode, 0);

    printf("Huffman codes generated successfully\n");
}

// =============================================================================
// BIT OPERATIONS
// =============================================================================

/**
 * Writes a single bit to file using bit buffer
 * @param file Output file pointer
 * @param bitBuffer Buffer for bit operations
 * @param bit Bit to write (0 or 1)
 */
void writeBit(FILE* file, BitBuffer* bitBuffer, int bit) {
    bitBuffer->buffer = (bitBuffer->buffer << 1) | (bit & 1);
    bitBuffer->bitCount++;

    if (bitBuffer->bitCount == 8) {
        fputc(bitBuffer->buffer, file);
        bitBuffer->buffer = 0;
        bitBuffer->bitCount = 0;
    }
}

/**
 * Flushes remaining bits in buffer to file
 * @param file Output file pointer
 * @param bitBuffer Buffer for bit operations
 */
void flushBits(FILE* file, BitBuffer* bitBuffer) {
    if (bitBuffer->bitCount > 0) {
        bitBuffer->buffer <<= (8 - bitBuffer->bitCount);
        fputc(bitBuffer->buffer, file);
    }
}

/**
 * Reads a single bit from file
 * @param file Input file pointer
 * @param currentByte Current byte being processed
 * @param bitPosition Current bit position in byte
 * @return Bit value (0 or 1) or -1 on EOF
 */
int readBit(FILE* file, unsigned char* currentByte, int* bitPosition) {
    if (*bitPosition == 0) {
        int ch = fgetc(file);
        if (ch == EOF) {
            return -1;
        }
        *currentByte = (unsigned char)ch;
        *bitPosition = 8;
    }

    (*bitPosition)--;
    return (*currentByte >> *bitPosition) & 1;
}

// =============================================================================
// COMPRESSION FUNCTIONS
// =============================================================================

/**
 * Writes file header to compressed file
 * @param file Output file pointer
 * @param header Header structure to write
 */
void writeFileHeader(FILE* file, FileHeader* header) {
    fwrite(&header->magic, sizeof(uint32_t), 1, file);
    fwrite(&header->original_size, sizeof(uint32_t), 1, file);
    fwrite(&header->compressed_size, sizeof(uint32_t), 1, file);
    fwrite(&header->frequency_count, sizeof(uint32_t), 1, file);
    fwrite(&header->padding_bits, sizeof(uint8_t), 1, file);
}

/**
 * Writes character frequencies to compressed file
 * @param file Output file pointer
 * @param frequencies Array of character frequencies
 */
void writeFrequencies(FILE* file, unsigned int frequencies[ASCII_SIZE]) {
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (frequencies[i] > 0) {
            fputc((unsigned char)i, file);
            fwrite(&frequencies[i], sizeof(unsigned int), 1, file);
        }
    }
}

/**
 * Encodes input data and writes to output file
 * @param inputFile Input file pointer
 * @param outputFile Output file pointer
 * @param codes Array of Huffman codes
 */
void encodeAndWrite(FILE* inputFile, FILE* outputFile, CodeEntry codes[ASCII_SIZE]) {
    BitBuffer bitBuffer = {0, 0};
    int ch;

    rewind(inputFile);

    while ((ch = fgetc(inputFile)) != EOF) {
        unsigned char character = (unsigned char)ch;

        if (codes[character].length == 0) {
            fprintf(stderr, "Error: No code found for character %d\n", character);
            continue;
        }

        // Write each bit of the character's code
        for (int i = 0; i < codes[character].length; i++) {
            int bit = (codes[character].code[i] == '1') ? 1 : 0;
            writeBit(outputFile, &bitBuffer, bit);
        }
    }

    flushBits(outputFile, &bitBuffer);
}

/**
 * Compresses a file using Huffman coding
 * @param inputFile Path to input file
 * @param outputFile Path to output file
 * @return 0 on success, -1 on error
 */
int compressFile(const char* inputFile, const char* outputFile) {
    printf("\n=== COMPRESSION STARTED ===\n");
    printf("Input file: %s\n", inputFile);
    printf("Output file: %s\n", outputFile);

    // Calculate frequencies
    unsigned int frequencies[ASCII_SIZE];
    int originalSize = calculateFrequencies(inputFile, frequencies);
    if (originalSize < 0) {
        return -1;
    }

    // Build Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencies);
    if (!root) {
        fprintf(stderr, "Error: Failed to build Huffman tree\n");
        return -1;
    }

    // Generate codes
    CodeEntry codes[ASCII_SIZE];
    buildCodeTable(root, codes);

    // Open files
    FILE* inFile = fopen(inputFile, "rb");
    FILE* outFile = fopen(outputFile, "wb");

    if (!inFile || !outFile) {
        fprintf(stderr, "Error: Cannot open files for compression\n");
        if (inFile) fclose(inFile);
        if (outFile) fclose(outFile);
        destroyTree(root);
        return -1;
    }

    // Count unique characters for header
    uint32_t frequencyCount = 0;
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (frequencies[i] > 0) {
            frequencyCount++;
        }
    }

    // Write header (placeholder, will be updated later)
    FileHeader header = {
        .magic = MAGIC_NUMBER,
        .original_size = (uint32_t)originalSize,
        .compressed_size = 0,
        .frequency_count = frequencyCount,
        .padding_bits = 0
    };

    writeFileHeader(outFile, &header);
    writeFrequencies(outFile, frequencies);

    // Encode and write data
    long dataStart = ftell(outFile);
    encodeAndWrite(inFile, outFile, codes);
    long dataEnd = ftell(outFile);

    // Calculate padding bits
    header.compressed_size = (uint32_t)(dataEnd - dataStart);

    // Update header with actual compressed size
    fseek(outFile, 0, SEEK_SET);
    writeFileHeader(outFile, &header);

    fclose(inFile);
    fclose(outFile);
    destroyTree(root);

    printf("=== COMPRESSION COMPLETED ===\n");
    return 0;
}

// =============================================================================
// DECOMPRESSION FUNCTIONS
// =============================================================================

/**
 * Reads file header from compressed file
 * @param file Input file pointer
 * @param header Header structure to populate
 * @return 0 on success, -1 on error
 */
int readFileHeader(FILE* file, FileHeader* header) {
    if (fread(&header->magic, sizeof(uint32_t), 1, file) != 1 ||
        fread(&header->original_size, sizeof(uint32_t), 1, file) != 1 ||
        fread(&header->compressed_size, sizeof(uint32_t), 1, file) != 1 ||
        fread(&header->frequency_count, sizeof(uint32_t), 1, file) != 1 ||
        fread(&header->padding_bits, sizeof(uint8_t), 1, file) != 1) {
        return -1;
    }

    if (header->magic != MAGIC_NUMBER) {
        fprintf(stderr, "Error: Invalid file format (magic number mismatch)\n");
        return -1;
    }

    return 0;
}

/**
 * Reads character frequencies from compressed file
 * @param file Input file pointer
 * @param frequencies Array to populate with frequencies
 * @param count Number of unique characters to read
 * @return 0 on success, -1 on error
 */
int readFrequencies(FILE* file, unsigned int frequencies[ASCII_SIZE], int count) {
    memset(frequencies, 0, ASCII_SIZE * sizeof(unsigned int));

    for (int i = 0; i < count; i++) {
        int ch = fgetc(file);
        if (ch == EOF) {
            fprintf(stderr, "Error: Unexpected end of file while reading frequencies\n");
            return -1;
        }

        unsigned char character = (unsigned char)ch;
        if (fread(&frequencies[character], sizeof(unsigned int), 1, file) != 1) {
            fprintf(stderr, "Error: Failed to read frequency data\n");
            return -1;
        }
    }

    return 0;
}

/**
 * Decodes compressed data and writes to output file
 * @param inputFile Input file pointer
 * @param outputFile Output file pointer
 * @param root Root of Huffman tree
 * @param originalSize Original file size in bytes
 * @param paddingBits Number of padding bits in last byte
 */
void decodeAndWrite(FILE* inputFile, FILE* outputFile, HuffmanNode* root, 
                   unsigned int originalSize, int paddingBits) {
    if (!root) return;

    HuffmanNode* currentNode = root;
    unsigned char currentByte = 0;
    int bitPosition = 0;
    int decodedBytes = 0;

    while (decodedBytes < originalSize) {
        int bit = readBit(inputFile, &currentByte, &bitPosition);
        if (bit == -1) break;

        // Navigate tree based on bit
        if (bit == 0) {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        // Check if we reached a leaf node
        if (!currentNode->left && !currentNode->right) {
            fputc(currentNode->character, outputFile);
            decodedBytes++;
            currentNode = root; // Reset to root for next character
        }

        if (!currentNode) {
            fprintf(stderr, "Error: Invalid bit sequence encountered during decoding\n");
            break;
        }
    }
}

/**
 * Decompresses a Huffman-encoded file
 * @param inputFile Path to compressed file
 * @param outputFile Path to output file
 * @return 0 on success, -1 on error
 */
int decompressFile(const char* inputFile, const char* outputFile) {
    printf("\n=== DECOMPRESSION STARTED ===\n");
    printf("Input file: %s\n", inputFile);
    printf("Output file: %s\n", outputFile);

    FILE* inFile = fopen(inputFile, "rb");
    if (!inFile) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", inputFile);
        return -1;
    }

    // Read header
    FileHeader header;
    if (readFileHeader(inFile, &header) != 0) {
        fclose(inFile);
        return -1;
    }

    printf("Original size: %u bytes\n", header.original_size);
    printf("Compressed size: %u bytes\n", header.compressed_size);
    printf("Unique characters: %u\n", header.frequency_count);

    // Read frequencies
    unsigned int frequencies[ASCII_SIZE];
    if (readFrequencies(inFile, frequencies, header.frequency_count) != 0) {
        fclose(inFile);
        return -1;
    }

    // Rebuild Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencies);
    if (!root) {
        fprintf(stderr, "Error: Failed to rebuild Huffman tree\n");
        fclose(inFile);
        return -1;
    }

    // Open output file
    FILE* outFile = fopen(outputFile, "wb");
    if (!outFile) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", outputFile);
        fclose(inFile);
        destroyTree(root);
        return -1;
    }

    // Decode data
    decodeAndWrite(inFile, outFile, root, header.original_size, header.padding_bits);

    fclose(inFile);
    fclose(outFile);
    destroyTree(root);

    printf("=== DECOMPRESSION COMPLETED ===\n");
    return 0;
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * Prints compression statistics
 * @param inputFile Path to original file
 * @param outputFile Path to compressed file
 */
void printCompressionStats(const char* inputFile, const char* outputFile) {
    FILE* inFile = fopen(inputFile, "rb");
    FILE* outFile = fopen(outputFile, "rb");

    if (!inFile || !outFile) {
        fprintf(stderr, "Error: Cannot open files for statistics\n");
        if (inFile) fclose(inFile);
        if (outFile) fclose(outFile);
        return;
    }

    // Get file sizes
    fseek(inFile, 0, SEEK_END);
    long originalSize = ftell(inFile);

    fseek(outFile, 0, SEEK_END);
    long compressedSize = ftell(outFile);

    fclose(inFile);
    fclose(outFile);

    double compressionRatio = (double)compressedSize / originalSize;
    double spaceSaving = ((double)(originalSize - compressedSize) / originalSize) * 100;

    printf("\n=== COMPRESSION STATISTICS ===\n");
    printf("Original size:    %ld bytes\n", originalSize);
    printf("Compressed size:  %ld bytes\n", compressedSize);
    printf("Compression ratio: %.2f\n", compressionRatio);
    printf("Space saved:      %.2f%%\n", spaceSaving);
    printf("===============================\n");
}

/**
 * Prints Huffman codes for all characters
 * @param codes Array of Huffman codes
 */
void printHuffmanCodes(CodeEntry codes[ASCII_SIZE]) {
    printf("\n=== HUFFMAN CODES ===\n");
    printf("Char\tASCII\tCode\tLength\n");
    printf("----\t-----\t----\t------\n");

    for (int i = 0; i < ASCII_SIZE; i++) {
        if (codes[i].length > 0) {
            char displayChar = (i >= 32 && i <= 126) ? (char)i : '?';
            printf("%c\t%d\t%s\t%d\n", displayChar, i, codes[i].code, codes[i].length);
        }
    }
    printf("===================\n");
}

/**
 * Validates input and output file paths
 * @param inputFile Path to input file
 * @param outputFile Path to output file
 * @return 0 on success, -1 on error
 */
int validateFiles(const char* inputFile, const char* outputFile) {
    if (!inputFile || !outputFile) {
        fprintf(stderr, "Error: File paths cannot be NULL\n");
        return -1;
    }

    FILE* inFile = fopen(inputFile, "rb");
    if (!inFile) {
        fprintf(stderr, "Error: Cannot access input file '%s'\n", inputFile);
        return -1;
    }
    fclose(inFile);

    // Check if output file can be created
    FILE* outFile = fopen(outputFile, "wb");
    if (!outFile) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", outputFile);
        return -1;
    }
    fclose(outFile);

    return 0;
}

/**
 * Prints usage information
 * @param programName Name of the program
 */
void printUsage(const char* programName) {
    printf("\n=== HUFFMAN CODING COMPRESSION TOOL ===\n");
    printf("Usage: %s [OPTIONS]\n", programName);
    printf("\nOptions:\n");
    printf("  -c <input> <output>    Compress input file to output file\n");
    printf("  -d <input> <output>    Decompress input file to output file\n");
    printf("  -s <original> <compressed>  Show compression statistics\n");
    printf("  -h                     Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -c document.txt document.huf\n", programName);
    printf("  %s -d document.huf document_restored.txt\n", programName);
    printf("  %s -s document.txt document.huf\n", programName);
    printf("=====================================\n");
}

// =============================================================================
// MAIN FUNCTION AND COMMAND LINE INTERFACE
// =============================================================================

/**
 * Interactive menu for user input
 */
void interactiveMenu() {
    char inputFile[256], outputFile[256];
    int choice;

    while (1) {
        printf("\n=== HUFFMAN CODING TOOL ===\n");
        printf("1. Compress a file\n");
        printf("2. Decompress a file\n");
        printf("3. Show compression statistics\n");
        printf("4. Exit\n");
        printf("Enter your choice (1-4): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter input file path: ");
                scanf("%255s", inputFile);
                printf("Enter output file path: ");
                scanf("%255s", outputFile);

                if (validateFiles(inputFile, outputFile) == 0) {
                    clock_t start = clock();
                    if (compressFile(inputFile, outputFile) == 0) {
                        clock_t end = clock();
                        double time = ((double)(end - start)) / CLOCKS_PER_SEC;
                        printf("Compression completed in %.2f seconds\n", time);
                        printCompressionStats(inputFile, outputFile);
                    }
                }
                break;

            case 2:
                printf("Enter compressed file path: ");
                scanf("%255s", inputFile);
                printf("Enter output file path: ");
                scanf("%255s", outputFile);

                if (validateFiles(inputFile, outputFile) == 0) {
                    clock_t start = clock();
                    if (decompressFile(inputFile, outputFile) == 0) {
                        clock_t end = clock();
                        double time = ((double)(end - start)) / CLOCKS_PER_SEC;
                        printf("Decompression completed in %.2f seconds\n", time);
                    }
                }
                break;

            case 3:
                printf("Enter original file path: ");
                scanf("%255s", inputFile);
                printf("Enter compressed file path: ");
                scanf("%255s", outputFile);
                printCompressionStats(inputFile, outputFile);
                break;

            case 4:
                printf("Thank you for using Huffman Coding Tool!\n");
                return;

            default:
                printf("Invalid choice. Please enter 1-4.\n");
        }
    }
}

/**
 * Main function with command-line argument parsing
 */
int main(int argc, char* argv[]) {
    printf("Huffman Coding Compression Tool v1.0\n");
    printf("=====================================\n");

    // Interactive menu if no arguments provided
    if (argc == 1) {
        interactiveMenu();
        return 0;
    }

    // Command line argument parsing
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    char* option = argv[1];

    // Help option
    if (strcmp(option, "-h") == 0) {
        printUsage(argv[0]);
        return 0;
    }

    // Compression option
    else if (strcmp(option, "-c") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: Compression requires input and output file paths\n");
            printUsage(argv[0]);
            return 1;
        }

        char* inputFile = argv[2];
        char* outputFile = argv[3];

        if (validateFiles(inputFile, outputFile) != 0) {
            return 1;
        }

        clock_t start = clock();
        int result = compressFile(inputFile, outputFile);
        clock_t end = clock();

        if (result == 0) {
            double time = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Compression completed in %.2f seconds\n", time);
            printCompressionStats(inputFile, outputFile);
        }

        return result;
    }

    // Decompression option
    else if (strcmp(option, "-d") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: Decompression requires input and output file paths\n");
            printUsage(argv[0]);
            return 1;
        }

        char* inputFile = argv[2];
        char* outputFile = argv[3];

        if (validateFiles(inputFile, outputFile) != 0) {
            return 1;
        }

        clock_t start = clock();
        int result = decompressFile(inputFile, outputFile);
        clock_t end = clock();

        if (result == 0) {
            double time = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Decompression completed in %.2f seconds\n", time);
        }

        return result;
    }

    // Statistics option
    else if (strcmp(option, "-s") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: Statistics requires original and compressed file paths\n");
            printUsage(argv[0]);
            return 1;
        }

        printCompressionStats(argv[2], argv[3]);
        return 0;
    }

    // Invalid option
    else {
        fprintf(stderr, "Error: Unknown option '%s'\n", option);
        printUsage(argv[0]);
        return 1;
    }
}
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAX_CODE_LENGTH 256
#define ASCII_SIZE 256
#define MAGIC_NUMBER 0x48554646  // "HUFF" in hex

// Huffman Tree Node Structure
typedef struct HuffmanNode {
    unsigned char character;
    unsigned int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

// Min-Heap Structure for Priority Queue
typedef struct MinHeap {
    int size;
    int capacity;
    HuffmanNode **nodes;
} MinHeap;

// Huffman Code Table Entry
typedef struct CodeEntry {
    char code[MAX_CODE_LENGTH];
    int length;
} CodeEntry;

// File Header Structure
typedef struct FileHeader {
    uint32_t magic;
    uint32_t original_size;
    uint32_t compressed_size;
    uint32_t frequency_count;
    uint8_t padding_bits;
} FileHeader;

// Function Declarations

// Memory Management
HuffmanNode* createNode(unsigned char character, unsigned int frequency);
void destroyTree(HuffmanNode* root);
void destroyHeap(MinHeap* heap);

// Min-Heap Operations
MinHeap* createMinHeap(int capacity);
void minHeapify(MinHeap* heap, int index);
HuffmanNode* extractMin(MinHeap* heap);
void insertMinHeap(MinHeap* heap, HuffmanNode* node);
int isHeapEmpty(MinHeap* heap);

// Frequency Calculation
int calculateFrequencies(const char* filename, unsigned int frequencies[ASCII_SIZE]);

// Huffman Tree Construction
HuffmanNode* buildHuffmanTree(unsigned int frequencies[ASCII_SIZE]);

// Code Generation
void generateCodes(HuffmanNode* root, CodeEntry codes[ASCII_SIZE], char* currentCode, int depth);
void buildCodeTable(HuffmanNode* root, CodeEntry codes[ASCII_SIZE]);

// Compression
int compressFile(const char* inputFile, const char* outputFile);
void writeFileHeader(FILE* file, FileHeader* header);
void writeFrequencies(FILE* file, unsigned int frequencies[ASCII_SIZE]);
void encodeAndWrite(FILE* inputFile, FILE* outputFile, CodeEntry codes[ASCII_SIZE]);

// Decompression
int decompressFile(const char* inputFile, const char* outputFile);
int readFileHeader(FILE* file, FileHeader* header);
int readFrequencies(FILE* file, unsigned int frequencies[ASCII_SIZE], int count);
void decodeAndWrite(FILE* inputFile, FILE* outputFile, HuffmanNode* root, 
                   unsigned int originalSize, int paddingBits);

// Utility Functions
void printCompressionStats(const char* inputFile, const char* outputFile);
void printHuffmanCodes(CodeEntry codes[ASCII_SIZE]);
void printUsage(const char* programName);
int validateFiles(const char* inputFile, const char* outputFile);

// Bit Operations
typedef struct BitBuffer {
    unsigned char buffer;
    int bitCount;
} BitBuffer;

void writeBit(FILE* file, BitBuffer* bitBuffer, int bit);
void flushBits(FILE* file, BitBuffer* bitBuffer);
int readBit(FILE* file, unsigned char* currentByte, int* bitPosition);

#endif // HUFFMAN_H
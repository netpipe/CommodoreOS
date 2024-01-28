#include <stdio.h>
#include "dictionary_compression.h"

int main() {
    const char* original_text = "ABABABACDCDCDCDEFGH";
    uint8_t compressed[50]; // Make sure this buffer is large enough
    char decompressed[50];

    int compressed_size = dictionaryCompress(original_text, compressed);
    dictionaryDecompress(compressed, decompressed);

    printf("Original text: %s\n", original_text);
    printf("Compressed data: ");
    for (int i = 0; i < compressed_size; i++) {
        printf("%d ", compressed[i]);
    }
    printf("\n");
    printf("Decompressed text: %s\n", decompressed);

    return 0;
}


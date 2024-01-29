#include <stdint.h>
#include <string.h>

void xor_cipher(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len) {
    for (size_t i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

int main() {
    uint8_t data[] = "Hello World"; // Data to be encrypted
    uint8_t key[] = {0x12, 0x34, 0x56, 0x78}; // Encryption key

    xor_cipher(data, strlen(data), key, sizeof(key)); // Encrypt
    xor_cipher(data, strlen(data), key, sizeof(key)); // Decrypt (same as encryption)

    return 0;
}

